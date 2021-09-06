#ifndef _CONTROL_RTOS_TASK_H_
#define _CONTROL_RTOS_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"
#include "init_task.h"
#define CONTROL_RTOS_TASK_PERIOD 1			//任务运行周期ms

void control_RTOS_Task(void*p_arg);
void resetTimeForDelay();
uint16_t getTimeForDelay();

uint16_t getClockTick();

#ifdef __cplusplus
}
#endif
#endif


