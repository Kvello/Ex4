#ifndef UDP_H
#define UDP_H
#include "common.h"
#define UDP_SERVER_PORT 5350
#define UDP_CLIENT_PORT 5351

int udp_receive(void* buf, int buf_size, const int socketfd, const void* in_addr, const socklen_t* addr_len);
int udp_transmitt(void* data, int data_size, int sockfd, const void* out_addr, int addr_len, int protocol_family, int ser_port);
int udp_get_sockaddr(const int protocol_family, const char* host_name, const int port, const void* sock_addr);
int udp_get_sock(int protocol_family);
#endif