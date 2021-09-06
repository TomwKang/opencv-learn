#ifndef _CONTROL_TASK_H_
#define _CONTROL_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "pid_regulator.h"
#include "can_bus_task.h"
//3508最大电流正负16384，2006最大电流正负10000，6020最大电流正负30000


	
#define YAW_INIT_ANGLE              0.0f
#define PITCH_INIT_ANGLE            -20.0f
#define PITCH_MAX                  	55.5f		//55.5
#define PITCH_MIN                   0.0f	
#define FRICTION_RAMP_TICK_COUNT    200
#define STABLE_PITCH_MOVE           40
#define QUICK_PITCH_MOVE            1
#define SLOW_PITCH_MOVE            0.5
	
#define CHASSIS_MAX 245
#define CHASSIS_MIN 15	



#define PREPARE_TIME_TICK_MS      1000
#define YAW_POSITION_KP_DEFAULTS  40
#define YAW_POSITION_KI_DEFAULTS  0.15
#define YAW_POSITION_KD_DEFAULTS  6000

#define YAW_SPEED_KP_DEFAULTS  2
#define YAW_SPEED_KI_DEFAULTS  0.15
#define YAW_SPEED_KD_DEFAULTS  1
/*
#define PITCH_POSITION_KP_DEFAULTS  27
#define PITCH_POSITION_KI_DEFAULTS  0.5
#define PITCH_POSITION_KD_DEFAULTS  0

#define PITCH_SPEED_KP_DEFAULTS  70
#define PITCH_SPEED_KI_DEFAULTS  0.3
#define PITCH_SPEED_KD_DEFAULTS  0
*/

#define PITCH_POSITION_KP_DEFAULTS  150
#define PITCH_POSITION_KI_DEFAULTS  0.15
#define PITCH_POSITION_KD_DEFAULTS 	300
//#define PITCH_POSITION_KP_DEFAULTS  1000
//#define PITCH_POSITION_KI_DEFAULTS  0.00
//#define PITCH_POSITION_KD_DEFAULTS  2500

#define PITCH_SPEED_KP_DEFAULTS  20
#define PITCH_SPEED_KI_DEFAULTS  0.14
#define PITCH_SPEED_KD_DEFAULTS  25


#define RAMMER_SPEED_KP_DEFAULTS  18
#define RAMMER_SPEED_KI_DEFAULTS  0.1
#define RAMMER_SPEED_KD_DEFAULTS  0 


#define FRICTION_SPEED_KP_DEFAULTS  18
#define FRICTION_SPEED_KI_DEFAULTS  0.3
#define FRICTION_SPEED_KD_DEFAULTS  0.1 

#define GIMBAL_YAW_CRUISE_DELTA   3.5f
#define GIMBAL_PITCH_CRUISE_DELTA 0.8f

//#define GET_YAW_ANGLE           get_yaw_angle()
//#define GET_YAW_ANGLE           imu_yaw_angle
#define GET_YAW_ANGLE            GMYawEncoder.ecd_angle
#define GET_YAW_ANGULAR_SPEED     imu_yaw_angular_speed

#define GET_PITCH1_ANGLE           GMPitchEncoder[0].ecd_angle
#define GET_PITCH2_ANGLE           GMPitchEncoder[1].ecd_angle


#define GET_PITCH_ANGULAR_SPEED   -imu_pitch_angular_speed

typedef enum{
    PREPARE_STATE,
    CRUISE_STATE,
    SHOOT_STATE,
    CONTROL_STATE,
}WorkState_e;

typedef enum{
    FRICTION_WHEEL_OFF = 0,
    FRICTION_WHEEL_ON = 1,
}FrictionWheelState_e;

/*
#define GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    PITCH_POSITION_KP_DEFAULTS,\
    PITCH_POSITION_KI_DEFAULTS,\
    PITCH_POSITION_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    30000,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}
*/
#define GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    PITCH_POSITION_KP_DEFAULTS,\
    PITCH_POSITION_KI_DEFAULTS,\
    PITCH_POSITION_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    30000,\
    0,\
    0,\
    0,\
		0,\
		0.75,\
    &PID_Calc_IncomD,\
    &PID_Reset_IncomD,\
}
//0.95


#define GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    PITCH_SPEED_KP_DEFAULTS,\
    PITCH_SPEED_KI_DEFAULTS,\
    PITCH_SPEED_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    30000,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

#define GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    YAW_POSITION_KP_DEFAULTS,\
    YAW_POSITION_KI_DEFAULTS,\
    YAW_POSITION_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    5000,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

#define GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    YAW_SPEED_KP_DEFAULTS,\
    YAW_SPEED_KI_DEFAULTS,\
    YAW_SPEED_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    4900,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

#define RAMMER_SPEED_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    RAMMER_SPEED_KP_DEFAULTS,\
    RAMMER_SPEED_KI_DEFAULTS,\
    RAMMER_SPEED_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    10000,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

#define FRICTION_SPEED_PID_DEFAULT \
{\
    0,\
    0,\
    {0,0},\
    FRICTION_SPEED_KP_DEFAULTS,\
    FRICTION_SPEED_KI_DEFAULTS,\
    FRICTION_SPEED_KD_DEFAULTS,\
    0,\
    0,\
    0,\
    4900,\
    1000,\
    1500,\
    0,\
    16000,\
    0,\
    0,\
    0,\
    &PID_Calc,\
    &PID_Reset,\
}

#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
    val = min;\
}\
else if(val>=max)\
{\
    val = max;\
}\
else\
{\
    val = val;\
}

//extern PID_Regulator_t GMPPositionPID[2] ;
extern PID_Regulator_IncomD_t GMPPositionPID[2];
extern PID_Regulator_t GMPSpeedPID[2]    ;
extern PID_Regulator_t GMYPositionPID ;
extern PID_Regulator_t GMYSpeedPID    ;
extern PID_Regulator_t RAMMERSpeedPID[2] ;
extern PID_Regulator_t FRICTIONSpeedPID1[2] ;
extern PID_Regulator_t FRICTIONSpeedPID2[2] ;

extern volatile int16_t minipc_alive_count;

void Control_Task(void);
void ControtTaskInit(void);
FrictionWheelState_e GetFrictionState(void);
WorkState_e GetWorkState(void);
void SetFrictionState(FrictionWheelState_e v);
void RammerSpeedPID(int num,int16_t TargetSpeed);
void FrictionSpeedPID1(int num,int16_t TargetSpeed);
void FrictionSpeedPID2(int num,int16_t TargetSpeed);

void Set_FRICTION_WHEEL_MAX_DUTY(uint32_t x);

void Set_Chassis_Now_Pos(uint8_t v);
uint8_t Get_Chassis_Now_Pos();
#ifdef __cplusplus
		}
#endif
#endif
