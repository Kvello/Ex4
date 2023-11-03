#include "udp_server.h"

int main(int argc, char** argv){
    int socket = udp_get_sock(AF_INET, UDP_SERVER_PORT, INADDR_ANY);
    struct sockaddr_in client_addr;
    struct StopAndWaitMessage recv;
    struct StopAndWaitMessage ack;
    bool seq_num = 1;
    uint8_t dummy_data = 0;
    ack.data = &dummy_data;
    ack.header = msg_create_header(0,0,8,0);
    uint32_t crc = utils_calculate_32crc(CRC_DIVISOR,&dummy_data,8);
    FILE* fp = utils_open_file(argv[1],"a");
    while(true){
        int ret = msg_receive_message(socket,&recv,(struct sockaddr*)&client_addr,sizeof(client_addr));
        if(ret != -1){
            ack.header = msg_create_header(!seq_num,!recv.header.seq_num,8,crc);
            fwrite(recv.data,1,recv.header.data_size,fp);
        }
        msg_send_message(socket,&ack,(struct sockaddr*)&client_addr);
    }
}