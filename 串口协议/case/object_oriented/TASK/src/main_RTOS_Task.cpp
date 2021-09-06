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
//	imu_main();
		forSend++;
		ProcUsartData();
		myserial.changeData(1,forSend);
		myserial.changeData(2,forSend);
		myserial.comSend();
		
		for(int i=0;i<myserial.getRecv_num();i++){
			printf("第%d个元素:",i);
			if(myserial.getRecvType(i)=='S'){
				printf("数据类型为:字符串 ");
				if(myserial.getRecvFlag(i)=='G')
					printf("传输类型为:获取 ");
				else if(myserial.getRecvFlag(i)=='P')
					printf("传输类型为:发送 ");
				printf("发送者为:%s ",myserial.getRecvName(i));
				printf("数据信息为:%s\r\n",myserial.getRecv_str(i));
			}else{
				printf("数据类型为:整数 ");
				if(myserial.getRecvFlag(i)=='G')
					printf("传输类型为:获取 ");
				else if(myserial.getRecvFlag(i)=='P')
					printf("传输类型为:发送 ");
				printf("发送者为:%s ",myserial.getRecvName(i));
				printf("数据信息为:%d\r\n",myserial.getRecv_int(i));				
			}
				
		}
		
		
		//printf("序列号:%d\r\n",myserial.getRecvRenm());
		//printf("%d\r\n",myserial.isLatestPack());
		vTaskDelayUntil(&currentTime, MAIN_RTOS_TASK_PERIOD / portTICK_RATE_MS);			//延时
	}
}

