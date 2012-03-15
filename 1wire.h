#ifndef _1wire_h_
#define _1wire_h_

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "delay.h"
#include "crc.h"
#include "return_codes.h"

#define OW_PIN  PB0
#define OW_IN   PINB
#define OW_OUT  PORTB
#define OW_DDR  DDRB

#define OW_LOW_STATE  0
#define OW_HIGH_STATE (1 << OW_PIN) /* bit set on the OW pin */

#define OW_MATCH_ROM	0x55
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_ROM	0xF0

#define OW_ROMCODE_SIZE 8

#define OW_GET_IN()   ( OW_IN  &    (1 << OW_PIN)  )
#define OW_OUT_LOW()  ( OW_OUT &= (~(1 << OW_PIN)) )
#define OW_OUT_HIGH() ( OW_OUT |=   (1 << OW_PIN)  )
#define OW_DIR_IN()   ( OW_DDR &= (~(1 << OW_PIN)) )
#define OW_DIR_OUT()  ( OW_DDR |=   (1 << OW_PIN)  )

#define COMMAND_READ_ROM   0x33
#define COMMAND_CONVERT_T  0x44
#define COMMAND_READ_SP    0xBE
#define COMMAND_WRITE_SP   0x4E

uint8_t ow_reset(void);
void    ow_bit_write(uint8_t);
uint8_t ow_bit_read(void);
void    ow_byte_write(uint8_t);
uint8_t ow_byte_read(void);
uint8_t ow_rom_search(uint8_t, uint8_t*);
uint8_t ow_command(uint8_t, uint8_t*);
uint8_t ow_input_pin_state(void);
void ow_strong_pullup_line(void);
void ow_normal_line(void);

#endif
