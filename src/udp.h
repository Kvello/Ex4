#ifndef UDP_H
#define UDP_H
#include "common.h"
#include "message.h"
int udp_transmitt(char* data, int data_size, int sockfd, const void* out_addr, int addr_len, int protocol_family, int ser_port);
int udp_receive(struct StopAndWaitMessage* buf, const int socketfd, const void* in_addr, const socklen_t* addr_len);
#endif