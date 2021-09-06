#ifndef _MAIN_RTOS_TASK_H_
#define _MAIN_RTOS_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"
#include "init_task.h"
#define MAIN_RTOS_TASK_PERIOD 5			//任务运行周期ms

void main_RTOS_Task(void*p_arg);

#ifdef __cplusplus
}
#endif

#endif


