#ifndef UDP_H
#define UDP_H
#include "common.h"
#define UDP_SERVER_PORT 5350
#define UDP_CLIENT_PORT 5351
#define TIMEOUT 1E5 // timeout in microseconds
#define BITFLIP_PROB 0.0 // probability of bit flip
#define LOST_PROB 0.1 // probability of losing a message
int udp_rx(void* buf, int buf_size, const int socketfd, struct sockaddr* const in_addr, socklen_t addr_len);
int udp_tx(void* data, int data_size, int sockfd, struct sockaddr* const out_addr, socklen_t addr_len);
int udp_get_sockaddr(const int protocol_family, const char* host_name, const int port, const void* sock_addr);
int udp_get_sock(int protocol_family, int port, void* sock_addr);
#endif