#include "udp_client.h"

int get_udp_socket(char* host_name, int port, int protocol_family){
    int sockfd, ret;
    if((sockfd = socket(protocol_family,SOCK_DGRAM,0))==-1){
        printf("error in socket");
        exit(1);
    }
    if(protocol_family == AF_INET6){
        struct sockaddr_in6 ser_addr;
        inet_pton(AF_INET6,host_name,&ser_addr.sin6_addr);
        ser_addr.sin6_port = htons(port);
        ser_addr.sin6_family = AF_INET6;
        ret = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in6));         //connect the socket with the host
        if (ret != 0) {
            printf ("connection failed\n"); 
            close(sockfd); 
            exit(1);
        }
        return sockfd;
    }else if(protocol_family == AF_INET){
        struct sockaddr_in ser_addr;
        inet_pton(AF_INET,host_name,&ser_addr.sin_addr);
        ser_addr.sin_port = htons(port);
        ser_addr.sin_family = AF_INET;
        ret = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in));         //connect the socket with the host
        if (ret != 0) {
            printf ("connection failed\n"); 
            close(sockfd); 
            exit(1);
        }
        return sockfd;
    }else{
        printf("protocol family %c not supported\n", protocol_family);
        exit(1);
    }
}
FILE* open_file(char* file_name){
    FILE *fp;
    if((fp = fopen(file_name,"r"))==NULL){
        printf("error in open file");
        exit(1);
    }
    return fp;
}
int transmitt(char* data, int data_size, int sockfd){
    int ret;
    
    uint32_t crc = calculate_32crc(CRC_DIVISOR,data,data_size,true);
    struct StopAndWaitMessage msg = create_message(create_header(false,false,data_size,0),data);

    if((ret = send(sockfd,data,data_size,0))==-1){
        printf("error in send");
        exit(1);
    }
    return ret;
}
int main(int argc,char** argv){
    
}