#include "udp_server.h"

bool finished = false;
pthread_mutex_t mux1 = PTHREAD_MUTEX_INITIALIZER;

static void clean_thread(void* args){
    free(args);
    return;
}

static void* async_receive_message(void* args){
    rx_args* arg = (rx_args*)args;
    int* ret = (int*) malloc(sizeof(int));
    pthread_cleanup_push(clean_thread,ret);
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
    pthread_exit(ret);
}

int init_server(struct StopAndWaitMessage* recv, 
    struct StopAndWaitMessage* ack, 
    int* socket, 
    struct sockaddr_in* client_addr, 
    socklen_t* addr_size, 
    FILE* fp){
    int ret = msg_receive_message(socket, &recv, (struct sockaddr*)&client_addr, addr_size);

    while(ret == -1){
        // Get one correct message
        msg_send_message(socket, ack, (struct sockaddr*)&client_addr);
        ret = msg_receive_message(socket, recv, client_addr, addr_size);
    }
    
    ack->header.ack = !recv->header.seq_num;
    fwrite(recv->data,1,recv->header.data_size,fp);
    msg_send_message(socket,ack,client_addr);
}

int main(int argc, char* argv[]){
    printf("Starting server\n");
    int socket = udp_get_sock(AF_INET, UDP_SERVER_PORT, INADDR_ANY);
    struct sockaddr_in client_addr;
    struct StopAndWaitMessage recv;
    struct StopAndWaitMessage ack;
    pthread_t rx_tread;

    uint8_t dummy_data = 0;
    ack.data = &dummy_data;
    ack.header = msg_create_header(1,0,1,utils_calculate_32crc(CRC_DIVISOR,&dummy_data,1));
    recv.data = (uint8_t*)malloc(MSG_MAX_DATA_SIZE);
    recv.header = msg_create_header(0,0,MSG_MAX_DATA_SIZE,0);

    FILE* fp = utils_open_file(argv[1],"a");
    socklen_t addr_size = sizeof(client_addr);

    init_server(&recv,&ack,&socket,&client_addr,&addr_size,fp);
    bool prev_seq_num = recv.header.seq_num;

    struct timeval rx_start, now; // Used to close connection after long period of inactivity
    while(true){
        dummy_data = 0;
        rx_args args = {
            .socket = socket,
            .recv = &recv,
            .src_addr = (struct sockaddr*)&client_addr,
            .addr_len = addr_size
        };
        gettimeofday(&rx_start, NULL);
        void* thread_res;
        int ret = 0;
        pthread_create(&rx_tread,NULL,async_receive_message,(void*)&args);
        gettimeofday(&now, NULL);
        bool timed_out = true;
        // Start timed connection
        while(utils_time_diff(&rx_start,&now) < CONNECTION_CLOSE_TIMEOUT){
            pthread_mutex_lock(&mux1);
            if(finished){
                pthread_mutex_unlock(&mux1);
                pthread_join(rx_tread,&thread_res);
                finished = false;
                break;
            }
            pthread_mutex_unlock(&mux1);
            gettimeofday(&now, NULL);
        } 
        // Handle timeout
        pthread_mutex_lock(&mux1);
        if(timed_out){
            printf("Timed out, closing connection\n");
            pthread_cancel(rx_tread);
            pthread_mutex_unlock(&mux1);
            break;
        }
        pthread_mutex_unlock(&mux1);
        ret = *(int*)thread_res;
        free(thread_res);

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