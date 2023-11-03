#include "udp_client.h"

bool finished = false;
pthread_mutex_t mux1 = PTHREAD_MUTEX_INITIALIZER;

static void clean_thread(void* args){
    free(args);
    return;
}

static void* async_receive_message(void* args){
    msg_rx_args* arg = (msg_rx_args*)args;
    int* ret = (int*) malloc(sizeof(int));
    pthread_cleanup_push(clean_thread, ret);
    *ret = msg_receive_message(arg->socket,arg->recv,arg->src_addr,arg->addr_len);
    if(utils_rand_bool(LOST_PROB)){
        printf("Simulating lost message\n");
        while(1){
            //Block
        }
    }
    pthread_mutex_lock(&mux1);
    finished = true;
    pthread_mutex_unlock(&mux1);
    pthread_cleanup_pop(0);
    pthread_exit(ret);
}

int main(int argc,char* argv[]){
    printf("Starting client\n");
    int sock = udp_get_sock(AF_INET, UDP_CLIENT_PORT, INADDR_ANY);
    struct sockaddr_in server_addr;
    struct StopAndWaitMessage send;
    struct StopAndWaitMessage ack;    
    pthread_t rx_tread;

    ack.data = (uint8_t*) malloc(ACKSIZE);
    memset(ack.data,0,ACKSIZE);
    ack.header = msg_create_header(0,0,ACKSIZE,utils_calculate_32crc(CRC_DIVISOR,ack.data,ACKSIZE));
    send.header = msg_create_header(1,0,0,0);

    udp_get_sockaddr(AF_INET,argv[1],UDP_SERVER_PORT,&server_addr);
    FILE* fp = utils_open_file(argv[2],"r");
    uint8_t* buf =(uint8_t*) malloc(MSG_MAX_DATA_SIZE);

    int read_bytes=0;
    unsigned long long total_bytes=0;
    struct timeval send_start, send_end;
    gettimeofday(&send_start, NULL);
    while((read_bytes=fread(buf,1,MSG_MAX_DATA_SIZE,fp))>0){
        total_bytes += read_bytes;

        int crc = utils_calculate_32crc(CRC_DIVISOR,buf,read_bytes);
        struct StopAndWaitHeader header = msg_create_header(!send.header.seq_num,0,read_bytes,crc);
        send = msg_create_message(header,buf);

        while(ack.header.ack == send.header.seq_num){
            void* thread_res;
            struct timeval p_start, now;
            struct StopAndWaitMessage temp;

            temp.header = ack.header;
            uint8_t temp_data = *ack.data;
            temp.data = &temp_data;
            msg_rx_args args = {
                .socket = sock,
                .recv = &temp,
                .src_addr = (struct sockaddr*)NULL,
                .addr_len = sizeof(server_addr.sin_addr)
            };

            // Simulate lost message
            if((float)rand()/(float)RAND_MAX<BITFLIP_PROB){
                printf("Simulating error\n");
                send.data[0] ^= 0b00000001; // Simulate single bit flip error 
            }

            int ret = msg_send_message(sock,&send, (struct sockaddr*)&server_addr);
            if(ret == -1){
                printf("error in msg_send_message\n");
                exit(1);
            }
            // Start timer
            gettimeofday(&p_start, NULL);
            ret = pthread_create(&rx_tread,NULL,async_receive_message,&args);
            if(ret != 0){
                printf("error in pthread_create\n");
                exit(1);
            }
            // Start timed connection
            gettimeofday(&now, NULL);
            bool timed_out = true;
            while(utils_time_diff(&p_start,&now) < TIMEOUT){
                pthread_mutex_lock(&mux1);
                if(finished){
                    pthread_mutex_unlock(&mux1);
                    pthread_join(rx_tread,&thread_res);
                    finished = false;
                    timed_out = false;
                    break;
                }
                pthread_mutex_unlock(&mux1);
                gettimeofday(&now, NULL);
            }
            // Handle timeout
            pthread_mutex_lock(&mux1);
            if(timed_out){
                printf("Timed out, resending\n");
                pthread_cancel(rx_tread);
                pthread_mutex_unlock(&mux1);
                continue;
            }
            pthread_mutex_unlock(&mux1);
            ret = *(int*)thread_res;
            free(thread_res);

            if(ret == -1){
                printf("msg_receive_message detected an error, resending\n");
            }else{
                // Correctly received ack. Store and continue
                ack.header = temp.header;
                *ack.data = *temp.data;
            }
        }
    }
    free(buf);
    free(ack.data);
    fclose(fp);
    gettimeofday(&send_end, NULL);
    double time = utils_time_diff(&send_start,&send_end);
    printf("Sent %llu bytes in %f micro seconds\n",total_bytes,time);
    printf("That is %.2e bytes per second\n",1e6*((double)total_bytes)/time);
}