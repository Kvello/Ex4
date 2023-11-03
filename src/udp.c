#include "udp.h"

int udp_tx(void* data, int data_size, int sockfd, struct sockaddr* const out_addr, socklen_t addr_len){
    /*
    * Sends the data in data over UDP with the file descriptor sockfd as one message.
    * Supports both IPv4 and IPv6.
    * Returns the number of bytes sent, or -1 if an error occured.
    * @param[in] data: the data to be sent
    * @param[in] data_size: the size of the data in bytes
    * @param sockfd[in]: the file descriptor of the socket to send the data to, an UDP socket
    * @param ser_addr[in]: the address of the server to send the data to, should be a sockaddr_in or sockaddr_in6
    * @param protocol_family[in]: the protocol family of the socket, should be AF_INET or AF_INET6
    * @param ser_port[in]: the port of the server to send the data to
    * @return the number of bytes sent, or -1 if an error occured
    */
    int ret; 
    ret = sendto(sockfd,data,data_size,0,out_addr,addr_len);
    printf("sent %d bytes\n", ret);
    return ret;
}
int udp_rx(void* buf, int buf_size, const int socketfd, struct sockaddr* const in_addr, socklen_t addr_len){
    /*
    * Receives a message over UDP from the address in_address with the file descriptor sockfd.
    * Supports both IPv4 and IPv6. The received message is stored in buf.
    * Returns the number of bytes received, or -1 if an error occured.
    * @param[out] buf: the buffer to store the received message
    * @param[in] buf_size: the size of the buffer in bytes
    * @param[in] sockfd: the file descriptor of the socket to receive the message from, an UDP socket
    * @param[in] in_addr: the address of the sender of the message, should be a sockaddr_in or sockaddr_in6
    * @param[in] addr_len: the length of the address of the sender of the message
    * @return the number of bytes received, or -1 if an error occured
    */
    int ret;
    ret = recvfrom(socketfd,buf,buf_size,0,in_addr,&addr_len);
    return ret;
}

int udp_get_sock(int protocol_family, int port, void* sock_addr){
    /*
    * Creates and binds an UDP socket. Returns the file descriptor.
    * Supports both IPv4 and IPv6.
    * @param[in] protocol_family: the protocol family of the socket, should be AF_INET or AF_INET6
    * @param[in] port: the port of the socket
    * @param[out] sock_addr: the socket address to be used in a socket, should be a sockaddr_in or sockaddr_in6
    * @return the file descriptor of the socket
    */
    int sockfd, ret;
    if((sockfd = socket(protocol_family,SOCK_DGRAM,0))==-1){
        printf("error creating in socket");
        exit(1);
    }
    if(protocol_family == AF_INET6){
        struct sockaddr_in6 my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin6_family = AF_INET6;
        my_addr.sin6_port = htons(port);
        my_addr.sin6_addr = *(struct in6_addr*)sock_addr;
        ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in6));
    }else if(protocol_family == AF_INET){
        struct sockaddr_in my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    }else{
        printf("protocol family %c not supported\n", protocol_family);
        exit(1);
    }
    if(ret == -1){
        printf("error in binding");
        exit(1);
    }
    return sockfd;
}
int udp_get_sockaddr(const int protocol_family, const char* host_name, const int port, const void* sock_addr){
    /*
    * Sets up the socket address to be used in a socket. Supports both IPv4 and IPv6.
    * @param[in] protocol_family: the protocol family of the socket, should be AF_INET or AF_INET6
    * @param[in] host_name: the host name of the server to connect to
    * @param[in] port: the port of the server to connect to
    * @param[out] sock_addr: the socket address to be used in a socket, should be a sockaddr_in or sockaddr_in6
    * @return 
    */
    int ret;
    if(protocol_family == AF_INET6){
        struct sockaddr_in6* ser_addr = (struct sockaddr_in6*)sock_addr;
        ret = inet_pton(AF_INET6,host_name,&(ser_addr->sin6_addr));
        ser_addr->sin6_port = htons(port);
        ser_addr->sin6_family = AF_INET6;
    }else if(protocol_family == AF_INET){
        struct sockaddr_in* ser_addr = (struct sockaddr_in*)sock_addr;
        ret = inet_pton(AF_INET,host_name,&(ser_addr->sin_addr));
        ser_addr->sin_port = htons(port);
        ser_addr->sin_family = AF_INET;
    }else{
        printf("protocol family %c not supported\n", protocol_family);
        exit(1);
    }
    if(ret == 0){
        printf("inet_pton failed\n");
    }
    return ret;
}