/*
 *  Project: 	 API RTC DS1307 (TWI) 
 *  Author:  	 Michal Lapacz
 *  Date:      	 2009-09-18
 *  Designation: AVR ATMega16 [F_CPU=16MHz] 
 *  Description: Allows communicate on I2C (TWI) interface with RTC DS1307.
 */

/****************************************************************************
 * COMPILATOR'S LIBRARIES
 */

/****************************************************************************
 * PROJECT'S LIBRARIES
 */
#include "ds1307.h"
#include "twi.h"
#include "basic_types.h"
#include "return_codes.h"

/****************************************************************************
 * DATA
 */
uint8_t* msgPtr;									// used in loops
uint8_t* byte_buffer;                               // byte buffer


/****************************************************************************
 * FUNCTONS
 */
void rtc_ds1307_init()
{
	twi_init();
}

uint8_t rtc_ds1307_read_data(union DS1307_DATA* data)
{
	twi_send_start();         						// send START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT)); // send SLA+W 
	twi_send_byte(0x00); 							// send word address
	twi_send_start();          						// send RE-START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT));  // send SLA+R

    /* receive message without last byte */
    for(msgPtr = data->pure_bytes; msgPtr < data->pure_bytes + 7; msgPtr++)
        twi_receive_byte(msgPtr, TWI_ACK);

    twi_receive_byte(msgPtr, TWI_NACK);				// receive last byte
   	twi_send_stop();                                // send STOP

    return TWI_TRANSMISSION_INTEGRITY_STATE;   		// return TWI transsmision integrity result (0 - if OK)
}

uint8_t rtc_ds1307_write_data(union DS1307_DATA* data)
{
	twi_send_start();          						// send START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT)); // send SLA+W
	twi_send_byte(0x00);							// send word address
	
    /* send message */
    for(msgPtr = data->pure_bytes; msgPtr < data->pure_bytes + 8; msgPtr++)
        twi_send_byte(*msgPtr);

   	twi_send_stop();                                // send STOP

    return TWI_TRANSMISSION_INTEGRITY_STATE;   		// return TWI transsmision integrity result (0 - if OK)
}		

uint8_t rtc_ds1307_read_byte(union DS1307_DATA* data, uint8_t byte_address)
{
	twi_send_start();         						// send START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT)); // send SLA+W 
	twi_send_byte(byte_address); 							// send word address
	twi_send_start();          						// send RE-START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT));  // send SLA+R

    /* receive message without last byte */
    msgPtr = data->pure_bytes; 
	msgPtr+=byte_address;
    twi_receive_byte(msgPtr, TWI_NACK);				// receive last byte
   	twi_send_stop();                                // send STOP

    return TWI_TRANSMISSION_INTEGRITY_STATE;   		// return TWI transsmision integrity result (0 - if OK)
}

uint8_t rtc_ds1307_write_byte(union DS1307_DATA* data, uint8_t byte_address)
{
	twi_send_start();          						// send START
	twi_send_addr((DS1307_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT)); // send SLA+W
	twi_send_byte(byte_address);							// send word address
	
    /* send message */
    msgPtr = data->pure_bytes;
	msgPtr+=byte_address;
    twi_send_byte(*msgPtr);

   	twi_send_stop();                                // send STOP

    return TWI_TRANSMISSION_INTEGRITY_STATE;   		// return TWI transsmision integrity result (0 - if OK)
}
