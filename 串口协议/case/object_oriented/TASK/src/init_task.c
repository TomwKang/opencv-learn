#include "init_task.h"

//------------------------------------------------------------
//任务列表
//------------------------------------------------------------

//命令任务
#define CONTROL_TASK_PRIO	2			//任务优先级
#define CONTROL_STK_SIZE	256			//任务堆栈大小

TaskHandle_t CONTROL_RTOS_Task_Handler;		//任务句柄
void control_RTOS_Task(void*p_arg);			//任务函数


#define MINIPC_TASK_PRIO	3			//任务优先级
#define MINIPC_STK_SIZE	1024			//任务堆栈大小

TaskHandle_t MINIPC_RTOS_Task_Handler;		//任务句柄
void minipc_RTOS_Task(void*p_arg);	//任务函数


#define MAIN_TASK_PRIO	3			//任务优先级
#define MAIN_STK_SIZE	256			//任务堆栈大小

TaskHandle_t MAIN_RTOS_Task_Handler;		//任务句柄
void main_RTOS_Task(void*p_arg);	//任务函数


#define START_TASK_PRIO	1			//任务优先级
#define START_STK_SIZE	128			//任务堆栈大小
TaskHandle_t START_Task_Handler;		//任务句柄
void start_Task(void*pvParameters);			//任务函数



void start_Task(void*pvParameters) {
	taskENTER_CRITICAL();//进入临界区

	xTaskCreate((TaskFunction_t)control_RTOS_Task,
		(const char*)"control_RTOS_Task",
		(uint16_t)CONTROL_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)CONTROL_TASK_PRIO,
		(TaskHandle_t*)&CONTROL_RTOS_Task_Handler);
/*
	xTaskCreate((TaskFunction_t)minipc_RTOS_Task,
		(const char*)"minipc_RTOS_Task",
		(uint16_t)MINIPC_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)MINIPC_TASK_PRIO,
		(TaskHandle_t*)&MINIPC_RTOS_Task_Handler);
*/
	xTaskCreate((TaskFunction_t)main_RTOS_Task,
		(const char*)"main_RTOS_Task",
		(uint16_t)MAIN_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)MAIN_TASK_PRIO,
		(TaskHandle_t*)&MAIN_RTOS_Task_Handler);



	vTaskDelete(START_Task_Handler);
	taskEXIT_CRITICAL();
}




void Task_Init(void) {
	taskENTER_CRITICAL();//进入临界区

	xTaskCreate((TaskFunction_t)start_Task,
		(const char*)"start_Task",
		(uint16_t)START_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)START_TASK_PRIO,
		(TaskHandle_t*)&START_Task_Handler);

	
		taskEXIT_CRITICAL();
	
}








