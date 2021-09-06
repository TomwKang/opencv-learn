#ifndef _CAN_BUS_TASK_H_
#define _CAN_BUS_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "can1.h"
#define PITCH_SENSOR_BY_ONLY_ONE    0	
	
#define ChassisSensor_ID                        0x402
#define Chassis_ID															0x366	
#define CAN_BUS2_Yaw_FEEDBACK_MSG_ID            0x209
#define CAN_BUS2_Pitch_FEEDBACK_MSG_ID1         0x20A
#define CAN_BUS2_Pitch_FEEDBACK_MSG_ID2      		0x20B
#define CAN_BUS2_GIMBAL_SEND_MSG_ID             0x2FF
	
#define CAN_BUS1_Rammer_FEEDBACK_MSG_ID1        0x205
#define CAN_BUS1_Rammer_FEEDBACK_MSG_ID2        0x206
#define CAN_BUS1_Rammer_SEND_MSG_ID             0x1FF


#define CAN_BUS1_FRICTION_FEEDBACK_MSG_ID1        0x201
#define CAN_BUS1_FRICTION_FEEDBACK_MSG_ID2        0x202
#define CAN_BUS1_FRICTION_FEEDBACK_MSG_ID3        0x203
#define CAN_BUS1_FRICTION_FEEDBACK_MSG_ID4        0x204
#define CAN_BUS1_FRICTION_SEND_MSG_ID             0x200
	
#define RATE_BUF_SIZE 6

#define SEND_TO_CHASSIS_MODE 2

typedef struct{
    int32_t raw_value;
    int32_t last_raw_value;
    int32_t ecd_value;
    int32_t diff;
    int32_t temp_count;
    uint8_t buf_count;
    int32_t ecd_bias;
    int32_t ecd_raw_rate;
    int32_t rate_buf[RATE_BUF_SIZE];
    int32_t round_cnt;
    int32_t filter_rate;
    float ecd_angle;
}Encoder;

typedef struct{
    int16_t speed;
		int8_t  temperature;
}Yaw_t;

typedef struct{
    int16_t speed;
		int8_t  temperature;
		int16_t current;
}Pitch_t;

typedef struct{
    int16_t angle;
    int16_t speed;
    int16_t torque;
}rammer;

typedef struct{
    int16_t angle;
    int16_t speed;
    int16_t torque;
		int8_t  temperature;
}friction_t;


extern volatile friction_t FrictionT1[2];
extern volatile friction_t FrictionT2[2];
extern volatile rammer Rammer[2];
extern volatile Encoder GMYawEncoder;
extern volatile Yaw_t  YawInfo;
extern volatile Encoder GMPitchEncoder[2];
extern volatile	Pitch_t PitchInfo[2];

extern volatile uint8_t My_Robot_Color;



uint16_t Get_Sentry_HeatData(void);
float Get_Sentry_BulletSpeed(void);
uint8_t Get_RunAway_State(void);

void CanReceiveMsgProcess(CanRxMsg *msg);
void EncoderProcess(volatile Encoder *v, CanRxMsg *msg);
void Send_Gimbal_Info(uint8_t Flag_Shoot_State, uint8_t Control_Mode, int16_t ChassisSpeed);
void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq, int16_t gimbal_pitch1_iq,int16_t gimbal_pitch2_iq);
void Set_Rammer_Current(int16_t rammer_current1,int16_t rammer_current2 );
void Set_Friction_Current(int16_t friction_current1,int16_t friction_current2,int16_t friction_current3,int16_t friction_current4);
void sendToClassis_test();
void CanReceiveMsgProcessForCan2(CanRxMsg * msg);


#ifdef __cplusplus
}
#endif
#endif

