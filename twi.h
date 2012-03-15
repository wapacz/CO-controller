/*
 *  Project: 	 API I2C
 *  Author:  	 Michal Lapacz
 *  Date:    	 2009-02-19
 *  Designation: AVR ATMega16 [F_CPU=12MHz] 
 *  Description: Header file for twi.c
 */
 
#ifndef _twi_h_
#define _twi_h_

/****************************************************************************
 * PREPROCESOR'S DEFINITIONS
 */

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  				0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  				1       // Bit position for LSB of the slave address bits in the init byte.

#define TWI_TRANSMISSION_INTEGRITY_STATE twi_integrity_indic
#define TWI_TRANSMISSION_INTEGRITY_CLEAR twi_integrity_indic = 0
#define TWI_TRANSMISSION_INTEGRITY_FAULT twi_integrity_indic = 0xFF
#define TWI_TRANSMISSION_INTEGRITY_CHECK if(twi_integrity_indic) return RES_FAULT

#define TWI_WAIT 					while(!(TWCR & (1<<TWINT)))
#define TWI_WAIT_FOR_STOP 			while ((TWCR & (1<<TWSTO)));
#define TWI_TWBR 					52        // 100 kHz of TWI while F_CPU = 12MHz
#define TWI_TWPS 					0

#define TWI_NACK 					0
#define TWI_ACK  					1

#define TWI_WITH_STOP  				TRUE
#define TWI_WITHOUT_STOP 			FALSE

#define TWI_MAX_ATTEMPS     		2		// Maximium attempts of restart when lost arbitration

/****************************************************************************
  TWI Result function codes
****************************************************************************/
#define TWI_SEND_START_FAULT   		0x01	// Fault during sending START on TWI
#define TWI_SEND_BYTE_FAULT    		0x02	// Fault during sending BYTE on TWI
#define TWI_RECEIVE_BYTE_FAULT 		0x03	// Fault during receiving BYTE on TWI

/****************************************************************************
  TWI State codes
****************************************************************************/
// General TWI Master staus codes                      
#define TWI_START                  	0x08  	// START has been transmitted  
#define TWI_REP_START              	0x10  	// Repeated START has been transmitted
#define TWI_ARB_LOST               	0x38  	// Arbitration lost

// TWI Master Transmitter staus codes                      
#define TWI_MTX_ADR_ACK            	0x18  	// SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           	0x20  	// SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           	0x28  	// Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          	0x30  	// Data byte has been tramsmitted and NACK received 

// TWI Master Receiver staus codes  
#define TWI_MRX_ADR_ACK            	0x40  	// SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           	0x48  	// SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           	0x50  	// Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          	0x58  	// Data byte has been received and NACK tramsmitted

// TWI Slave Transmitter staus codes
#define TWI_STX_ADR_ACK            	0xA8  	// Own SLA+R has been received; ACK has been returned
#define TWI_STX_ADR_ACK_M_ARB_LOST 	0xB0  	// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
#define TWI_STX_DATA_ACK           	0xB8  	// Data byte in TWDR has been transmitted; ACK has been received
#define TWI_STX_DATA_NACK          	0xC0  	// Data byte in TWDR has been transmitted; NOT ACK has been received
#define TWI_STX_DATA_ACK_LAST_BYTE 	0xC8  	// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received

// TWI Slave Receiver staus codes
#define TWI_SRX_ADR_ACK            	0x60  	// Own SLA+W has been received ACK has been returned
#define TWI_SRX_ADR_ACK_M_ARB_LOST 	0x68  	// Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
#define TWI_SRX_GEN_ACK            	0x70  	// General call address has been received; ACK has been returned
#define TWI_SRX_GEN_ACK_M_ARB_LOST 	0x78  	// Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_ACK       	0x80  	// Previously addressed with own SLA+W; data has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_NACK      	0x88  	// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define TWI_SRX_GEN_DATA_ACK      	0x90  	// Previously addressed with general call; data has been received; ACK has been returned
#define TWI_SRX_GEN_DATA_NACK      	0x98  	// Previously addressed with general call; data has been received; NOT ACK has been returned
#define TWI_SRX_STOP_RESTART       	0xA0  	// A STOP condition or repeated START condition has been received while still addressed as Slave

// TWI Miscellaneous status codes
#define TWI_NO_STATE               	0xF8  	// No relevant state information available; TWINT = “0”
#define TWI_BUS_ERROR              	0x00  	// Bus error due to an illegal START or STOP condition

/****************************************************************************
 * DATA
 */
uint8_t  twi_integrity_indic;	 					// TWI transmission integrity indicator
uint8_t  attempt;                                   // attempt counter for START when ARBITATION lost
uint8_t* msgPtr;									// used in loops
uint8_t* byte_buffer;                               // byte buffer

/****************************************************************************
 * FUNCTIONS
 */
void twi_init();
int8_t twi_send_start();
int8_t twi_send_stop();
int8_t twi_send_addr(uint8_t address);
int8_t twi_send_byte(uint8_t data);
int8_t twi_receive_byte(uint8_t* data, uint8_t ack);

#endif
