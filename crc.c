/*
 *
 * CRC MODULE
 *
 */

#include "crc.h"

uint8_t crc8(uint8_t *input, uint8_t size)
{
	static uint8_t i, j, operand=0, crc=0;

	for(i=0; i<size; i++)
	{
		for(j=0; j<8; j++)
		{
			operand = ((input[i]>>j)^crc)&0x01;
			crc >>= 1;
			crc = crc|(operand<<7);
			crc = crc^(operand<<3);
			crc = crc^(operand<<2);
		}
	}

	return crc;
}
 
uint8_t crc16(uint8_t input)
{
	static uint8_t i, operand, crc;

	for(i=0; i<8; i++)
	{
		operand = ((input>>i)^crc)&0x01;
		crc >>= 1;
		crc = crc|(operand<<15);
		crc = crc^(operand<<13);
		crc = crc^(operand<<0);
	}

	return crc;
}
