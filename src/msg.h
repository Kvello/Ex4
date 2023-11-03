#ifndef MSG_H
#define MSG_H
#include "common.h"
#include "utils.h"
#include "udp.h"
#include <math.h>
#define MSG_HEADER_SIZE sizeof(struct StopAndWaitHeader)
#define MSG_MAX_DATA_SIZE 1024
#define MSG_MAX_SIZE MSG_HEADER_SIZE + MSG_MAX_DATA_SIZE
// #define CRC_DIVISOR 0x104C11DB7 // CRC-32 polynomial
struct StopAndWaitHeader
{
    /*
        A simple header struct for stop and wait protocol
        inspired by the ethernet frame

        seq_num:    1 bit sequence number
        ack:        1 bit ack flag, if this is set the seq_num holds the sequence number of the next expected message
        data_size:  2 bytes, the size of the data in bytes
        crc:        4 bytes, the crc of the message
        This implementation assumes little endian if proper crc is to be used
        An alternative usage is provided in the function calculate_32crc, which can work with big endian aswell
    */
    uint32_t crc;
    bool seq_num;
    bool ack;
    uint16_t data_size;
};
struct StopAndWaitMessage
{
    /*
        A simple message struct for stop and wait protocol
        inspired by the ethernet frame

        header:     the header of the message
        data:       the data of the message
    */
    struct StopAndWaitHeader header;
    u_int8_t *data;
};
struct StopAndWaitHeader msg_create_header(bool seq_num, bool ack, u_int16_t data_size, u_int32_t crc);
struct StopAndWaitMessage msg_create_message(struct StopAndWaitHeader header, u_int8_t *data);
int msg_send_message(int socket, struct StopAndWaitMessage*message, struct sockaddr *address);
int msg_receive_message(int sock,struct StopAndWaitMesage* recv,struct sockaddr src_addr,int addr_size);
uint32_t msg_calculate_32crc_little_edian(uint64_t crc, struct StopAndWaitMessage message);
#endif