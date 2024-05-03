// SPDX-License-Identifier: BSD-2-Clause

#ifndef FAST_CRC32_H
#define FAST_CRC32_H

#include <stdint.h>

uint32_t crc32a_bitwise(uint8_t* buff, uint16_t len);
uint32_t crc32b_bitwise(uint8_t* buff, uint16_t len);
uint32_t crc32a_lookup_table(uint8_t* buff, uint32_t len);
uint32_t crc32b_lookup_table(uint8_t* buff, uint16_t len);
uint32_t crc32a_fast_bytewise(uint8_t* buff, uint32_t len);
uint32_t crc32b_fast_bytewise(uint8_t* buff, uint32_t len);
uint32_t crc32a_fast_nibblewise(uint8_t* buff, uint32_t len);
uint32_t crc32b_fast_nibblewise(uint8_t* buff, uint32_t len);

void crc32a_table_init(uint32_t *table);
void crc32b_table_init(uint32_t *table);

#endif
