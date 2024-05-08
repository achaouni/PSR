#include <pthread.h>
#include "server.h"
/*------Programmation de la fonction Initiation du serveur------*/
int Initialisation_serveur(short int *port, int Max_connexions)
{
    int server_socket;
    struct sockaddr_in server_address;
    /* Création du socket*/
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Creation de la socket failed");
        exit(EXIT_FAILURE);
    }
    /* Spécification de l'adresse de la socket et le port */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    // Spécification de l'adress hoste 127.0.0.1
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Spécification du port lu en argument ligne command.
    server_address.sin_port = ntohs(*port);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Erreur d'initiation du socket !");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    /* La fonction listen qui permet de scanner le port pour les sockets*/
    if (listen(server_socket, Max_connexions) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server Sioux is listening on 127.0.0.1:%d\n", *port);
    return server_socket;
}
/*----- Programmation de la méthode BoucleSeveur qui accepte les connections sur le(s) port(s) */
void boucle_Serveur(int Server_sckt, void *(*traitement)(void *))
{
    int client_socket;
    pthread_t thread_client;
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    // Boucle infinie pour scanner les connections au serveur sur le porte.
    while (1)
    {
        read_from_shm();
        client_socket = accept(Server_sckt, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1)
        {
            perror("Accepting connection failed");
            // Continue listening for other connections.
            continue;
        }
        printf("Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        int *client_socket_ptr = (int *)malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        /*------------------- La fonction de gestion du requette HTTP.----------------------*/
        if (pthread_create(&thread_client, NULL, traitement, (void *)client_socket_ptr) != 0)
        {
            perror("Failed to create a thread");
            close(client_socket);
        }
        printf("\033[2J\033[H");
    }
    pthread_join(thread_client, NULL);
}