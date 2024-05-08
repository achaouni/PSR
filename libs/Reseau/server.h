#ifndef server
#define server


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <getopt.h>
#include <string.h>
#include "../IPC/Partage_memoire.h"
#include "../../sioux/analyse_http.h"
#include "../../sioux/multi_threads.h"


void boucle_Serveur(int Server_sckt, void *(*traitement)(void *));
int Initialisation_serveur(short int *port, int Max_connexions);

#endif