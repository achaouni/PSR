/*
   GESTION MEMOIRE PARTAGÉE
   - LECTURE
   - ECRITURE
*/
#include "Partage_memoire.h"
#define SHM_KEY 1234

/* Pour écrire les statistiques issus de Ablette dans un fichier Ablette.html*/
void write_to_html(char *content)
{
   // Nom du fichier HTML
   char *nomFichier = "Ablette.html";

   // Ouvrir le fichier en mode d'écriture
   // Utilisez "w" à la place de "a" pour écraser le fichier existant
   FILE *fichier = fopen(nomFichier, "w");

   // Vérifier si le fichier a été ouvert avec succès
   if (fichier == NULL)
   {
      fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
      return; // Arrêter la fonction en cas d'erreur
   }

   // Écrire le contenu dans le fichier HTML
   fprintf(fichier, "%s\n", content);

   // Fermer le fichier
   fclose(fichier);

   printf("Des statistiques ont été ajoutées à la page %s\n", nomFichier);
}

/* Lire de la mémoire partagée */
void read_from_shm()
{
   // générer clé unique
   key_t key = ftok("shmfile", 65);

   // returns ID for the shared memory segment
   int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

   // s'attacher à la mémoire partagée
   char *str = (char *)shmat(shmid, NULL, 0);

   printf("Packets reçus de Ablette: \n%s", str);
   // Écrire le contenu dans le fichier HTML
   write_to_html(str);

   shmdt(str); // se déttacher de la memoire partagée.
}

/* Ecrire les statistiques dans la mémoire partagée */
void write_in_shm(struct SourcePacketCount top_sources[MAX_TOP_SOURCES])
{
   key_t key = ftok("shmfile", 65); // générer clé unique

   int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
   // returns ID for the shared memory segment

   // s'attacher à la mémoire partagée
   char *str = (char *)shmat(shmid, NULL, 0);
   /* Partie stockage dans la memoire partagée*/
   str[0] = '\0'; // Raffraîchir la mémoire pour chaque execution

   for (int i = 0; i < MAX_TOP_SOURCES; i++)
   {
      if (top_sources[i].packet_count > 0)
      {
         strcat(str, "Source IP: ");
         strcat(str, top_sources[i].src_ip);
         strcat(str, " | ");
         strcat(str, " Destination IP: ");
         strcat(str, top_sources[i].dst_ip);
         strcat(str, " | ");
         strcat(str, " Direction: ");
         strcat(str, top_sources[i].direction);
         strcat(str, "\n");
      }
   }

   printf("(Updated) Top 10 packets stockés dans la mémoire! \n");
   shmdt(str); // se détacher de la mémoire
}
