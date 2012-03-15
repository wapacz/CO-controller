/*
 *  Project: 	 Return codes
 *  Author:  	 Michal Lapacz
 *  Date:    	 2009-02-19
 *  Designation: (none)
 *  Description: Result codes of functions.
 */
 
#ifndef _return_codes_h_
#define _return_codes_h_

/****************************************************************************
 * COMPILATOR'S LIBRARIES
 */
#include <errno.h>

/****************************************************************************
 * PREPROCESOR'S DEFINITIONS
 */
#define STORE_ERRINFO err_line_number = __LINE__; err_filename = __FILE__; errno_extended 
 
/*
 * Results of functions
 */
#define RES_OK      0
#define RES_FAULT  -1

/****************************************************************************
 * DATA
 */
uint8_t errno_extended; 					// extended error number
int 	err_line_number;					// error line number
char*   err_filename;						// error filename

#endif
