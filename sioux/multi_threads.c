#include "multi_threads.h"

void *Gestion_HTTP_THREAD(void *arg){
    int client_socket = *(int *)arg;
    GesDeSocket_HTTP(client_socket);
    return NULL;
}