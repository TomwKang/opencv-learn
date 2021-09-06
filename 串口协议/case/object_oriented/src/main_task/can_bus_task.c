#include "can_bus_task.h"
#include "stdio.h"
#include "can1.h"
#include "main.h"
#define needDelay 10000
volatile Encoder GMYawEncoder   = {0,0,0,0,0,0,Yaw_Encoder_Offset,0,0};
volatile Yaw_t YawInfo={0,0};
volatile Encoder GMPitchEncoder[2] = {{0,0,0,0,0,0,Pit1_Encoder_Offset,0,0},{0,0,0,0,0,0,Pit1_Encoder_Offset,0,0}};
volatile Pitch_t PitchInfo[2]={{0,0},{0,0}};

volatile rammer Rammer[2] ={{ 0, 0, 0 },{0,0,0}};
volatile friction_t FrictionT1[2]={{ 0, 0, 0 ,0},{ 0, 0, 0 ,0}};
volatile friction_t FrictionT2[2]={{ 0, 0, 0 ,0},{ 0, 0, 0 ,0}};

volatile uint16_t Sentry_HeatData    = 0;
volatile float Sentry_BulletSpeed = 0;
volatile uint8_t Flag_In_RunAwayState = 0;
volatile uint8_t My_Robot_Color = 0;


uint8_t PresetPosition[5]={255*0.5,255*0.75,255*0.25,255*0.95,18};
uint8_t pointToNextPos=0;
uint8_t sendPos=0;
int8_t couldReturnToClassis=0;

void EncoderProcess(volatile Encoder *v, CanRxMsg *msg){
    int i = 0;
    int32_t temp_sum  = 0;    
    v->last_raw_value = v->raw_value;
    v->raw_value = (msg->Data[0]<<8)|msg->Data[1];
    v->diff = v->raw_value - v->last_raw_value;
    if(v->diff < -7500){
        v->round_cnt++;
        v->ecd_raw_rate = v->diff + 8192;
    }
    else if(v->diff > 7500){
        v->round_cnt--;
        v->ecd_raw_rate = v->diff - 8192;
    }
    else{
        v->ecd_raw_rate = v->diff;
    }
    v->ecd_value = v->raw_value + v->round_cnt * 8192;
    v->ecd_angle = (float)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;
		/*
    v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
    if(v->buf_count == RATE_BUF_SIZE){
        v->buf_count = 0;
    }
    for(i = 0; i < RATE_BUF_SIZE; i++){
        temp_sum += v->rate_buf[i];
    }
    v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);
		*/
}

void CanReceiveMsgProcess(CanRxMsg * msg){
    volatile static uint32_t bullet_speed[1] = {0};
    
    switch(msg->StdId){
				
        case CAN_BUS1_Rammer_FEEDBACK_MSG_ID1:{
            Rammer[0].angle  = msg->Data[0] << 8 | msg->Data[1];
            Rammer[0].speed  = msg->Data[2] << 8 | msg->Data[3];
            Rammer[0].torque = msg->Data[4] << 8 | msg->Data[5];
        }break;
				case CAN_BUS1_Rammer_FEEDBACK_MSG_ID2:{
            Rammer[1].angle  = msg->Data[0] << 8 | msg->Data[1];
            Rammer[1].speed  = msg->Data[2] << 8 | msg->Data[3];
            Rammer[1].torque = msg->Data[4] << 8 | msg->Data[5];
        }break;
				
				//CAN_BUS1_FRICTION_FEEDBACK_MSG_ID1
				case CAN_BUS1_FRICTION_FEEDBACK_MSG_ID1:{
            FrictionT1[0].angle  = msg->Data[0] << 8 | msg->Data[1];
            FrictionT1[0].speed  = msg->Data[2] << 8 | msg->Data[3];
            FrictionT1[0].torque = msg->Data[4] << 8 | msg->Data[5];
						FrictionT1[0].temperature = msg->Data[6];
        }break;
				case CAN_BUS1_FRICTION_FEEDBACK_MSG_ID2:{
            FrictionT1[1].angle  = msg->Data[0] << 8 | msg->Data[1];
            FrictionT1[1].speed  = msg->Data[2] << 8 | msg->Data[3];
            FrictionT1[1].torque = msg->Data[4] << 8 | msg->Data[5];
        }break;
				case CAN_BUS1_FRICTION_FEEDBACK_MSG_ID3:{
            FrictionT2[0].angle  = msg->Data[0] << 8 | msg->Data[1];
            FrictionT2[0].speed  = msg->Data[2] << 8 | msg->Data[3];
            FrictionT2[0].torque = msg->Data[4] << 8 | msg->Data[5];
						FrictionT2[0].temperature = msg->Data[6];
        }break;
				case CAN_BUS1_FRICTION_FEEDBACK_MSG_ID4:{
            FrictionT2[1].angle  = msg->Data[0] << 8 | msg->Data[1];
            FrictionT2[1].speed  = msg->Data[2] << 8 | msg->Data[3];
            FrictionT2[1].torque = msg->Data[4] << 8 | msg->Data[5];
        }break;				
				

        default:{
            
        }break;
    }

#if Monitor_GM_Encoder==1
    printf("yaw_raw_value:%6.6d  pit_raw_value:%6.6d  yaw_ecd:%6.6f  pit_ecd:%6.6f\r\n",
            GMYawEncoder.raw_value, GMPitchEncoder.raw_value,GMYawEncoder.ecd_angle, GMPitchEncoder.ecd_angle);
#endif

#if Monitor_rammer==1
    printf("angle:%6d  speed:%6d  torque:%6d\r\n",
            Rammer.angle, Rammer.speed, Rammer.torque);
#endif
}

void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq, int16_t gimbal_pitch1_iq,int16_t gimbal_pitch2_iq){
    CanTxMsg tx_message;
    tx_message.StdId = CAN_BUS2_GIMBAL_SEND_MSG_ID;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = (unsigned char)(gimbal_yaw_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_yaw_iq;
    tx_message.Data[2] = (unsigned char)(gimbal_pitch1_iq >> 8);
    tx_message.Data[3] = (unsigned char)gimbal_pitch1_iq;
    tx_message.Data[4] = (unsigned char)(gimbal_pitch2_iq >> 8);
    tx_message.Data[5] = (unsigned char)gimbal_pitch2_iq;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CANx,&tx_message);
}

void Set_Rammer_Current(int16_t rammer_current1,int16_t rammer_current2 ){
    CanTxMsg tx_message;
    tx_message.StdId = CAN_BUS1_Rammer_SEND_MSG_ID ;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = (uint8_t)(rammer_current1 >> 8);
    tx_message.Data[1] = (uint8_t)rammer_current1;
    tx_message.Data[2] = (uint8_t)(rammer_current2 >> 8);;
    tx_message.Data[3] = (uint8_t)rammer_current2;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CAN1,&tx_message);

}

void Set_Friction_Current(int16_t friction_current1,int16_t friction_current2,int16_t friction_current3,int16_t friction_current4){
    CanTxMsg tx_message;
    tx_message.StdId = CAN_BUS1_FRICTION_SEND_MSG_ID ;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = (uint8_t)(friction_current1 >> 8);
    tx_message.Data[1] = (uint8_t)friction_current1;
    tx_message.Data[2] = (uint8_t)(friction_current2 >> 8);
    tx_message.Data[3] = (uint8_t)friction_current2;
    tx_message.Data[4] = (uint8_t)(friction_current3 >> 8);
    tx_message.Data[5] = (uint8_t)friction_current3;
    tx_message.Data[6] = (uint8_t)(friction_current4 >> 8);
    tx_message.Data[7] = (uint8_t)friction_current4;
    CAN_Transmit(CAN1,&tx_message);
}

void Send_Gimbal_Info(uint8_t Flag_Shoot_State, uint8_t Control_Mode, int16_t ChassisSpeed){
    CanTxMsg tx_message;
    tx_message.StdId = 0x401;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = (uint8_t)Flag_Shoot_State;
    tx_message.Data[1] = (uint8_t)Control_Mode;
    tx_message.Data[2] = (uint8_t)(ChassisSpeed >> 8);
    tx_message.Data[3] = (uint8_t)ChassisSpeed;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CAN1,&tx_message);
}

void sendToClassis_test(){
#if SEND_TO_CHASSIS_MODE==3																				//测试模式
		if(getTimeForDelay()>=needDelay){
						sendPos=PresetPosition[(pointToNextPos++)%5];
					//	couldReturnToClassis=0;		
			resetTimeForDelay();	
		//	couldReturnToClassis=0;					
			}
			

			CanTxMsg TxMessage;
			TxMessage.StdId = 0x392;
      TxMessage.RTR = CAN_RTR_DATA;
      TxMessage.IDE = CAN_Id_Standard;
      TxMessage.DLC = 0x01;
			TxMessage.Data[0]=sendPos;
			CAN_Transmit(CAN2, &TxMessage);	
#elif SEND_TO_CHASSIS_MODE==2																			//遥控器控制模式
			CanTxMsg TxMessage;
			TxMessage.StdId = 0x392;
      TxMessage.RTR = CAN_RTR_DATA;
      TxMessage.IDE = CAN_Id_Standard;
      TxMessage.DLC = 0x02;
			TxMessage.Data[0] = (uint8_t)(Get_ChassisSpeed_Target() >> 8);
			TxMessage.Data[1] = (uint8_t)Get_ChassisSpeed_Target();
			CAN_Transmit(CAN2, &TxMessage);		



#elif SEND_TO_CHASSIS_MODE==1																			//minipc控制模式			
			CanTxMsg TxMessage;
			TxMessage.StdId = 0x392;
      TxMessage.RTR = CAN_RTR_DATA;
      TxMessage.IDE = CAN_Id_Standard;
      TxMessage.DLC = 0x08;
			TxMessage.Data[0] = (uint8_t)(Chassis_Target >> 8);
			CAN_Transmit(CAN2, &TxMessage);		
#endif

}


float Get_Sentry_BulletSpeed(void){
    return Sentry_BulletSpeed / 100.0f;
}

uint16_t Get_Sentry_HeatData(void){
    return Sentry_HeatData;
}

uint8_t Get_RunAway_State(void){
    return Flag_In_RunAwayState;
}

void EncoderProcessForYaw(volatile Encoder *v, CanRxMsg *msg){
    v->ecd_value = (msg->Data[0] << 8) | msg->Data[1];
    
    v->ecd_angle=(float)(v->ecd_value - v->ecd_bias) * 360 / 8192;
		
		if(v->ecd_angle > 180)
			v->ecd_angle -= 360;
		else if(v->ecd_angle < -180)
			v->ecd_angle += 360;
}


void CanReceiveMsgProcessForCan2(CanRxMsg * msg){

    
    switch(msg->StdId){
        case CAN_BUS2_Yaw_FEEDBACK_MSG_ID:{
            EncoderProcess(&GMYawEncoder, msg);
						YawInfo.speed=msg->Data[2] << 8 | msg->Data[3];
						YawInfo.temperature=msg->Data[6];
        }break;				
        case CAN_BUS2_Pitch_FEEDBACK_MSG_ID1:{
            EncoderProcess(&GMPitchEncoder[0], msg);
            //由于pitch电机反馈的角度和遥控器给定的角度变化方向相反，故取反
            GMPitchEncoder[0].ecd_angle = -GMPitchEncoder[0].ecd_angle; 	
						PitchInfo[0].speed=msg->Data[2] << 8 | msg->Data[3];
						PitchInfo[0].temperature=msg->Data[6];
						PitchInfo[0].current=msg->Data[4] << 8 | msg->Data[5];
        }break;
        case CAN_BUS2_Pitch_FEEDBACK_MSG_ID2:{
            EncoderProcess(&GMPitchEncoder[1], msg);
            //由于pitch电机反馈的角度和遥控器给定的角度变化方向相反，故取反
            GMPitchEncoder[1].ecd_angle = GMPitchEncoder[1].ecd_angle; 
						PitchInfo[1].speed=msg->Data[2] << 8 | msg->Data[3];
						PitchInfo[1].temperature=msg->Data[6];
        }break;		
				case Chassis_ID:{
					Set_Chassis_Now_Pos(msg->Data[0]);																//获取底盘当前位置
				}
        default:{
            
        }break;
    }
}


