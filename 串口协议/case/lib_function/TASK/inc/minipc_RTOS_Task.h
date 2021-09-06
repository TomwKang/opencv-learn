#ifndef MINIPC_RTOS_TASK_H_
#define MINIPC_RTOS_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"
#include "init_task.h"
void minipc_RTOS_Task(void*p_arg);


#define MINIPC_RTOS_TASK_PERIOD 10			//任务运行周期ms
#ifdef __cplusplus
}
#endif
#endif


