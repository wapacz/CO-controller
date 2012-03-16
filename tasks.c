
#include "tasks.h"

#include "os.h"
#include "HD44780.h"
#include "ds18x20.h"
#include "ds1307.h"

// used for ReadTemperatureSaveAndDisplay purposes
char buffer[16];
int16_t tempI;
int8_t tempE;

// used for InitModules purposes
DS1307_DATA rtc_ds1307_data;

void Task_InitModules(int data) {

	/*** SETUP ALL PINS ***/
	//...
	
	/*** LCD ***/
	LCD_Initalize();
	LCD_Home();
	//sprintf(buffer, "Hello, i=%d", ++i);
	//LCD_WriteText(buffer);

	/*** Clock DS1307 (TWI/I2C) ***/
	rtc_ds1307_init();
	//second = 0; //TODO: read actual second value from DS

	//every second interrupt
	GICR|=_BV(INT0); //w³¹czenie przerwañ INT0
	MCUCR|=(_BV(ISC01)|_BV(ISC00)); //INT0 reaguje na zbocze rosnace

	// setup clock control bits
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

}

void Task_ReadTemperatureSaveAndDisplay(int data) {

	if(ds18x20_read_temp(&tempI, &tempE) == RES_OK)
	{
		LCD_Clear();
		sprintf(buffer, "Piec: %d.%d%cC", tempI, tempE, 0xDF);
		LCD_WriteText(buffer);
	}	

	OS_Delay(1000, Task_ReadTemperatureSaveAndDisplay, data);
}
