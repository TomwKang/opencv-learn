#include "minipc_task.h"
#include "sys.h"
#include "usart6.h"
#include "remote_task.h"
#include "ramp.h"
#include "gun.h"
#include "control_task.h"
#include "can_bus_task.h"
#include "stdio.h"
#include "beep.h"
#include "communication.h"

#define  maxWaitingTime 3000
#define  maxInitSeqNum  200
volatile uint8_t upperMonitorOnline = 0;
volatile static  UpperMonitor_Ctr_t upperMonitorCmd = {0,GIMBAL_CMD_STOP,0,0};
volatile static  package_t package = { 0x0a,0x0d,0xff,0x00,0x00,0x00,0x00,0x0d,0x0a };
uint16_t waitingTime=0;
uint8_t  AtSYNing=1;


void miniPC_ACK_status(void){
    int16_t yaw = (int16_t)GET_YAW_ANGLE * 5;
    int16_t pit = (int16_t)(GET_PITCH1_ANGLE * 6);
    package.cmdid = 0x11;
    package.data[0] = ((uint8_t *)&yaw)[0]; 
    package.data[1] = ((uint8_t *)&yaw)[1];
    package.data[2] = ((uint8_t *)&pit)[0]; 
    package.data[3] = ((uint8_t *)&pit)[1];     
    USART6_Print((uint8_t*)&package, 9);
}
void miniPC_HS_SYN(void) {
		serialChangeData(0,0);
		serialSendData();
}

void miniPC_task(void){ //��ִ��һ�Σ���ʱ��Ҫ�����������в���ѭ��
    static int lastPCState = 0;

    if(Get_Flag(Camera_error) == 1){                       //����ͷ����
        Sing_bad_case();
    }
    else if(Get_Flag(PC_ack) == 1 && lastPCState == 0){    //minipc����
        lastPCState = 1;
			  SetUpperMonitorOnline(1);
        SetFrictionState(FRICTION_WHEEL_ON);
        Set_Flag(Shoot);
				serialDeleteSendDataAll();
				serialAddNewSendData("P1", 'P', 0);					//��ǰλ��
				serialAddNewSendData("P2", 'P', 0);					//pitch��Ƕ�
				serialAddNewSendData("P3", 'P', 0);					//yaw��Ƕ�
				serialAddNewSendData("P4", 'P', 0);					//����				
        Sing_miniPC_online();
    }
    else if(lastPCState == 0 && Get_Flag(PC_ack) == 0){    //��������
        miniPC_HS_SYN();
    }
    else{
        ;
    }
}

/*
void UpperMonitorDataProcess(volatile uint8_t *pData){
    static const uint8_t START_UPPER_MONITOR_CTR = 0x00;
    static const uint8_t SEND_STATUS             = 0x01;
    static const uint8_t GIMBAL_MOVEBY           = 0x02;
    static const uint8_t GIMBAL_MOVETO           = 0x03;
    static const uint8_t ACK                     = 0x10;
    static const uint8_t EXIT_UPPER_MONITOR_CTR  = 0x12;
    static const uint8_t CAMERA_ERROR            = 0xEE;
    
    int16_t d1 = *((int16_t *)(pData + 1));
    int16_t d2 = *((int16_t *)(pData + 3));

    if(pData[0] ==  SEND_STATUS){
        miniPC_ACK_status();
        minipc_alive_count = 0;
    }
    if(pData[0] ==  CAMERA_ERROR){ //����ͷ�������������������minipc
        Set_Flag(Camera_error);
        SetUpperMonitorOnline(0);
        Reset_Flag(Shoot);
    }
    if(upperMonitorOnline){
        switch (pData[0]){
            case GIMBAL_MOVEBY:{
                upperMonitorCmd.d1 = d1 * 0.001f;
                upperMonitorCmd.d2 = d2 * 0.001f;
                upperMonitorCmd.gimbalMovingCtrType = GIMBAL_CMD_MOVEBY;
            }break;
            
            case GIMBAL_MOVETO:{
                upperMonitorCmd.d1 = d1 / 5.0f;
                upperMonitorCmd.d2 = d2 / 6.0f;
                upperMonitorCmd.gimbalMovingCtrType = GIMBAL_CMD_MOVETO;
            }break;
            
            case EXIT_UPPER_MONITOR_CTR:{
                SetUpperMonitorOnline(0);
                Reset_Flag(Shoot);
            }break;

            default:{
                ;
            }break;
        }
    }
    else if(pData[0] == START_UPPER_MONITOR_CTR){
        SetUpperMonitorOnline(1);
        SetFrictionState(FRICTION_WHEEL_ON);
        Set_Flag(Shoot);
    }
    else if(pData[0] ==  ACK){
        Set_Flag(PC_ack);
    }
    else{
        ;
    }
}
*/
UpperMonitor_Ctr_t GetUpperMonitorCmd(void){
    return upperMonitorCmd;
}

void SetUpperMonitorOnline(uint8_t isOnline){
    upperMonitorOnline = isOnline;
}

uint8_t GetUpperMonitorOnline(void){
    return upperMonitorOnline;
}
uint8_t getAtSYNing(){
	return AtSYNing;
}
uint8_t shouldSendColor(){
	for(int i=0;i<serialGetNums();i++)
		if(strcmp(serialGetRecvName(i),"col")==0)
			return i;
		
	return -1;
}

void UpperMonitorDataProcess(){
	if(isLatestPack())
		waitingTime=0;

	if(waitingTime>=maxWaitingTime){
		SetUpperMonitorOnline(0);
		return;
	}
	
	waitingTime++;
	
	if(upperMonitorOnline){
		if(shouldSendColor()){
			if(serialGetSeq()>=maxInitSeqNum){
				Set_Flag(Camera_error);
        SetUpperMonitorOnline(0);
        Reset_Flag(Shoot);
			}
		}else{
			Set_Flag(PC_ack);
			AtSYNing=0;
		}
	}else{
		
		
		
	}
	
	
}
