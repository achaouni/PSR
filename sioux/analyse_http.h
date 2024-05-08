#ifndef analyse_http
#define analyse_http

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <string.h>
#include "../libs/Reseau/server.h"
#include "sioux.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>


#define MAX_BUFFER 1000000
int GesDeSocket_HTTP(int client_socket_ptr);

#endif
