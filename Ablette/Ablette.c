#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../libs/IPC/Partage_memoire.h"

// Tableau pour stocker les sources
struct SourcePacketCount top_sources[MAX_TOP_SOURCES];

// Indicateur pour indiquer si SIGINT a été reçu
volatile sig_atomic_t sigint_received = 0;

// Initialiser le tableau des sources
void initialize_top_sources()
{
    for (int i = 0; i < MAX_TOP_SOURCES; i++)
    {
        top_sources[i].src_ip[0] = '\0';
        top_sources[i].dst_ip[0] = '\0';
        top_sources[i].packet_count = 0;
        top_sources[i].port = 0;
        top_sources[i].protocol[0] = '\0';
        top_sources[i].direction[0] = '\0';
    }
}

// Fonction pour mettre à jour les sources
void update_top_sources(char *src_ip, int port, char *protocol, char *direction, char *dst_ip)
{
    for (int i = 0; i < MAX_TOP_SOURCES; i++)
    {
        if (strcmp(top_sources[i].src_ip, src_ip) == 0 &&
            strcmp(top_sources[i].dst_ip, dst_ip) == 0 &&
            top_sources[i].port == port &&
            strcmp(top_sources[i].protocol, protocol) == 0)
        {
            top_sources[i].packet_count++;
            return;
        }
    }
    // Si l'adresse IP n'est pas déjà dans la liste, ajoutez-la.
    for (int i = 0; i < MAX_TOP_SOURCES; i++)
    {
        if ((top_sources[i].packet_count == 0))
        {
            strcpy(top_sources[i].src_ip, src_ip);
            top_sources[i].port = port;
            strcpy(top_sources[i].protocol, protocol);
            top_sources[i].packet_count = 1;
            strcpy(top_sources[i].direction, direction);
            strcpy(top_sources[i].dst_ip, dst_ip);
            return;
        }
    }

    // Si la liste est pleine, recherchez la source ayant le moins de paquets et remplacez-la
    int min_index = 0;
    for (int i = 1; i < MAX_TOP_SOURCES; i++)
    {
        if (top_sources[i].packet_count < top_sources[min_index].packet_count)
        {
            min_index = i;
        }
    }
    strcpy(top_sources[min_index].src_ip, src_ip);
    strcpy(top_sources[min_index].dst_ip, dst_ip);
    top_sources[min_index].port = port;
    strcpy(top_sources[min_index].protocol, protocol);
    top_sources[min_index].packet_count = 1;
    strcpy(top_sources[min_index].direction, direction);
}

void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    struct ip *ip_header;
    struct tcphdr *tcp_header;

    // Skip Ethernet header.
    ip_header = (struct ip *)(packet + 14);

    if (ip_header->ip_p == IPPROTO_TCP)
    {
        tcp_header = (struct tcphdr *)(packet + 14 + (ip_header->ip_hl << 2)); // Skip IP header

        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];

        // Convertir les adresses IP en format lisible
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        // Déterminer la direction du paquet et garder seulement les paquettes a destination de l'interface en paramaètre.
        char direction[20];
        if (ntohs(tcp_header->th_sport) < 1000)
        {
            /*
                Well-known ports (0-1023): These are reserved for system services and applications
                commonly used by the operating system.
                Examples include HTTP (80), HTTPS (443), FTP (21), etc.
            */
            /*
                Si le port source est l'un des ports reservés donc la paquette est reçu.
                La paquette est envoyée si le port est parmis les ports destiner et alloué dynamiquement par le OS
                Dynamic or Private ports (49152-65535)!
            */
            strcpy(direction, "Received");
            // Mettre à jour la liste des sources
            update_top_sources(src_ip, ntohs(tcp_header->th_dport), "TCP", direction, dst_ip);
        }
    }
    /* Ecrire les statistiques dans la memoire partagée*/
    write_in_shm(top_sources);

    // Afficher les 10 principales sources ayant envoyé des paquets
    printf("(Updated) Top 10 Sources envoyent le plus de paquets :\n");
    for (int i = 0; i < MAX_TOP_SOURCES; i++)
    {
        if (top_sources[i].packet_count > 0)
        {
            printf("Source IP: \033[0;32m%s\033[0m, Destination IP: \033[0;33m%s\033[0m, "
                   "Packets Sent: \033[0;33m%d\033[0m, Port: \033[0;32m%d\033[0m, Protocol: \033[0;32m%s\033[0m, "
                   "Direction: \033[0;33m%s\033[0m\n",
                   top_sources[i].src_ip, top_sources[i].dst_ip, top_sources[i].packet_count,
                   top_sources[i].port, top_sources[i].protocol, top_sources[i].direction);
        }
    }
    // Norme ANSI code pour effacer l'écran après affichage et mise a jour(Pour Linux/UNIX)
    printf("\033[2J\033[H");
}

int main(int argc, char *argv[])
{
    char *dev = "";
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    char *filter_expression = "";
    char *ports = "";

    // Options de ligne de commande pour personnaliser les ports
    int option;
    while ((option = getopt(argc, argv, "p:i:")) != -1)
    {
        switch (option)
        {
        case 'p':
            // L'option -p est utilisée pour spécifier des ports personnalisés.
            ports = optarg;
            break;
        case 'i':
            // L'option -i est utilisée pour spécifier l'interface.
            dev = optarg;
            break;
        default:
            fprintf(stderr, "Utilisation : %s [-p port1,port2] -i interface\n", argv[0]);
            exit(1);
        }
    }

    if (optind < argc)
    {
        dev = argv[optind];
    }

    // Ouvrir l'interface réseau en mode promiscuité.
    printf("En train de capturer les paquets, veuillez attendre...\n");
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL)
    {
        fprintf(stderr, "Impossible d'ouvrir l'interface %s : %s\n", dev, errbuf);
        return 2;
    }

    // Créer une expression de filtre basée sur les ports personnalisés
    char filter[100];
    // Ajustez la taille selon vos besoins
    snprintf(filter, sizeof(filter), "tcp port %s", filter_expression);

    // Construct the filter expression based on multiple ports separated by a comma.
    if (strlen(ports) > 0)
    {
        char *token = strtok(ports, ",");
        char port_filter[50];
        snprintf(port_filter, sizeof(port_filter), "tcp port %s", token);
        snprintf(filter, sizeof(filter), "(%s)", port_filter);

        while ((token = strtok(NULL, ",")) != NULL)
        {
            snprintf(port_filter, sizeof(port_filter), " or tcp port %s", token);
            strncat(filter, port_filter, sizeof(filter) - strlen(filter) - 1);
        }
    }
    else
    {
        snprintf(filter, sizeof(filter), "tcp");
    }

    // Compiler et appliquer le filtre.
    struct bpf_program fp;
    if (pcap_compile(handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        fprintf(stderr, "Erreur lors de la compilation du filtre\n");
        return 2;
    }
    if (pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr, "Erreur lors de la définition du filtre\n");
        return 2;
    }

    // Initialisez le tableau des sources
    initialize_top_sources();

    // Commencez à capturer des paquets et appelez packet_handler pour chaque paquet.
    pcap_loop(handle, -1, packet_handler, NULL);

    // Afficher les 5 principales sources ayant envoyé des paquets
    printf("Top 5 Sources Sending the Most Packets:\n");
    for (int i = 0; i < MAX_TOP_SOURCES; i++)
    {
        if (top_sources[i].packet_count > 0)
        {
            printf("Source IP: \033[0;32m%s\033[0m, Destination IP: \033[0;33m%s\033[0m, "
                   "Packets Sent: \033[0;33m%d\033[0m, Port: \033[0;32m%d\033[0m, Protocol: \033[0;32m%s\033[0m, "
                   "Direction: \033[0;33m%s\033[0m\n",
                   top_sources[i].src_ip, top_sources[i].dst_ip, top_sources[i].packet_count,
                   top_sources[i].port, top_sources[i].protocol, top_sources[i].direction);
        }
    }
    // Fermez la session de capture.
    pcap_close(handle);
    return 0;
}