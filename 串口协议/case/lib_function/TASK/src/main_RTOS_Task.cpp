#include "main_RTOS_Task.h"
#include "visualscope.h"                              
#include "minipc_task.h"
#include "imu.h"
#include "flags.h"
#include "usart3.h"
#include "control_task.h"
#include "communication.h"
#include "com_usart.h"


void main_RTOS_Task(void*p_arg) {
	//说是mainTask，但其实只是将原来主函数的语句放在这里了
	portTickType currentTime;
			int forSend=0;
	while (1)
	{
		currentTime = xTaskGetTickCount();	//获取当前系统时间
		//imu_main();
		forSend++;
		ProcUsartData();
				serialChangeData(1,forSend);								//更改第一个子包信息
				serialChangeData(2,forSend);								//更改第二个子包信息
//				serialChangeData(3,forSend);	
				serialSendData();
		
		for(int i=0;i<serialGetNums();i++){
			printf("第%d个元素:",i);
			if(serialGetRecvType(i)=='S'){
				printf("数据类型为:字符串 ");
				if(serialGetRecvFlag(i)=='G')
					printf("传输类型为:获取 ");
				else if(serialGetRecvFlag(i)=='P')
					printf("传输类型为:发送 ");
				printf("发送者为:%s ",serialGetRecvName(i));
				printf("数据信息为:%s\r\n",serialGetRecvStr(i));
			}else{
				printf("数据类型为:整数 ");
				if(serialGetRecvFlag(i)=='G')
					printf("传输类型为:获取 ");
				else if(serialGetRecvFlag(i)=='P')
					printf("传输类型为:发送 ");
				printf("发送者为:%s ",serialGetRecvName(i));
				printf("数据信息为:%d\r\n",serialGetRecvInt(i));				
			}
				
		}
		
		
		//printf("序列号:%d\r\n",serialGetSeq());
		vTaskDelayUntil(&currentTime, MAIN_RTOS_TASK_PERIOD / portTICK_RATE_MS);			//延时
	}
}

