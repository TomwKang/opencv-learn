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

#define  maxWaitingTime 1500
#define  maxInitSeqNum  3000
volatile uint8_t upperMonitorOnline = 0;
volatile static  UpperMonitor_Ctr_t upperMonitorCmd = { 0,0,0,0,0,0,0,0,0 };
volatile static  package_t package = { 0x0a,0x0d,0xff,0x00,0x00,0x00,0x00,0x0d,0x0a };
static int lastPCState = 0;
uint16_t waitingTime = 0;
uint8_t  AtSYNing = 1;
uint8_t  initing=1;
uint16_t  acceptInitPackTimes=0;


void miniPC_ACK_status(void) {
/*
	int16_t yaw = (int16_t)GET_YAW_ANGLE * 5;
	int16_t pit = (int16_t)(GET_PITCH1_ANGLE * 6);
	package.cmdid = 0x11;
	package.data[0] = ((uint8_t *)&yaw)[0];
	package.data[1] = ((uint8_t *)&yaw)[1];
	package.data[2] = ((uint8_t *)&pit)[0];
	package.data[3] = ((uint8_t *)&pit)[1];
	USART6_Print((uint8_t*)&package, 9);
*/
}
void miniPC_HS_SYN(void) {
	serialChangeData(0, 0);
	serialSendData();
}

void miniPC_task(void) { //��ִ��һ�Σ���ʱ��Ҫ�����������в���ѭ��


	if (Get_Flag(Camera_error) == 1) {                       //����ͷ����
		Sing_bad_case();
	}
	else if (Get_Flag(PC_ack) == 1 && lastPCState == 0) {    //minipc����
		lastPCState = 1;
		SetUpperMonitorOnline(1);
		SetFrictionState(FRICTION_WHEEL_ON);
		Set_Flag(Shoot);
		serialDeleteSendDataAll();
		serialAddNewSendData("TNOW", 'P', 0);					//��ǰλ��
		serialAddNewSendData("YNOW", 'P', 0);					//pitch��Ƕ�
		serialAddNewSendData("PNOW", 'P', 0);					//yaw��Ƕ�				
		Sing_miniPC_online();
	}
	else if (lastPCState == 0 && Get_Flag(PC_ack) == 0 && acceptInitPackTimes>1) {    //��������
		miniPC_HS_SYN();
	}
	else {
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
UpperMonitor_Ctr_t *GetUpperMonitorCmd(void) {
	return (UpperMonitor_Ctr_t *)&upperMonitorCmd;
}

void SetUpperMonitorOnline(uint8_t isOnline) {
	upperMonitorOnline = isOnline;
}

uint8_t GetUpperMonitorOnline(void) {
	return upperMonitorOnline;
}
uint8_t getAtSYNing() {
	return AtSYNing;
}
int8_t shouldSendColor() {
	for (int i = 0; i<serialGetNums(); i++)
		if (strcmp(serialGetRecvName(i), "COLO") == 0)
			return i;

	return -1;
}

void UpperMonitorDataProcess() {
	if (isLatestPack()){									//������յ��µİ������ȴ����հ���ʱ������
		waitingTime = 0;
	}
	if (waitingTime >= maxWaitingTime) {		//����ܾö�û���յ���������minipc
		SetUpperMonitorOnline(0);
		return;
	}

	waitingTime++;													//û���յ���������Ҳ��û�е�ָ��ʱ�䣬�ȴ�ʱ���1
	
	if(!initing&&shouldSendColor()>=0){
		initing=1;
		acceptInitPackTimes=0;
		SetUpperMonitorOnline(0);
		Reset_Flag(Shoot);
		Reset_Flag(PC_ack);
		lastPCState = 0;
		serialDeleteSendDataAll();
		serialAddNewSendData("COLO", 'P','S', (void *)"R");	
	}
	if (initing) {

		//�ʼ�����֣������ͻ�������ɫ
		if (shouldSendColor()>=0) {
			if (acceptInitPackTimes >= maxInitSeqNum) {
				//���һֱû��minipc�ķ���(��minipc�յ���������ɫ���Ӱ��оͲ�����������ɫ��Ϣ)
				//����minipc
				Set_Flag(Camera_error);
				SetUpperMonitorOnline(0);
				Reset_Flag(Shoot);
			}else
				acceptInitPackTimes++;											//����ֵδ���ڸ��������ֵ�ż�1����ֹ���
		}
		else if(acceptInitPackTimes>=1){
			//��������minipc
			//�������յ���������£�ͬʱ����û���յ�Ҫ���������ɫ�İ���������
			Set_Flag(PC_ack);
			renewACK();
			AtSYNing = 0;
			initing=0;
		}
	}
	else if(upperMonitorOnline==1&&waitingTime==1){
		//minipc���ߵ����
		if(serialGetNums()==ALL_SUB_PACKS_NUM){
			//��T����Y����P
			uint32_t chassisPack=serialGetRecvInt(0);
			uint32_t yawPack=serialGetRecvInt(1);
			uint32_t pitchPack=serialGetRecvInt(2);

			
			//ȡ����������Ϣ
			upperMonitorCmd.Mode=chassisPack&GET_MODE;
			upperMonitorCmd.chassisMovingType=(chassisPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.PositionData=((chassisPack&GET_DATA)>>16)/100;
			
			//ȡyaw��������Ϣ
			upperMonitorCmd.yawMovingType=(yawPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.yawData=(float)((yawPack&GET_DATA)>>16)/100;			
			
			//ȡpitch��������Ϣ
			upperMonitorCmd.pitchMovingType=(pitchPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.pitchData=(float)((pitchPack&GET_DATA)>>16)/100;	
			
			//���°���Ϣ����Ϊ���µİ�������û�б��ù�������used��0
			upperMonitorCmd.used=0;
			
		}
	}else{
		//�����˳�ʼ����״̬����minipc������
		//�����յ�minipc�İ���������minipc
		if(waitingTime==1)
			SetUpperMonitorOnline(1);
	}


}

void SendGimbalToMiniPC(int chassisPos,int yawAngle,int pitchAngle){
		if(!getAtSYNing()){
				serialChangeData(0,chassisPos);
				serialChangeData(1,yawAngle);
				serialChangeData(2,pitchAngle);	
				serialSendData();
		}	
}
