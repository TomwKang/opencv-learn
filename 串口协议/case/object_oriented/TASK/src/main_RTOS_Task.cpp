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
	//˵��mainTask������ʵֻ�ǽ�ԭ����������������������
	portTickType currentTime;
			int forSend=0;
	while (1)
	{
		currentTime = xTaskGetTickCount();	//��ȡ��ǰϵͳʱ��
//	imu_main();
		forSend++;
		ProcUsartData();
		myserial.changeData(1,forSend);
		myserial.changeData(2,forSend);
		myserial.comSend();
		
		for(int i=0;i<myserial.getRecv_num();i++){
			printf("��%d��Ԫ��:",i);
			if(myserial.getRecvType(i)=='S'){
				printf("��������Ϊ:�ַ��� ");
				if(myserial.getRecvFlag(i)=='G')
					printf("��������Ϊ:��ȡ ");
				else if(myserial.getRecvFlag(i)=='P')
					printf("��������Ϊ:���� ");
				printf("������Ϊ:%s ",myserial.getRecvName(i));
				printf("������ϢΪ:%s\r\n",myserial.getRecv_str(i));
			}else{
				printf("��������Ϊ:���� ");
				if(myserial.getRecvFlag(i)=='G')
					printf("��������Ϊ:��ȡ ");
				else if(myserial.getRecvFlag(i)=='P')
					printf("��������Ϊ:���� ");
				printf("������Ϊ:%s ",myserial.getRecvName(i));
				printf("������ϢΪ:%d\r\n",myserial.getRecv_int(i));				
			}
				
		}
		
		
		//printf("���к�:%d\r\n",myserial.getRecvRenm());
		//printf("%d\r\n",myserial.isLatestPack());
		vTaskDelayUntil(&currentTime, MAIN_RTOS_TASK_PERIOD / portTICK_RATE_MS);			//��ʱ
	}
}

