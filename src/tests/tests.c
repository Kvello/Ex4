#include "../msg.h"

int main(void)
{
    printf("Hello World!\n");
    uint64_t crc = CRC_DIVISOR;
    uint8_t data1[6] = {0b00000001, 0b00000010, 0b00000011, 0b00000100, 0b00000101, 0b00000000};
    uint8_t data2[7] = {0b00001111, 0b10111000, 0b00011101, 0b11000001, 0b00000100, 0b00000001, 0b00000000};
    uint64_t data3 = CRC_DIVISOR;

    // printf("data1_int: %lu\n", data1_int);
    uint8_t *data3_char = (uint8_t *)&(data3);
    for (int i = 0; i < 8; i++)
    {
        printf("data1_char[%d]: %d\n", i, data3_char[i]);
    }
    unsigned int num = 1;
    char *byte = (char *)&num;

    if (*byte == 1)
    {
        printf("Little-endian\n");
    }
    else
    {
        printf("Big-endian\n");
    }
    struct StopAndWaitHeader header1 = msg_create_header(0, 0, 6, 0);
    struct StopAndWaitMessage message4 = msg_create_message(header1, data1);

    uint32_t remainder1 = utils_calculate_32crc(crc, data1, 6);
    uint32_t remainder2 = utils_calculate_32crc(crc, data2, 6);
    uint32_t remainder3 = utils_calculate_32crc(crc, data3_char, 8);
    uint32_t remainder4 = msg_calculate_32crc_little_edian(crc, message4);
    printf("Remainder1: %d\n", remainder1);
    printf("Remainder2: %u\n", remainder2);
    printf("Remainder3: %d\n", remainder3);
    printf("Remainder4: %u\n", remainder4);
    message4.header.crc = remainder4;
    remainder4 = msg_calculate_32crc_little_edian(crc, message4);
    printf("Remainder4 after crc inclusion: %u\n", remainder4);
    return 0;
}