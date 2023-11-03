#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include "../udp.h"
#include "../msg.h"
#include "../utils.h"

static void* async_receive_message(void* args);

typedef struct{
    int socket;
    struct StopAndWaitMessage* recv;
    struct sockaddr* src_addr;
    socklen_t addr_len;
} rx_args;

int init_server(struct StopAndWaitMessage* recv, 
    struct StopAndWaitMessage* ack, 
    int* socket, 
    struct sockaddr_in* client_addr, 
    socklen_t* addr_size, 
    FILE* fp);

static void clean_thread(void* args);

#define CONNECTION_CLOSE_TIMEOUT 3000000 // 3 second
#endif