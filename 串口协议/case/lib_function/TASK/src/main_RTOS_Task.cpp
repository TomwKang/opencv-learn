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
		//imu_main();
		forSend++;
		ProcUsartData();
				serialChangeData(1,forSend);								//���ĵ�һ���Ӱ���Ϣ
				serialChangeData(2,forSend);								//���ĵڶ����Ӱ���Ϣ
//				serialChangeData(3,forSend);	
				serialSendData();
		
		for(int i=0;i<serialGetNums();i++){
			printf("��%d��Ԫ��:",i);
			if(serialGetRecvType(i)=='S'){
				printf("��������Ϊ:�ַ��� ");
				if(serialGetRecvFlag(i)=='G')
					printf("��������Ϊ:��ȡ ");
				else if(serialGetRecvFlag(i)=='P')
					printf("��������Ϊ:���� ");
				printf("������Ϊ:%s ",serialGetRecvName(i));
				printf("������ϢΪ:%s\r\n",serialGetRecvStr(i));
			}else{
				printf("��������Ϊ:���� ");
				if(serialGetRecvFlag(i)=='G')
					printf("��������Ϊ:��ȡ ");
				else if(serialGetRecvFlag(i)=='P')
					printf("��������Ϊ:���� ");
				printf("������Ϊ:%s ",serialGetRecvName(i));
				printf("������ϢΪ:%d\r\n",serialGetRecvInt(i));				
			}
				
		}
		
		
		//printf("���к�:%d\r\n",serialGetSeq());
		vTaskDelayUntil(&currentTime, MAIN_RTOS_TASK_PERIOD / portTICK_RATE_MS);			//��ʱ
	}
}

