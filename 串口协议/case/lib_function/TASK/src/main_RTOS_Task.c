#include "main_RTOS_Task.h"
#include "visualscope.h"                              
#include "minipc_task.h"
#include "imu.h"
#include "flags.h"
#include "usart3.h"
#include "control_task.h"


void main_RTOS_Task(void*p_arg) {
	portTickType currentTime;
	while (1)
	{
		currentTime = xTaskGetTickCount();	//获取当前系统时间
		imu_main();
		//VisualScope(USART3, GMPPositionPID[0].ref, GMPPositionPID[0].fdb, GMPSpeedPID[0].ref, (GET_PITCH_ANGULAR_SPEED));
		//		if (Get_Flag(Gimble_ok) == 1) {
		//			miniPC_task();
		//		}
		vTaskDelayUntil(&currentTime, MAIN_RTOS_TASK_PERIOD/portTICK_RATE_MS);			//延时
	}
}

