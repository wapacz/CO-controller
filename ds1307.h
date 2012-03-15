/*
 *  Project: 	 API RTC DS1307
 *  Author:  	 Michal Lapacz
 *  Date:    	 2009-09-18
 *  Designation: AVR ATMega16 [F_CPU=16MHz] 
 *  Description: Header file for ds1307.c
 */
 
#ifndef _ds1307_h_
#define _ds1307_h_

/****************************************************************************
 * COMPILATOR'S LIBRARIES
 */
#include <stdint.h>

/****************************************************************************
 * PREPROCESOR'S DEFINITIONS
 */
#define DS1307_ADDRESS 0x68

#define SECONDS_BYTE   0x00
#define MINUTES_BYTE   0x01
#define HOURS_BYTE     0x02
#define DAY_BYTE       0x03
#define DATE_BYTE      0x04
#define MONTH_BYTE     0x05
#define YEAR_BYTE      0x06
#define CONTROL_BYTE   0x07

/****************************************************************************
 * DATA
 */
uint8_t word_address;

 /*
 * This structure coresponds to CLOCK MODE data structure
 */
typedef union DS1307_DATA
{
    uint8_t pure_bytes[8];
    struct 
    {
		/* 0 byte */
        uint8_t  sec_1:4;
		uint8_t  sec_10:3;
		uint8_t  CH:1;
        
		/* 1st byte */
		uint8_t  min_1:4;
		uint8_t  min_10:3;
		uint8_t  :0;
        
		/* 2nd byte */
		uint8_t  hour_1:4;
        uint8_t  hour_10:2;
		/*uint8_t  AM_PM:1;*/
		uint8_t  _12_24:1;
        uint8_t  :0;

		/* 3rd byte */
		uint8_t  day_of_week:3;
		uint8_t  :0;
        
		/* 4th byte */
		uint8_t  day_1:4;
		uint8_t  day_10:2;
		uint8_t  :0;

		/* 5th byte */
		uint8_t  month_1:4;
        uint8_t  month_10:1;
		uint8_t  :0;
		
		/* 6th byte */
		uint8_t  year_1:4;
		uint8_t  year_10:4;
		
		/* 7th byte */
		uint8_t  RS0:1;
		uint8_t  RS1:1;
		uint8_t  :2;
		uint8_t  SQWE:1;
		uint8_t  :2;
		uint8_t  OUT:1;
    };
} DS1307_DATA;

/****************************************************************************
 * FUNCTONS
 */
 
void rtc_ds1307_init();
uint8_t rtc_ds1307_read_data(union DS1307_DATA* data);
uint8_t rtc_ds1307_write_data(union DS1307_DATA* data);
uint8_t rtc_ds1307_read_byte(union DS1307_DATA* data, uint8_t byte);
uint8_t rtc_ds1307_write_byte(union DS1307_DATA* data, uint8_t byte);

#endif
