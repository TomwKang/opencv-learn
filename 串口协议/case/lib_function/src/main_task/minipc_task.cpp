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

void miniPC_task(void) { //仅执行一次，无时延要求，在主函数中参与循环


	if (Get_Flag(Camera_error) == 1) {                       //摄像头错误
		Sing_bad_case();
	}
	else if (Get_Flag(PC_ack) == 1 && lastPCState == 0) {    //minipc上线
		lastPCState = 1;
		SetUpperMonitorOnline(1);
		SetFrictionState(FRICTION_WHEEL_ON);
		Set_Flag(Shoot);
		serialDeleteSendDataAll();
		serialAddNewSendData("TNOW", 'P', 0);					//当前位置
		serialAddNewSendData("YNOW", 'P', 0);					//pitch轴角度
		serialAddNewSendData("PNOW", 'P', 0);					//yaw轴角度				
		Sing_miniPC_online();
	}
	else if (lastPCState == 0 && Get_Flag(PC_ack) == 0 && acceptInitPackTimes>1) {    //发起握手
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
if(pData[0] ==  CAMERA_ERROR){ //摄像头错误则蜂鸣器响且下线minipc
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
	if (isLatestPack()){									//如果接收到新的包，将等待接收包的时间置零
		waitingTime = 0;
	}
	if (waitingTime >= maxWaitingTime) {		//如果很久都没有收到包，下线minipc
		SetUpperMonitorOnline(0);
		return;
	}

	waitingTime++;													//没有收到包，但是也还没有到指定时间，等待时间加1
	
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

		//最开始是握手，即发送机器人颜色
		if (shouldSendColor()>=0) {
			if (acceptInitPackTimes >= maxInitSeqNum) {
				//如果一直没有minipc的反馈(若minipc收到机器人颜色，子包中就不会再请求颜色信息)
				//下线minipc
				Set_Flag(Camera_error);
				SetUpperMonitorOnline(0);
				Reset_Flag(Shoot);
			}else
				acceptInitPackTimes++;											//若该值未大于给定的最大值才加1，防止溢出
		}
		else if(acceptInitPackTimes>=1){
			//否则上线minipc
			//在曾经收到包的情况下，同时现在没有收到要求机器人颜色的包，才上线
			Set_Flag(PC_ack);
			renewACK();
			AtSYNing = 0;
			initing=0;
		}
	}
	else if(upperMonitorOnline==1&&waitingTime==1){
		//minipc上线的情况
		if(serialGetNums()==ALL_SUB_PACKS_NUM){
			//先T，再Y，再P
			uint32_t chassisPack=serialGetRecvInt(0);
			uint32_t yawPack=serialGetRecvInt(1);
			uint32_t pitchPack=serialGetRecvInt(2);

			
			//取底盘命令信息
			upperMonitorCmd.Mode=chassisPack&GET_MODE;
			upperMonitorCmd.chassisMovingType=(chassisPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.PositionData=((chassisPack&GET_DATA)>>16)/100;
			
			//取yaw轴命令信息
			upperMonitorCmd.yawMovingType=(yawPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.yawData=(float)((yawPack&GET_DATA)>>16)/100;			
			
			//取pitch轴命令信息
			upperMonitorCmd.pitchMovingType=(pitchPack&GET_MOVING_TYPE)>>8;
			upperMonitorCmd.pitchData=(float)((pitchPack&GET_DATA)>>16)/100;	
			
			//更新包信息，因为是新的包，所以没有被用过，所以used置0
			upperMonitorCmd.used=0;
			
		}
	}else{
		//经过了初始化的状态，且minipc下线了
		//若又收到minipc的包，就上线minipc
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
