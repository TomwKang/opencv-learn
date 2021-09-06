#ifndef _MINIPC_TASK_H_
#define _MINIPC_TASK_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

#define GET_MODE 0x000000FF
#define GET_MOVING_TYPE 0x0000FF00
#define GET_DATA	0xFFFF0000	
#define ALL_SUB_PACKS_NUM 3	
	
typedef struct {
    uint8_t start1;
    uint8_t start2;
    uint8_t cmdid;
    uint8_t data[4]; 
    uint8_t end1;
    uint8_t end2;
}package_t;
typedef enum {
    GIMBAL_CMD_STOP,
    GIMBAL_CMD_MOVEBY,
    GIMBAL_CMD_MOVETO
}GimbalMovingCtrType_e;

typedef struct{
		uint8_t Mode;
		uint8_t used;
    uint8_t startFriction;
//    GimbalMovingCtrType_e gimbalMovingCtrType;
		uint8_t chassisMovingType;
		uint8_t pitchMovingType;											//pitch轴的运动状态，0-->moveTo, 1-->moveBy
		uint8_t yawMovingType;												//yaw轴的运动状态,0-->moveTo, 1-->moveBy
		uint8_t shootSpeed;													//射速状态,0-->low speed,  1-->middle speed,  2-->high speed   
		uint8_t PositionData;											//给定的底盘运动的目标位置
		float pitchData;
    float yawData;
}UpperMonitor_Ctr_t;

UpperMonitor_Ctr_t *GetUpperMonitorCmd(void);

uint8_t GetUpperMonitorOnline(void);
void SetUpperMonitorOnline(uint8_t isOnline);
void UpperMonitorDataProcess();

void RequestFinishFrictionSpeedUp(void);
void miniPC_task(void);
void SendGimbalToMiniPC(int chassisPos,int yawAngle,int pitchAngle);
uint8_t getAtSYNing();
#ifdef __cplusplus
}
#endif

#endif
