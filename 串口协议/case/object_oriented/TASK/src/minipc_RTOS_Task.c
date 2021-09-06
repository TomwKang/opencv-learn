#include "minipc_RTOS_Task.h"
#include "minipc_task.h"
#include "remote_task.h"
#include "control_task.h"
#include "imu.h"
void minipc_RTOS_Task(void*p_arg) {
	portTickType currentTime;
		int forSend=0;
	while (1)
	{
		currentTime = xTaskGetTickCount();	//获取当前系统时间
		//UpperMonitorDataProcess();
		//		if (Get_Flag(Gimble_ok) == 1) {
		//			miniPC_task();
		//		}
		//forSend++;
	//SendGimbalToMiniPC(forSend,forSend,forSend);
		imu_main();

		vTaskDelayUntil(&currentTime, MINIPC_RTOS_TASK_PERIOD/portTICK_RATE_MS);			//延时
		
	}

}
