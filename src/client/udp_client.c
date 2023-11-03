#include "udp_client.h"


int main(int argc,char** argv){
    int sock = udp_get_sock(AF_INET, UDP_CLIENT_PORT, INADDR_ANY);
    struct sockaddr_in server_addr;
    udp_get_sockaddr(AF_INET,argv[1],UDP_SERVER_PORT,&server_addr);
    int fp = open_file(argv[2]);
    uint8_t buf[MSG_MAX_DATA_SIZE];
    struct StopAndWaitMessage out_message;
    struct StopAndWaitMessage ack_message;
    ack_message.header = msg_create_header(0,0,0,0);
    out_message.header = msg_create_header(1,0,0,0);
    int read_bytes=0;
    while((read_bytes=read(fp,buf,MSG_MAX_DATA_SIZE))!=EOF){
        int crc = utils_calculate_32crc(CRC_DIVISOR,buf,read_bytes);
        struct StopAndWaitHeader header = msg_create_header(!out_message.header.seq_num,0,read_bytes,crc);
        out_message = msg_create_message(header,buf);
        while(ack_message.header.ack == out_message.header.seq_num){
            int ret = msg_send_message(sock,&out_message, (struct sockaddr*)&server_addr);
            if(ret == -1){
                printf("error in udp_transmitt\n");
                exit(1);
            }
            msg_receive_message(sock,&ack_message,(struct sockaddr* )NULL,sizeof(server_addr));
        }

    }
}