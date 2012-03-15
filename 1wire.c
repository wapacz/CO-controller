/*
 *
 * 1-WIRE MODULE
 *
 */

#include "1wire.h"

uint8_t ow_reset(void)
{
	uint8_t error = 0;
	
	OW_OUT_LOW();           /* disable internal pull-up (maybe on from parasite) */
	
	OW_DIR_OUT();           /* pull OW-Pin low for 480us            */
	
	_delay_us(480);
	
	cli();                  /* disable global interrupts            */

	OW_DIR_IN(); /* set Pin as input - wait for clients to pull low */
	
	_delay_us(70);
	
	if(OW_GET_IN() == OW_HIGH_STATE)  /* no presence detect         */
		error = 1;			/* nobody pulled to low, still high     */
	
	sei();                  /* enable global interrupts             */
	
	/* 
	 * after a delay the clients should release the line
	 * and input-pin gets back to high due to pull-up-resistor
	 */
	_delay_us(480-70);
	
	if(OW_GET_IN() == OW_LOW_STATE)	/* short circuit                */
		error = 2;
	
	return error;
}

void ow_bit_write(uint8_t dataBit)
{
	cli();
	
	OW_DIR_OUT(); // drive bus low

	_delay_us(1);  // Recovery-Time wuffwuff was 1

	if(dataBit) OW_DIR_IN(); // if bit is 1 set bus high (by ext. pull-up)
		
	_delay_us(60-1);

	OW_DIR_IN();
	
	sei();	
}

uint8_t ow_bit_read(void)
{
	uint8_t dataBit = 0;

	cli();
	
	OW_DIR_OUT(); // drive bus low

	_delay_us(1);
		
	OW_DIR_IN();

	_delay_us(15-1);
		
	if(OW_GET_IN() == OW_HIGH_STATE) 
		dataBit = 1;  // sample at end of read-timeslot
	
	_delay_us(60-15);
	
	sei();

	return dataBit;
}

void ow_byte_write(uint8_t dataByte)
{
	uint8_t i = 8;
	
	do 
	{
		ow_bit_write(dataByte & 0x01);

		dataByte >>= 1;
	}
	while(--i);
}

uint8_t ow_byte_read(void)
{
	uint8_t i = 8, dataByte = 0;
	
	do 
	{
		dataByte >>= 1;

		if(ow_bit_read()) 
			dataByte |= 0x80;
	}
	while(--i);

	return dataByte;
}

uint8_t ow_command(uint8_t command, uint8_t *id)
{
	uint8_t i;

	ow_reset();

	if(OW_GET_IN())
	{
		if(id) 
		{
			ow_byte_write(OW_MATCH_ROM);			/* to a single device */
			i = OW_ROMCODE_SIZE;
			do 
			{
				ow_byte_write(*id);
				id++;
			}while(--i);
		}
		else 
		{
			ow_byte_write(OW_SKIP_ROM);			/* to all devices */
		}
	
		ow_byte_write(command);

		return RES_OK; /* OK */
	}
	else
	{
		return RES_FAULT;
	}
}

uint8_t ow_input_pin_state()
{
	return OW_GET_IN();
}

void ow_strong_pullup_line(void)
{
    OW_OUT_HIGH();
	OW_DIR_OUT();
}

void ow_normal_line(void)
{
    OW_OUT_LOW();
	OW_DIR_IN();
}

/*
uint8_t ow_rom_search(uint8_t diff, uint8_t *id)
{
	uint8_t i, j, next_diff;
	uint8_t b;
	
	if( ow_reset() ) return 0xFF;	// error, no device found
	
	ow_byte_write( 0xF0 );			// ROM search command
	next_diff = 0x00; //OW_LAST_DEVICE;			// unchanged on last device
	
	i = 8 * 8;					// 8 bytes
	
	do {
		j = 8;					// 8 bits
		do {
			b = ow_bit_read();			// read bit
			if( ow_bit_read() ) 
			{			// read complement bit
				if( b )					// 11
					return 0xFE;			// data error
			}
			else 
			{
				if( !b ) 
				{				// 00 = 2 devices
					if( diff > i || ((*id & 1) && diff != i) ) 
					{
						b = 1;				// now 1
						next_diff = i;			// next pass 0
					}
				}
			}
			ow_bit_write( b );     			// write bit
			*id >>= 1;
			if( b ) *id |= 0x80;			// store bit
			
			i--;
			
		} while( --j );
		
		id++;					// next byte
	
	} while( i );
	
	return next_diff;				// to continue search
}

*/
