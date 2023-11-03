#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>  // Include this header for inet_pton
#include <netinet/in.h> // Include this header for struct sockaddr_in6
#include <stdbool.h>

#define MYUDP_PORT 5350
#define MAX_DATA_BYTES 100 //Maximal number of bytes in a message
// OBS! this means that each message must have a size of a whole number of bytes
#endif