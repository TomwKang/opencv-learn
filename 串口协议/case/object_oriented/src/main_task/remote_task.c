#include <stm32f4xx.h>
#include "remote_task.h"
#include "ramp.h"
#include "gun.h"
#include "usart6.h"
#include "control_task.h"
#include "stdio.h"
#include "beep.h"
#include "led.h"


//static ControlMode_e controlmode = FAKE_SHOOT;
static ControlMode_e controlmode = FAKE_SHOOT;
RC_Ctl_t           RC_CtrlData;
Gimbal_Ref_t       GimbalRef;
Gimbal_Target_t    Gimbal_Target;
uint8_t						 Chassis_Target=0;


int16_t ChassisSpeed_Target = 0;

void RemoteDataProcess(uint8_t *pData){
    switch(GetWorkState()){
        case PREPARE_STATE:{
            ;
        }break;
        default:{
					
            if(pData == NULL){
                return;
            }
            RC_CtrlData.rc.ch0  = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
            RC_CtrlData.rc.ch1  = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
            RC_CtrlData.rc.ch2  = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                                  ((int16_t)pData[4] << 10)) & 0x07FF;
            RC_CtrlData.rc.ch3  = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
            RC_CtrlData.rc.s1   = ((pData[5] >> 4) & 0x000C) >> 2;
            RC_CtrlData.rc.s2   = ((pData[5] >> 4) & 0x0003);
            RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
            RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
            RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);
            RC_CtrlData.mouse.press_l = pData[12];
            RC_CtrlData.mouse.press_r = pData[13];
            RC_CtrlData.key.v   = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);
            
            if( RC_CtrlData.rc.s1 == 3 || RC_CtrlData.rc.s1 == 2){
                Reset_Flag(Auto_aim_debug);
            }
            else{
                Set_Flag(Auto_aim_debug);
            }
            
            SetControlMode(&RC_CtrlData.rc);

            if(GetWorkState() == CONTROL_STATE){
            //ChassisSpeed_Target = (RC_CtrlData.rc.ch1 - 1024) * 20;
						ChassisSpeed_Target = (RC_CtrlData.rc.ch1 - 1024);
#if DEBUG_YAW_PID == 0 && DEBUG_PITCH_PID == 0
                Gimbal_Target.pitch_angle_target += (float)(RC_CtrlData.rc.ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET)
                                                    * STICK_TO_PITCH_ANGLE_INC_FACT;
//                Gimbal_Target.pitch_angle_target = ((float)(RC_CtrlData.rc.ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET)/1024u)*55
//                                                    ;								
                Gimbal_Target.yaw_angle_target   -= (float)(RC_CtrlData.rc.ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET)
                                                    * STICK_TO_YAW_ANGLE_INC_FACT;
#endif
                if( RC_CtrlData.rc.s1 == 1 ){
                    Reset_Flag(Shoot);
                    SetFrictionState(FRICTION_WHEEL_OFF);
                    //SetFrictionState(FRICTION_WHEEL_ON);
                }
                else if( RC_CtrlData.rc.s1 == 3 ){
                    SetFrictionState(FRICTION_WHEEL_ON);
                    Reset_Flag(Shoot);
                }
                else{
                    SetFrictionState(FRICTION_WHEEL_ON);
                    Set_Flag(Shoot);
                }
            }
        }
    }
}

void SetControlMode(Remote *rc){
    if(rc->s2 == 1){
        controlmode = REMOTE_CONTROL;
    }
    else if(rc->s2 == 3){
        controlmode = AUTO_CONTROL;
    }
    else if(rc->s2 == 2){
        controlmode = FAKE_SHOOT;
    }
}

ControlMode_e GetControlMode(void){
    return controlmode;
}

int16_t Get_ChassisSpeed_Target(void){
    return ChassisSpeed_Target;
}

void Reset_ChassisSpeed_Target(void){
    ChassisSpeed_Target = 0;
}
