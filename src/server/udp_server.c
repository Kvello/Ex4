#include "udp_server.h"

int main(int argc, char* argv[]){
    printf("Starting server\n");
    int socket = udp_get_sock(AF_INET, UDP_SERVER_PORT, INADDR_ANY);
    struct sockaddr_in client_addr;
    struct StopAndWaitMessage recv;
    struct StopAndWaitMessage ack;
    uint8_t dummy_data = 0;
    ack.data = &dummy_data;
    ack.header = msg_create_header(1,0,1,utils_calculate_32crc(CRC_DIVISOR,&dummy_data,1));
    uint8_t received_data[MSG_MAX_DATA_SIZE];
    recv.data = received_data;
    recv.header = msg_create_header(0,0,MSG_MAX_DATA_SIZE,0);
    FILE* fp = utils_open_file(argv[1],"a");
    socklen_t addr_size = sizeof(client_addr);
    while(true){
        int ret = msg_receive_message(socket,&recv,(struct sockaddr*)&client_addr,addr_size);
        if(ret != -1){
            ack.header.seq_num = !ack.header.seq_num;
            ack.header.ack = !ack.header.ack;
            fwrite(recv.data,1,recv.header.data_size,fp);
        }
        msg_send_message(socket,&ack,(struct sockaddr*)&client_addr);
    }
}