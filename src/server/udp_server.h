#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>
#include "../udp.h"
#include "../msg.h"
#include "../utils.h"
#include "../msg.h"
#include "../utils.h"
#include "../udp.h"
#define CONNECTION_CLOSE_TIMEOUT 3000000 // 3 second

static void* async_receive_message(void* args);

typedef struct{
    int socket;
    struct StopAndWaitMessage* recv;
    struct sockaddr* src_addr;
    socklen_t addr_len;
} rx_args;
int init_server(struct StopAndWaitMessage* recv, 
    struct StopAndWaitMessage* ack, 
    int socket, 
    struct sockaddr* client_addr, 
    socklen_t addr_size, 
    FILE* fp);
static void clean_thread(void* args);
#endif