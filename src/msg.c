#include "msg.h"

struct StopAndWaitHeader msg_create_header(bool seq_num, bool ack, u_int16_t data_size, u_int32_t crc)
{
    struct StopAndWaitHeader header;
    header.seq_num = seq_num;
    header.ack = ack;
    header.data_size = data_size;
    header.crc = crc;
    return header;
}

struct StopAndWaitMessage msg_create_message(struct StopAndWaitHeader header, u_int8_t *data)
{
    struct StopAndWaitMessage message;
    message.header = header;
    message.data = data;
    return message;
}
uint32_t msg_calculate_32crc_little_edian(uint64_t crc_divisor, struct StopAndWaitMessage message)
{
    /*
        This function calculates the crc of a whole message. It only works for as intended if both
        sender and receiver use little edian.
        The calculated crc should be included in the message. If the crc is correct, the remainder is 0.
    */
    int header_size = sizeof(message.header);
    int data_size = message.header.data_size;
    int size = header_size + data_size;
    uint8_t data[size];
    memcpy(&data, &message.header, header_size);
    memcpy(&data[header_size], message.data, data_size);
    uint32_t remainder = utils_calculate_32crc(crc_divisor, data, size);
    return remainder;
}
int msg_send_message(int socket, struct StopAndWaitMessage* msg, struct sockaddr *address){
    /*
        Sends a message over UDP with the file descriptor sockfd as one message.
        Supports both IPv4 and IPv6.
        Returns the number of bytes sent, or -1 if an error occured.
        @param[in] message: the message to be sent
        @param[in] socket: the file descriptor of the socket to send the data to, an UDP socket
        @param[in] address: the address of the server to send the data to, should be a sockaddr_in or sockaddr_in6
        @return the number of bytes sent, or -1 if an error occured
    */
    uint8_t buf[MSG_MAX_SIZE];
    memcmp(&buf,&(msg->header),MSG_HEADER_SIZE); // copy message to buf, same as memcpy but with
    memcmp(&buf[MSG_HEADER_SIZE],msg->data,msg->header.data_size); // same as above
    int ret; 
    ret = udp_tx(buf, MSG_HEADER_SIZE + msg->header.data_size, socket, address, sizeof(*address));
    return ret;
}
int msg_receive_message(const int sock,struct StopAndWaitMessage* recv,struct sockaddr* src_addr, const socklen_t* addr_size){
    /*
    * Receives a message over UDP from the address in_address with the file descriptor sockfd.
    * Supports both IPv4 and IPv6. The received message is stored in recv. The user must allocate memory for recv.
    * Also the user must set the correct size of the data in recv->header.data_size, reflecting the size of the allocated memory.
    * If more bytes are received than the size of the allocated memory and error(-1) is returned.
    * The function also checks the crc of the received message, and returns -1 if the crc is incorrect.
    * Returns the number of bytes received, or -1 if an error occured.
    * @param[out] recv: the buffer to store the received message
    * @param[in] sockfd: the file descriptor of the socket to receive the message from, an UDP socket
    * @param[in] src_addr: the address of the sender of the message, should be a sockaddr_in or sockaddr_in6
    * @param[in] addr_size: the length of the address of the sender of the message
    * @return the number of bytes received, or -1 if an error occured
    */
    uint8_t buf[MSG_MAX_SIZE];
    int ret;
    ret = udp_rx(buf,MSG_MAX_SIZE,sock,src_addr,addr_size);
    if (ret == -1){
        printf("error in udp_receive\n");
        return -1;
    }
    if (ret < MSG_HEADER_SIZE){
        printf("message too short\n");
        return -1;
    }else if(ret > MSG_HEADER_SIZE+recv->header.data_size){
        printf("message too long\n");
        return -1;
    }
    memcpy(&(recv->header),&buf,MSG_HEADER_SIZE);
    memcpy(recv->data,&buf[MSG_HEADER_SIZE],recv->header.data_size);
    if(utils_calculate_32crc(CRC_DIVISOR,recv->data,recv->header.data_size) != 0){
        printf("crc error\n");
        return -1;
    }

}