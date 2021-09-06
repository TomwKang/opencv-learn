#ifndef _INIT_TASK_H_
#define _INIT_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"


#include "control_RTOS_Task.h"
#include "minipc_RTOS_Task.h"
#include "main_RTOS_Task.h"
#include "FreeRTOS.h"
#include "task.h"

void start_Task(void*pvParameters);
void Task_Init(void);


#ifdef __cplusplus
}
#endif
#endif


