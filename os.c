/*
 *  Project: 	 Operating System for AVR 
 *  Author:  	 Michal Lapacz
 *  Date:      	 2012-03-15
 *  Designation: AVR ATMega16 [F_CPU=16MHz] 
 *  Description: Handle tasks.
 */

#include "os.h"

/****************************************************************************
 * GLOBAL DATA
 */
OperatingSystem os;

/****************************************************************************
 * MAIN FUNCTIONS
 */
void OS_Start() {

	// init os
	os.taskQueue   = TaskQueue_Create();
	os.uTimerQueue = TimerQueue_Create();
	os.mTimerQueue = TimerQueue_Create();
	
	// main loop
	while(1) {
		TaskQueue_ExecuteNext(&os.taskQueue);
	}
}

void OS_Execute(void (*execute)(int), int data) {
	TaskQueue_Add(&os.taskQueue, execute, data);
}

void OS_Delay(int delay, void (*execute)(int), int data) {
	TimerQueue_Add(&os.mTimerQueue, delay, execute, data);
	// TODO: add for us
}

