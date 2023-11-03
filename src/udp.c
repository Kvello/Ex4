#include "udp.h"

int udp_transmitt(char* data, int data_size, int sockfd, const void* out_addr, int addr_len, int protocol_family, int ser_port){
    /*
    * Sends the data in data over UDP with the file descriptor sockfd as one message.
    * Supports both IPv4 and IPv6.
    * Returns the number of bytes sent, or -1 if an error occured.
    * The data is sent as a StopAndWaitMessage, see src/message.h for more information.
    * @param[in] data: the data to be sent
    * @param[in] data_size: the size of the data in bytes
    * @param sockfd[in]: the file descriptor of the socket to send the data to, an UDP socket
    * @param ser_addr[in]: the address of the server to send the data to, should be a sockaddr_in or sockaddr_in6
    * @param protocol_family[in]: the protocol family of the socket, should be AF_INET or AF_INET6
    * @param ser_port[in]: the port of the server to send the data to
    * @return the number of bytes sent, or -1 if an error occured
    */
    int ret; 
    uint32_t crc = calculate_32crc(CRC_DIVISOR,data,data_size);
    struct StopAndWaitMessage msg = create_message(create_header(false,false,data_size,crc),data);
    int msg_size = sizeof(msg);
    ret = sendto(sockfd,&msg,msg_size,0,out_addr,addr_len);
    printf("sent %d bytes\n", ret);
    return ret;
}
int udp_receive(struct StopAndWaitMessage* buf, const int socketfd, const void* in_addr, const socklen_t* addr_len){
    /*
    * Receives a message over UDP with the file descriptor sockfd. The address received from is stored in in_addr.
    * Supports both IPv4 and IPv6. The received message is stored in buf, the data field here need to be big
    * enough for the message, and should be reflected in the data_size field, otherwise an error will be returned(-1).
    * The function checks the crc of the DATA
    * of the message, and compares it to the crc in the header. If they are not equal, the function returns -1.
    * Otherwise it returns the number of bytes received.
    * @param[out] buf: the buffer to store the received message in
    * @param[in] socketfd: the file descriptor of the socket to receive the message from, an UDP socket
    * @param[in] in_addr: the address of the sender, should be a sockaddr_in or sockaddr_in6
    * @param[in] addr_len: the length of the address of the sender
    * @return the number of bytes received, or -1 if an error occured
    */
    char temp_buf[MAX_DATA_BYTES+HEADER_SIZE];
    int ret;
    ret = recvfrom(socketfd,temp_buf,MAX_DATA_BYTES+HEADER_SIZE,0,in_addr,addr_len);
    if(ret == -1){
        printf("error in recvfrom\n");
    }
    if(ret < HEADER_SIZE){
        printf("message too short\n");
        return -1;
    }else if(ret > buf->header.data_size + HEADER_SIZE){
        printf("message too long to be stored in buf\n");
        return -1;
    }

    memcpy(buf,temp_buf,ret);
    uint32_t crc = calculate_32crc(CRC_DIVISOR,buf->data,buf->header.data_size);
    if(crc != buf->header.crc){
        printf("crc does not match\n");
        ret = -1;
    }
    return ret;
}

int udp_get_sock(int protocol_family){
    /*
    * Creates a UDP socket and returns the file descriptor.
    * Supports both IPv4 and IPv6.
    * @param[in] protocol_family: the protocol family of the socket, should be AF_INET or AF_INET6
    * @return the file descriptor of the socket
    */
    int sockfd, ret;
    if((sockfd = socket(protocol_family,SOCK_DGRAM,0))==-1){
        printf("error creating in socket");
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