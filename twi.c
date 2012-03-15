/*
 *  Project:     API I2C
 *  Author:      Michal Lapacz
 *  Date:        2009-02-19
 *  Designation: AVR ATMega16 [F_CPU=12MHz]
 *  Description: Allows communicate on I2C (TWI) interface.
 *               Without interrupts. Shouldn't be used with many masters on TWI,
 *               which may transceive a lot amount of data.
 */

/****************************************************************************
 * COMPILATOR'S LIBRARIES
 */
#include <avr/io.h>
#include <stdint.h>

/****************************************************************************
 * PROJECT'S LIBRARIES
 */
#include "twi.h"
#include "basic_types.h"
#include "return_codes.h"

/****************************************************************************
 * FUNCTIONS
 */

/*
 * Initialise TWI
 *
 * Params: NONE
 */
void twi_init()
{
	DDRC &= ~(_BV(PC1)|_BV(PC0)); //??

    TWBR = TWI_TWBR;                                // Set bit rate register (Baudrate). Defined in header file.
    TWSR = TWI_TWPS;                                // Driver presumes prescaler to be 00. Defined in header file.
    TWDR = 0xFF;                                    // Default content = SDA released.
    TWCR = (1<<TWEN)|                               // Enable TWI-interface and release TWI pins.
           (0<<TWIE)|(0<<TWINT)|                    // Disable Interupt.
           (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|         // No Signal requests.
           (0<<TWWC);                               //
}

/*
 * Send START message on TWI
 *
 * Params: NONE
 */
int8_t twi_send_start()
{
	TWI_TRANSMISSION_INTEGRITY_CLEAR;				 // clear TWI error indicator

	for(attempt=0; attempt < TWI_MAX_ATTEMPS; attempt++)
	{
		TWCR = (1<<TWEN)|                            // TWI Interface enabled.
			   (0<<TWIE)|(1<<TWINT)|                 // Enable TWI Interupt and clear the flag.
			   (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|      // Initiate a START condition.
			   (0<<TWWC);                            //

		TWI_WAIT;                                    // wait for TWI
		
		switch(TWSR)
		{
			case TWI_START:                          // * START has been transmitted
			case TWI_REP_START:                      // * Repeated START has been transmitted
				return RES_OK;

			case TWI_ARB_LOST:                       // * Arbitration lost
				break;   			                 //    Try resend START

			default:                                 // * Other states
				TWI_TRANSMISSION_INTEGRITY_FAULT;	 // TWI transmission integrity fault
				twi_send_stop();                     //    Send STOP
				errno = TWSR;                        //    Set ERROR CODE
				STORE_ERRINFO = TWI_SEND_START_FAULT;//    Set EXTENDED ERROR CODE
				return RES_FAULT;                    //    Return FAULT
		}
	}

	TWI_TRANSMISSION_INTEGRITY_FAULT;				 // TWI transmission integrity fault
	twi_send_stop();                        		 //    Send STOP
	errno = TWSR;                           		 //    Set ERROR CODE
	return RES_FAULT;                       		 //    Return FAULT
}

/*
 * Send STOP message on TWI
 *
 * Params: NONE
 */
int8_t twi_send_stop()
{
    TWCR = (1<<TWEN)|                               // TWI Interface enabled
           (0<<TWIE)|(1<<TWINT)|                    // Disable TWI Interrupt and clear the flag
           (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|         // Initiate a STOP condition.
		   (0<<TWWC);                               //

    TWI_WAIT_FOR_STOP;                              // wait for STOP from TWI
	
	return RES_OK;
}

/*
 * Send address byte to SLAVE on TWI
 *
 * Params:
 * - data   - address of SLAVE to send on TWI
 */
int8_t twi_send_addr(uint8_t address)
{
	TWI_TRANSMISSION_INTEGRITY_CHECK;

    TWDR = address;                                 // prepare data to send on TWI

    TWCR = (1<<TWEN)|                               // TWI Interface enabled
           (0<<TWIE)|(1<<TWINT)|                    // Enable TWI Interupt and clear the flag to send byte
           (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|         //
           (0<<TWWC);

	TWI_WAIT;                                       // wait for TWI
	
	switch(TWSR)
	{
		case TWI_MTX_ADR_ACK:                       // * SLA+W has been tramsmitted and ACK received
			return RES_OK;

		case TWI_MRX_ADR_ACK:                       // * SLA+R has been tramsmitted and NACK received
			return RES_OK;

		default:                                    // * Other states
			TWI_TRANSMISSION_INTEGRITY_FAULT;	 	//    TWI transmission integrity fault
			twi_send_stop();                        //    Send STOP
			errno = TWSR;                           //    Set ERROR CODE
			STORE_ERRINFO = TWI_SEND_BYTE_FAULT; 	//    Set EXTENDED ERROR CODE
			return RES_FAULT;                       //    Return FAULT
	}	
}


/*
 * Send byte to SLAVE on TWI
 *
 * Params:
 * - data   - data to send on TWI
 */
int8_t twi_send_byte(uint8_t data)
{
	TWI_TRANSMISSION_INTEGRITY_CHECK;

    TWDR = data;                                    // prepare data to send on TWI

    TWCR = (1<<TWEN)|                               // TWI Interface enabled
           (0<<TWIE)|(1<<TWINT)|                    // Enable TWI Interupt and clear the flag to send byte
           (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|         //
           (0<<TWWC);

	TWI_WAIT;                                       // wait for TWI
	
	switch(TWSR)
	{
		case TWI_MTX_DATA_ACK:                      // SLA+W has been tramsmitted and ACK received
			return RES_OK;

		case TWI_MTX_DATA_NACK:                     // * SLA+W has been tramsmitted and NACK received
		default:                                    // * Other states
			TWI_TRANSMISSION_INTEGRITY_FAULT;		//    TWI transmission integrity fault
			twi_send_stop();                        //    Send STOP
			errno = TWSR;                           //    Set ERROR CODE
			STORE_ERRINFO = TWI_SEND_BYTE_FAULT; 	//    Set EXTENDED ERROR CODE
			return RES_FAULT;                       //    Return FAULT
	}	
}

/*
 * Receive byte from SLAVE on TWI (with NACK from MASTER)
 *
 * Params:
 * - byte   - data to send on TWI
 * - ack    - send acknowledge or not (proper values: TWI_ACK or TWI_NACK)
 */
int8_t twi_receive_byte(uint8_t* data, uint8_t ack)
{
	TWI_TRANSMISSION_INTEGRITY_CHECK;

	TWCR = (1<<TWEN)  |                             // TWI Interface enabled
		   (0<<TWIE)  |(1<<TWINT)|                  // Enable TWI Interupt and clear the flag to send byte
		   (ack<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|       // Send acknowledge or not
		   (0<<TWWC);

	TWI_WAIT;                                       // wait for TWI

    *data = TWDR;                                   // store byte received from TWI
	
	switch(TWSR)
	{
		case TWI_MRX_DATA_ACK:                  	// * Data byte has been tramsmitted and ACK received
			return RES_OK;

		case TWI_MRX_DATA_NACK:                 	// * Data byte has been tramsmitted and NACK received
		
		// TODO: it should be rather some kind of result code instead of fault code
		
			if(ack)									//   If this is not last byte, it reqired acknowledge
			{										//   so if SLAVE send NACK it will be EOT
				//??TWI_TRANSMISSION_INTEGRITY_FAULT;//  TWI transmission integrity fault
				twi_send_stop();                    //    Send STOP
				errno = TWSR;                       //    Set ERROR CODE
				STORE_ERRINFO = TWI_RECEIVE_BYTE_FAULT;// Set EXTENDED ERROR CODE
				return RES_FAULT;                   //    Return FAULT
			}
			else									// If this is last byte, it should be NACKed by SLAVE
				return RES_OK;

		default:                                	// * Other states
			TWI_TRANSMISSION_INTEGRITY_FAULT;	    //    TWI transmission integrity fault
			twi_send_stop();                    	//    Send STOP
			errno = TWSR;                       	//    Set ERROR CODE
			STORE_ERRINFO = TWI_RECEIVE_BYTE_FAULT;	//    Set EXTENDED ERROR CODE
			return RES_FAULT;                   	//    Return FAULT
	}	
}

