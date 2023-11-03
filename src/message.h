#ifndef MESSAGE_H
#define MESSAGE_H
#include "common.h"
#include "utils.h"
#include <math.h>
#define CRC_DIVISOR 0b100000100110000010001110110110111
#define NUM_BYTES_CRC_DIVISOR 5
#define HEADER_SIZE sizeof(struct StopAndWaitHeader)
#define NUM_BITS_CRC_DIVISOR 33
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
struct StopAndWaitHeader create_header(bool seq_num, bool ack, u_int16_t data_size, u_int32_t crc);
struct StopAndWaitMessage create_message(struct StopAndWaitHeader header, u_int8_t *data);
int findMSBPosition(uint8_t *x, int length, bool little_edian);
uint32_t calculate_32crc_little_edian(uint64_t crc, struct StopAndWaitMessage message);
uint32_t calculate_32crc(uint64_t crc_divisor, const uint8_t *data, int data_size)

#endif