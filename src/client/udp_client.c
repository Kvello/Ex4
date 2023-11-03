#include "udp_client.h"

int rec = 0;
bool finished = false;
pthread_mutex_t mux1 = PTHREAD_MUTEX_INITIALIZER;
typedef struct{
    int socket;
    struct StopAndWaitMessage* recv;
    struct sockaddr* src_addr;
    socklen_t* addr_len
}async_receive_message_args;

void* async_receive_message(void* args){
    async_receive_message_args* arg = (async_receive_message_args*)args;
    rec = msg_receive_message(arg->socket,arg->recv,arg->src_addr,arg->addr_len);
    pthread_mutex_lock(&mux1);
    finished = true;
    pthread_mutex_unlock(&mux1);
    return NULL;
}

int main(int argc,char** argv){
    pthread_t rx_tread;
    async_receive_message_args rx_args;
    int sock = udp_get_sock(AF_INET, UDP_CLIENT_PORT, INADDR_ANY);
    rx_args.socket = sock;
    rx_args.src_addr = (struct sockaddr*)NULL;
    struct sockaddr_in server_addr;
    udp_get_sockaddr(AF_INET,argv[1],UDP_SERVER_PORT,&server_addr);
    int fp = open_file(argv[2],"r");
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
            struct StopAndWaitMessage temp;
            temp.header = ack_message.header;
            int temp_data = *ack_message.data;
            temp.data = &temp_data;
            rx_args.addr_len = sizeof(server_addr.sin_addr);
            rx_args.recv = &temp;
            // Start timer
            struct timeval *p_start, *now;
            gettimeofday(p_start, NULL);
            ret = pthread_create(&rx_tread,NULL,async_receive_message,(void*)&rx_args);
            if(ret != 0){
                printf("error in pthread_create\n");
                exit(1);
            }
            gettimeofday(now, NULL);
            while(utils_time_diff(p_start,now) < TIMEOUT){
                pthread_mutex_lock(&mux1);
                if(finished){
                    pthread_mutex_unlock(&mux1);
                    pthread_join(rx_tread,NULL);
                    finished = false;
                    break;
                }
                pthread_mutex_unlock(&mux1);
                gettimeofday(now, NULL);
            }
            if(rec == -1){
                printf("error in udp_receive\n");
            }else if(rec == 0){
                printf("timeout\n");
            }else{
                // Correctly received ack. Store and continue
                ack_message.header = temp.header;
                ack_message.data = temp.data;
            }
        }

    }
}