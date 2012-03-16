/*
 *  Project: 	CO controller
 *  Author:  	Michal Lapacz
 *  Date:    	2012-01-05
 *  Designation: AVR ATMega16 [F_CPU=16MHz]
 *  Description: Controls furnace, and other heating devices for example boiler.
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
#include "os.h"
#include "tasks.h"
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
DS1307_DATA rtc_ds1307_data;
volatile uint8_t second;

/****************************************************************************
 * MAIN FUNCTION
 */
int main()
{
	// Initialize
	OS_Execute(Task_InitModules, 0);

	// Add periodic tasks
	OS_Execute(Task_ReadTemperatureSaveAndDisplay, 0);

	// Start operating system
	OS_Start();

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
