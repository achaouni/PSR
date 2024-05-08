#ifndef multi_thread
#define multi_thread

#include "analyse_http.h"
#include "../libs/Reseau/server.h"
extern void *Gestion_HTTP_THREAD(void *arg);
#endif
