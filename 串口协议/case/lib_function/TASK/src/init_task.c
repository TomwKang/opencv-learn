#include "init_task.h"

//------------------------------------------------------------
//�����б�
//------------------------------------------------------------

//��������
#define CONTROL_TASK_PRIO	2			//�������ȼ�
#define CONTROL_STK_SIZE	256			//�����ջ��С

TaskHandle_t CONTROL_RTOS_Task_Handler;		//������
void control_RTOS_Task(void*p_arg);			//������


#define MINIPC_TASK_PRIO	3			//�������ȼ�
#define MINIPC_STK_SIZE	1024			//�����ջ��С

TaskHandle_t MINIPC_RTOS_Task_Handler;		//������
void minipc_RTOS_Task(void*p_arg);	//������


#define MAIN_TASK_PRIO	3			//�������ȼ�
#define MAIN_STK_SIZE	256			//�����ջ��С

TaskHandle_t MAIN_RTOS_Task_Handler;		//������
void main_RTOS_Task(void*p_arg);	//������


#define START_TASK_PRIO	1			//�������ȼ�
#define START_STK_SIZE	128			//�����ջ��С
TaskHandle_t START_Task_Handler;		//������
void start_Task(void*pvParameters);			//������



void start_Task(void*pvParameters) {
	taskENTER_CRITICAL();//�����ٽ���

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
	taskENTER_CRITICAL();//�����ٽ���

	xTaskCreate((TaskFunction_t)start_Task,
		(const char*)"start_Task",
		(uint16_t)START_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)START_TASK_PRIO,
		(TaskHandle_t*)&START_Task_Handler);

	
		taskEXIT_CRITICAL();
	
}








