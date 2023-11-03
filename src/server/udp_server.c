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
    bool prev_seq_num = 0;
    int ret = msg_receive_message(socket,&recv,(struct sockaddr*)&client_addr,addr_size);
    while(ret == -1){
        // Get one correct message
        msg_send_message(socket,&ack,(struct sockaddr*)&client_addr);
        ret = msg_receive_message(socket,&recv,(struct sockaddr*)&client_addr,addr_size);
    }
    prev_seq_num = recv.header.seq_num;
    ack.header.ack = !recv.header.seq_num;
    msg_send_message(socket,&ack,(struct sockaddr*)&client_addr);
    fwrite(recv.data,1,recv.header.data_size,fp);
    while(true){
        dummy_data = 0;
        ret = msg_receive_message(socket,&recv,(struct sockaddr*)&client_addr,addr_size);
        if ((float)rand()/(float)RAND_MAX < LOST_PROB){
            printf("Simulating lost message\n");
            continue;
        }
        if ( (float)rand()/(float)RAND_MAX < BITFLIP_PROB){
            printf("Simulating error\n");
            ack.data[0] ^= 0b00000001; // Simulate single bit flip error 
        }
        if(ret != -1 && prev_seq_num != recv.header.seq_num){
            // accept the message, and ack it
            prev_seq_num = recv.header.seq_num;
            ack.header.seq_num = !ack.header.seq_num;
            ack.header.ack = !recv.header.seq_num;
            printf("Correctly received %d bytes of data\n",recv.header.data_size);
            fwrite(recv.data,1,recv.header.data_size,fp);
        }
        msg_send_message(socket,&ack,(struct sockaddr*)&client_addr);
    }
}