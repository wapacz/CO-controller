
#ifndef _os_h_
#define _os_h_

#include "task_manager.h"
#include "tasks.h"

typedef struct st_OperatingSystem {
	TaskQueue taskQueue;
	TimerQueue uTimerQueue;
	TimerQueue mTimerQueue;
} OperatingSystem;

void OS_Start();
void OS_Execute();
void OS_Delay();

#endif
