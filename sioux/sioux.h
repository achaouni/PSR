#ifndef sioux
#define sioux
#include "analyse_http.h"
#include "../libs/Reseau/server.h"
#include "../libs/IPC/Partage_memoire.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <string.h>
#define MAX_LIGNE 1024
#define MAX_CONNEXIONS 5
extern void read_from_shm();
#endif