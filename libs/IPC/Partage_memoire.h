#ifndef Partage_memoire
#define Partage_memoire

#define MAX_TOP_SOURCES 10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "../../sioux/sioux.h"
// Structure pour suivre les sources et le nombre de paquets envoyés
struct SourcePacketCount
{
   char src_ip[16];
   int packet_count;
   int port;
   char protocol[16];
   // Added field for direction
   char direction[20];
   // Adding field to print the destination IP address
   char dst_ip[16];
};

void read_from_shm();

void write_in_shm(struct SourcePacketCount top_sources[MAX_TOP_SOURCES]);

#endif