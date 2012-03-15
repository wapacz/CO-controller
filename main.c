/*
 *  Project: 	CO controller
 *  Author:  	Michal Lapacz
 *  Date:    	2012-01-05
 *  Designation: AVR ATMega16 [F_CPU=16MHz]
 *  Description: Controls CO stone, and other CO devices for example boiler.
 */

/****************************************************************************
 * COMPILATOR'S LIBRARIES
 */
#include <avr/interrupt.h>
#include <stdio.h>


/****************************************************************************
 * PROJECT'S LIBRARIES
 */
#include "main.h"
#include "delay.h"
#include "twi.h"
#include "ds1307.h"
#include "HD44780.h"
#include "1wire.h"
#include "ds18x20.h"
#include "crc.h"
#include "return_codes.h"
#include "basic_types.h"


/****************************************************************************
 * GLOBAL DATA
 */

volatile unsigned int iter, key, port;
char buffer[25];
volatile uint8_t second;
DS1307_DATA rtc_ds1307_data;

/****************************************************************************
 * MAIN FUNCTION
 */

int init()
{
	/*** INITIALIZE OF ALL PINS ***/
	//...
	
	/*** LCD ***/
	LCD_Initalize();
	LCD_Home();
	//sprintf(buffer, "Hello, i=%d", ++i);
	//LCD_WriteText(buffer);

	/*** Clock DS1307 (TWI/I2C) ***/
	rtc_ds1307_init();
	second = 0; //TODO: read actual second value from DS

	//every second interrupt
	GICR|=_BV(INT0); //w³¹czenie przerwañ INT0
	MCUCR|=(_BV(ISC01)|_BV(ISC00)); //INT0 reaguje na zbocze rosnace

	rtc_ds1307_data.RS0 = 0;
	rtc_ds1307_data.RS1 = 0;
	rtc_ds1307_data.SQWE = 1;
	rtc_ds1307_write_byte(&rtc_ds1307_data, CONTROL_BYTE);

	
	/*** Termometer DS18B20 (1WIRE) ***/
	ds18x20_init();

	/*** RELAYS ***/
	//DDRA&=~0x7F;
	//PORTA|=0x7F;


	/*** STATUS DIODES ***/
	/*	
	DDRC|=_BV(PC5);//DATA
	DDRC|=_BV(PC4);//CLK
	PORTC&=~_BV(PC4);//CLK LOW
	PORTC|=_BV(PC5);//DATA HIGH
	_delay_us(50);
	PORTC|=_BV(PC4);//CLK LOW TO HIGH
	*/	
	

	/*** KEYPAD ***/
	DDRB&=~(_BV(PB5)|_BV(PB6)|_BV(PB7)); //3 rows IN
	DDRB|=(_BV(PB3)|_BV(PB4));			 //2 cols OUT 

	PORTB|=(_BV(PB5)|_BV(PB6)|_BV(PB7)); //active pull ups 
	PORTB&=~(_BV(PB3)|_BV(PB4));		 //on rows enabled

	DDRB&=~_BV(PB2); //koñcówka INT2(PB2 w Mega16) - wejœcie
	PORTB|=_BV(PB2); //w³¹czenie pull-upa dla wejœcia INT2
	GICR|=_BV(INT2); //w³¹czenie przerwañ INT2
	MCUCSR&=~_BV(ISC2); //INT2 reaguje na zbocze opadaj¹ce

	/*** GLOBAL THINGS */
	sei();  		 //enable global interrupt


	/*** READY ***/
	LCD_WriteText("Ready");

	return 0;
}

int main()
{
	double temp;
	uint8_t i, rom[8], sp[9];
	int16_t tempI;
	int8_t tempE;

	iter = 0;

	init();

	while(1) 
	{
		if(ds18x20_read_temp(&tempI, &tempE) == RES_OK)
		{
			LCD_Clear();
			sprintf(buffer, "Piec: %d.%d%cC", tempI, tempE, 0xDF);
			LCD_WriteText(buffer);
		}

		if(key!=0) 
		{
			switch(key) {
				case 1:

					ow_reset();

					if(ow_input_pin_state())  // only send if bus is "idle" = high
					{
						ow_byte_write(0x33);
						for (i=0; i<8; i++)	
						{
							rom[i] = ow_byte_read();
						}

						LCD_Clear();
						if(crc8(rom,8)==CRC_OK)
						{
							//for(i=0; i<8; i++)
							//{
								sprintf(buffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", rom[0], rom[1], rom[2], rom[3], rom[4], rom[5], rom[6], rom[7]);
								
							//}

							LCD_WriteText(buffer);
							//LCD_WriteText("CRC OK");
						}
						else
						{
							LCD_WriteText("CRC FAILED");
						}	
					}
					break;


				case 2:

					if(ow_command(COMMAND_CONVERT_T,NULL) == RES_FAULT) break;

					ow_strong_pullup_line();
					_delay_ms(750);
					ow_normal_line();

					if(ow_command(COMMAND_READ_SP,NULL) == RES_FAULT) break;

					for (i=0; i<9; i++)	
						sp[i] = ow_byte_read();

					LCD_Clear();
					if(crc8(sp,9)==CRC_OK)
					{
						temp = 0;
						tempI = 0;

						tempI = (sp[1] << 8) | (sp[0]);
						tempI /= 16;
						//temp = tempI + ((sp[0] & M00001111) * 0.625);
						sprintf(buffer, "Piec: %d%cC", tempI, 0xDF);
						//sprintf(buffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
						LCD_WriteText(buffer);
					}
					else
					{
						//sprintf(buffer, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X", sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
						//LCD_WriteText(buffer);
						LCD_WriteText("CRC FAILED");
					}
					break;					

				case 4:
					
					if(ds18x20_read_temp(&tempI, &tempE) == RES_OK)
					{
						LCD_Clear();
						sprintf(buffer, "Piec: %d%cC", tempI, 0xDF);
						LCD_WriteText(buffer);
					}

					break;

				default:
					LCD_Clear();
					sprintf(buffer, "k=%d, p=%x, i=%d", key, port, iter);
					LCD_WriteText(buffer);
					break;
			}
						
			key = 0;
		}
	}

	return 0;
}

/****************************************************************************
 * INTERRUPTS
 */

// for clock purposes
ISR (SIG_INTERRUPT0)
{
	second++;
	if(second>59)
	{
		//TODO: read time and update LCD
		second = 0;
	}
	LCD_Clear();
	sprintf(buffer, "s=%d", second);
	LCD_WriteText(buffer);
}

// for keypad purposes
ISR (SIG_INTERRUPT2) 
{
	uint8_t temp;

	//wy³¹czenie pull-upa dla wejœcia INT2
	PORTB&=~_BV(PB2);

	key = 0;

	iter++;

	// get PB5-PB7 bits from PINB
	temp = (PINB >> 5) & M00000111;
	switch(temp) 
	{
		case 0x3: //011 - row PB7
			key = 1;
			break;
		case 0x5: //101 - row PB6
			key = 3;
			break;
		case 0x6: //110 - row PB5
			key = 5;
			break;
		default: // other values not valid
			key = 0;
			PORTB|=_BV(PB2);
			//SREG = sreg;
			//sei();
			return;
	}

	//change port B I/O to find column press
	DDRB|=(_BV(PB5)|_BV(PB6)|_BV(PB7));
	DDRB&=~(_BV(PB3)|_BV(PB4));

	//active pull ups on cols enabled
	PORTB&=~(_BV(PB5)|_BV(PB6)|_BV(PB7));
	PORTB|=(_BV(PB3)|_BV(PB4));

	// to settle port
	_delay_us(100); 
	//for(index=0; index<255; index++);

	port = PINB;

	// get PB3, PB4 bits from PINB
	temp = (PINB>>3) & M00000011;
	switch(temp) {
		case 0x1: //01 - col PB4
			key += 0;
			break;
		case 0x2: //10 - col PB3
			key += 1;
			break;
		default:
			key = 0;
			//return;
	}

	//bring original PORTB settings

	//3 ROWS IN, 2 COLS OUT 	
	DDRB&=~(_BV(PB5)|_BV(PB6)|_BV(PB7));
	DDRB|=(_BV(PB3)|_BV(PB4));

	//active pull ups on rows enabled
	PORTB|=(_BV(PB5)|_BV(PB6)|_BV(PB7));
	PORTB&=~(_BV(PB3)|_BV(PB4));

	//ponowne w³¹czenie pull-upa dla wejœcia INT2
	PORTB|=_BV(PB2);	
}
