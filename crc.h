#ifndef _crc_h_
#define _crc_h_

#include <inttypes.h>

#define CRC_OK 0

uint8_t crc8(uint8_t*, uint8_t);
uint8_t crc16(uint8_t);

#endif
