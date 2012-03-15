
#include "ds18x20.h"


uint8_t ds18x20_rom[8];
uint8_t ds18x20_sp[9];

void ds18x20_init()
{
	//only for DS18B20 (not for DS18S20)
	ow_command(COMMAND_WRITE_SP,NULL); // write 
	ow_byte_write(0x00); //Th
	ow_byte_write(0x00); //Tl
	ow_byte_write(0x7F); //config
}

uint8_t ds18x20_read_temp(int16_t *t, int8_t *e) 
{
	uint8_t i;

	if(ow_command(COMMAND_CONVERT_T,NULL) == RES_FAULT) return RES_FAULT;

	ow_strong_pullup_line();
	_delay_ms(1500); //
	ow_normal_line();

	if(ow_command(COMMAND_READ_SP,NULL) == RES_FAULT) return RES_FAULT;

	for (i=0; i<9; i++)	
		ds18x20_sp[i] = ow_byte_read();

	//LCD_Clear();
	if(crc8(ds18x20_sp,9) == CRC_OK)
	{
		*t = ((ds18x20_sp[1] << 8) | (ds18x20_sp[0])) / 16;
		*e = ((ds18x20_sp[0] & M00001111) * 10) / 16;
		//temp = tempI + ((sp[0] & M00001111) * 0.625);
		//sprintf(buffer, "Piec: %d%cC", tempI, 0xDF);
		//sprintf(buffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
		//LCD_WriteText(buffer);
		 return RES_OK;
	}
	else
	{
		//sprintf(buffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
		//LCD_WriteText(buffer);
		//LCD_WriteText("CRC FAILED");
		*t = 127;
		*e = 0;
		return RES_FAULT;
	}
}
