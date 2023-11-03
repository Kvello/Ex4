#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define NUM_BITS_CRC_DIVISOR 33
#define NUM_BYTES_CRC_DIVISOR 5
#define CRC_DIVISOR 0b100000100110000010001110110110111
bool utils_is_little_edian();
int utils_findMSBPosition(uint8_t *x, int length, bool little_edian);
uint32_t utils_calculate_32crc(uint64_t crc_divisor, const uint8_t *data, int data_size);
#endif