/*
 *  Project: 	 Basic types
 *  Author:  	 Michal Lapacz
 *  Date:    	 2009-02-19
 *  Designation: (none)
 *  Description: DS18x20
 */
 
#ifndef _ds18x20_h_
#define _ds18x20_h_

#include <stdio.h>
#include "1wire.h"
#include "crc.h"
#include "return_codes.h"
#include "basic_types.h"

void ds18x20_init();
uint8_t ds18x20_read_temp(int16_t *t, int8_t *e);

#endif
