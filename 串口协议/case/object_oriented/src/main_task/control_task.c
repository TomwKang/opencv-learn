#include "control_task.h"
#include "pid_regulator.h"
#include "ramp.h"
#include "remote_task.h"
#include "flags.h"
#include "can_bus_task.h"
#include "imu.h"
#include "usart3.h"
#include "remote_task.h"
#include "main.h"
#include "gun.h"
#include "minipc_task.h"
#include "kalman_filter.h"
#include "auto_aim.h"


//PID_Regulator_t GMPPositionPID[2] = {GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT,GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT};				//pitch������6020
PID_Regulator_IncomD_t GMPPositionPID[2] = {GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT,GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT};				//pitch������6020
PID_Regulator_t GMPSpeedPID[2] = {GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT,GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT};									//pitch������6020
PID_Regulator_t GMYPositionPID = GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT;
PID_Regulator_t GMYSpeedPID = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;
PID_Regulator_t RAMMERSpeedPID[2] = { RAMMER_SPEED_PID_DEFAULT,RAMMER_SPEED_PID_DEFAULT };
PID_Regulator_t FRICTIONSpeedPID1[2] = { FRICTION_SPEED_PID_DEFAULT,FRICTION_SPEED_PID_DEFAULT };
PID_Regulator_t FRICTIONSpeedPID2[2] = { FRICTION_SPEED_PID_DEFAULT,FRICTION_SPEED_PID_DEFAULT };

RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
RampGen_t GMYawRamp = RAMP_GEN_DAFAULT;
RampGen_t FrictionRamp1 = RAMP_GEN_DAFAULT;
RampGen_t FrictionRamp2 = RAMP_GEN_DAFAULT;

WorkState_e lastWorkState = PREPARE_STATE;
WorkState_e workState = PREPARE_STATE;
FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;

uint8_t				 		 Chassis_Now_Pos=0;																			//���̵�ǰλ��


volatile static int16_t  FRICTION_WHEEL_MAX_DUTY1_1 = -1320-2000;
#define  FRICTION_WHEEL_MAX_DUTY1_2  -FRICTION_WHEEL_MAX_DUTY1_1
#define  FRICTION_WHEEL_MAX_DUTY2_1 FRICTION_WHEEL_MAX_DUTY1_1-300
#define  FRICTION_WHEEL_MAX_DUTY2_2 -FRICTION_WHEEL_MAX_DUTY2_1
volatile static float first_pit_angle[2] = {0,0};
volatile static uint8_t pit_angle_limit_flag = 0;
volatile int16_t minipc_alive_count = 0;
static uint8_t  yaw_clockwise_flag = 0;
int16_t AimTic = 0; //for auto aim forcasting 
uint8_t FriOk[2]={0,0};

void GimbalPitchAngleLimit(void) {
	if (Gimbal_Target.pitch_angle_target <= PITCH_MIN) {
		Gimbal_Target.pitch_angle_target = PITCH_MIN;
		pit_angle_limit_flag = 1;
	}
	else if (Gimbal_Target.pitch_angle_target >= PITCH_MAX) {
		Gimbal_Target.pitch_angle_target = PITCH_MAX;
		pit_angle_limit_flag = 1;
	}
	else {
		pit_angle_limit_flag = 0;
	}
}


void GimbalYawAngleLimit(){

	
}

/**
  * @author         ���˻�
  * @brief          ��̨״̬�л�
  * @data           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void WorkStateFSM(void) {
	static uint16_t time_tick_1ms = 0;
	lastWorkState = workState;

	switch (workState) {
	case PREPARE_STATE: {
		if ((time_tick_1ms++) >= PREPARE_TIME_TICK_MS) {
			workState = CRUISE_STATE;
		}
	}break;
	case CRUISE_STATE: {
		if (GetUpperMonitorOnline() == 1) {
			workState = SHOOT_STATE;
		}
		if (GetControlMode() == REMOTE_CONTROL) {
			workState = CONTROL_STATE;
		}
	}break;
	case SHOOT_STATE: {
		if (GetUpperMonitorOnline() == 0) {
			workState = CRUISE_STATE;
		}
	}break;
	case CONTROL_STATE: {
		if (GetControlMode() != REMOTE_CONTROL) {
			workState = CRUISE_STATE;
			Reset_ChassisSpeed_Target();
		}
		if (GetUpperMonitorOnline() == 1) {
			workState = SHOOT_STATE;
			Reset_ChassisSpeed_Target();
		}
	}break;
	}
	/* �Ҳ��˲������¡�ģ��minipc���ߣ����ڵ��̵��� */
	if (time_tick_1ms >= PREPARE_TIME_TICK_MS && GetControlMode() == FAKE_SHOOT) {
		workState = SHOOT_STATE;
	}
}

void SetWorkState(WorkState_e state) {
	workState = state;
}

WorkState_e GetWorkState(void) {
	return workState;
}
uint8_t Is_Shoot_State(void) {
	if (GetWorkState() == SHOOT_STATE) {
		return 1;
	}
	else {
		return 0;
	}
}
uint8_t Is_Control_State(void) {
	if (GetWorkState() == CONTROL_STATE) {
		return 1;
	}
	else {
		return 0;
	}
}
/**
  * @brief          minipc���ƻ�·
  * @author         ���˻�
  * @data           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void UpperMonitorControlLoop(void) {
	static float *angle_data;
	static GMAngle_t angle;
	static float last_yaw = 0;
	static float last_pit = 0;
	UpperMonitor_Ctr_t *cmd = GetUpperMonitorCmd();
	if(cmd->used==1)
		//������ù��Ļ���ֱ�ӷ���
		return;
	//�ȵ���
	if(cmd->chassisMovingType==0x00)
		Chassis_Target+=cmd->PositionData;
	else
		Chassis_Target=cmd->PositionData;
	
	if(Chassis_Target>=CHASSIS_MAX)
		Chassis_Target=CHASSIS_MAX;
	else if(Chassis_Target<=CHASSIS_MIN)
		Chassis_Target=CHASSIS_MIN;


#if   AUTO_AIM == 0   //����Ԥ��	
	//yaw��
	if(cmd->yawMovingType==0x00)
		Gimbal_Target.yaw_angle_target+=cmd->yawData;
	else{
			Gimbal_Target.yaw_angle_target = cmd->yawData;
	}
	
	//pitch��
	
	if(cmd->pitchMovingType==0x00)
		Gimbal_Target.pitch_angle_target+=cmd->pitchData;
	else{
			Gimbal_Target.pitch_angle_target = cmd->pitchData;
	}	

#elif AUTO_AIM == 1   //kalmanԤ�⣬����δ����
	if(cmd->yawMovingType==0x00&&cmd->pitchMovingType==0x00){
		angle_data = kalman_filter_calc(&kalman_filter_F, Gimbal_Target.yaw_angle_target+cmd->yawData, Gimbal_Target.pitch_angle_target+cmd->pitchData;, 0, 0);
	}else if(cmd->yawMovingType==0xFF&&cmd->pitchMovingType==0x00){
		angle_data = kalman_filter_calc(&kalman_filter_F, cmd->yawData, Gimbal_Target.pitch_angle_target+cmd->pitchData;, 0, 0);
	}else if(cmd->yawMovingType==0x00&&cmd->pitchMovingType==0xFF){
		angle_data = kalman_filter_calc(&kalman_filter_F, Gimbal_Target.yaw_angle_target+cmd->yawData, cmd->pitchData;, 0, 0);
	}else{
		angle_data = kalman_filter_calc(&kalman_filter_F, cmd->yawData, cmd->pitchData;, 0, 0);
	}
	Gimbal_Target.yaw_angle_target = angle_data[0];
	Gimbal_Target.pitch_angle_target = angle_data[1];
#else	
	float d1,d2;
	if(cmd->yawMovingType==0x00&&cmd->pitchMovingType==0x00){
		d1=Gimbal_Target.yaw_angle_target+cmd->yawData;
		d2=Gimbal_Target.pitch_angle_target+cmd->pitchData;
	}else if(cmd->yawMovingType==0xFF&&cmd->pitchMovingType==0x00){
		d1=cmd->yawData;
		d2=Gimbal_Target.pitch_angle_target+cmd->pitchData;
	}else if(cmd->yawMovingType==0x00&&cmd->pitchMovingType==0xFF){
		d1=Gimbal_Target.yaw_angle_target+cmd->yawData;
		d2=cmd->pitchData;
	}else{
		d1=cmd->yawData;
		d2=cmd->pitchData;
	}
	if (d1 != last_yaw || d2 != last_pit) {
			angle = aimProcess(d1, d2, &AimTic);
			last_yaw = d1;
			last_pit = d2;
	}
	Gimbal_Target.yaw_angle_target = angle.yaw;
	Gimbal_Target.pitch_angle_target = angle.pitch;
	
#endif	
	cmd->used=1;																			//������Ϣȡ���Ժ�Ҫ���
	

	
	
	
	
	
	
	/*
		if (Gimbal_Target.pitch_angle_target >= -2.0f) {
			Gimbal_Target.pitch_angle_target = -20.0f;
			if (yaw_clockwise_flag == 1) {
				Gimbal_Target.yaw_angle_target += 90.0f;
			}
			else {
				Gimbal_Target.yaw_angle_target -= 90.0f;
			}
		}	
	*/
	
/*	
	switch (cmd->gimbalMovingCtrType) {
	case GIMBAL_CMD_MOVEBY: {
		Gimbal_Target.yaw_angle_target += cmd->d1;
		Gimbal_Target.pitch_angle_target += cmd->d2;
	}break;
	case GIMBAL_CMD_MOVETO: {
#if   AUTO_AIM == 0   //����Ԥ��
		Gimbal_Target.yaw_angle_target = cmd->d1;
		Gimbal_Target.pitch_angle_target = cmd->d2;
#elif AUTO_AIM == 1   //kalmanԤ�⣬����δ����
		angle_data = kalman_filter_calc(&kalman_filter_F, cmd->d1, cmd->d2, 0, 0);
		Gimbal_Target.yaw_angle_target = angle_data[0];
		Gimbal_Target.pitch_angle_target = angle_data[1];
#else                 //��ֲ�Ͻ���Ԥ���㷨��δ����
		if (cmd->d1 != last_yaw || cmd->d2 != last_pit) {
			angle = aimProcess(cmd->d1, cmd->d2, &AimTic);
			last_yaw = cmd->d1;
			last_pit = cmd->d2;
		}
		Gimbal_Target.yaw_angle_target = angle.yaw;
		Gimbal_Target.pitch_angle_target = angle.pitch;
#endif
		if (Gimbal_Target.pitch_angle_target >= -2.0f) {
			Gimbal_Target.pitch_angle_target = -20.0f;
			if (yaw_clockwise_flag == 1) {
				Gimbal_Target.yaw_angle_target += 90.0f;
			}
			else {
				Gimbal_Target.yaw_angle_target -= 90.0f;
			}
		}
	}break;
	default: {

	}break;
	}
	*/
	
	
}
/**
  * @author         ���˻�
  * @brief          ��̨�����״̬����
  * @data           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void GMYawPitchModeSwitch(void) {
	static int16_t  Cruise_Time_Between = 0;
	static uint8_t  pitch_dowm_flag = 0;
	static uint8_t  first_prepare_flag = 1;
	static uint8_t  first_cruise_flag = 1;
	switch (GetWorkState()) {
	case PREPARE_STATE: {
		Gimbal_Target.yaw_angle_target = GET_YAW_ANGLE;
		if (first_prepare_flag == 1 && GMPitchEncoder[0].ecd_angle != 0&&GMPitchEncoder[1].ecd_angle != 0) {
			#if PITCH_SENSOR_BY_ONLY_ONE==1
			Gimbal_Target.pitch_angle_target = PITCH_INIT_ANGLE;
			first_pit_angle[0] = GMPitchEncoder[0].ecd_angle;
			#else
			Gimbal_Target.pitch_angle_target = PITCH_INIT_ANGLE;
			first_pit_angle[0] = GMPitchEncoder[0].ecd_angle;
			first_pit_angle[1] = GMPitchEncoder[1].ecd_angle;
			#endif
			first_prepare_flag = 0;
		};
	}break;
	case CRUISE_STATE: {
		if (first_cruise_flag == 1) {
			Set_Flag(Gimble_ok);
			//SetFrictionState(FRICTION_WHEEL_ON);
			first_cruise_flag = 0;
		};
#if DEBUG_YAW_PID == 0
		Cruise_Time_Between++;
		if (Cruise_Time_Between > 10) {
			if (yaw_clockwise_flag == 1) {
				Gimbal_Target.yaw_angle_target += GIMBAL_YAW_CRUISE_DELTA;
			}
			else {
				Gimbal_Target.yaw_angle_target -= GIMBAL_YAW_CRUISE_DELTA;
			}

			/*
			if (Gimbal_Target.yaw_angle_target >= 3600) {
				yaw_clockwise_flag = 0;
			}
			else if (Gimbal_Target.yaw_angle_target <= -3600) {
				yaw_clockwise_flag = 1;
			}*/
			
			if (Gimbal_Target.yaw_angle_target >= 180) {
				yaw_clockwise_flag = 0;
			}
			else if (Gimbal_Target.yaw_angle_target <= -180) {
				yaw_clockwise_flag = 1;
			}

			if (pitch_dowm_flag == 1) {
				Gimbal_Target.pitch_angle_target -= GIMBAL_PITCH_CRUISE_DELTA;
			}
			else {
				Gimbal_Target.pitch_angle_target += GIMBAL_PITCH_CRUISE_DELTA;
			}

			if (Gimbal_Target.pitch_angle_target > PITCH_MAX - 5) {
				pitch_dowm_flag = 1;
			}
			if (Gimbal_Target.pitch_angle_target < PITCH_MIN + 5) {
				pitch_dowm_flag = 0;
			}
			Cruise_Time_Between = 0;
		}
#endif

#if DEBUG_YAW_PID == 1
		static int i = 0;
		if (i == 4000) {
			Gimbal_Target.yaw_angle_target += 50;
		}
		else if (i >= 8000) {
			Gimbal_Target.yaw_angle_target -= 50;
			i = 0;
		}
		i++;
#endif
	}break;
	case SHOOT_STATE: {
		UpperMonitorControlLoop();
	}break;
	case CONTROL_STATE: {
#if DEBUG_YAW_PID == 1
		static int i = 0;
		if (i == 5000) {
			Gimbal_Target.yaw_angle_target += 50;
		}
		else if (i >= 10000) {
			Gimbal_Target.yaw_angle_target -= 50;
			i = 0;
		}
		i++;
#endif
	}break;
	}

}

/**
  * @author         ���˻�
  * @brief          yaw�������
  * @data           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void GMYawControlLoop(void) {
	GimbalYawAngleLimit();
	
	GMYPositionPID.ref = Gimbal_Target.yaw_angle_target;
	GMYPositionPID.fdb = -GET_YAW_ANGLE;
	GMYPositionPID.Calc(&GMYPositionPID);

	GMYSpeedPID.ref = GMYPositionPID.output;

#if DEBUG_YAW_PID == 2
	//    static int i=0;
	//    if(i==4000){
	//        GMYSpeedPID.ref = 100;
	//    }
	//    else if(i>=8000){
	//        GMYSpeedPID.ref = -100;
	//        i=0;
	//    }
	//    i++;
	GMYSpeedPID.ref = 0;
#endif

	GMYSpeedPID.fdb = GET_YAW_ANGULAR_SPEED;
	GMYSpeedPID.Calc(&GMYSpeedPID);
	//GMYSpeedPID.output=GMYPositionPID.output;
}

/**
  * @author         ���˻�
  * @brief          pitch�������
  * @latest         2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void GMPitchControlLoop(void) {
#if  PITCH_SENSOR_BY_ONLY_ONE==0
	float  getRampTmp = 0;
#endif
#if 0
	static int i = 0;
	if (i == 5000) {
		Gimbal_Target.pitch_angle_target = -5;
	}
	else if (i >= 10000) {
		Gimbal_Target.pitch_angle_target = -30;
		i = 0;
	}
	i++;
#endif

	GimbalPitchAngleLimit();
#if  PITCH_SENSOR_BY_ONLY_ONE==0	
	getRampTmp=GMPitchRamp.Calc(&GMPitchRamp);
	GMPPositionPID[0].ref = (first_pit_angle[0] + (Gimbal_Target.pitch_angle_target - first_pit_angle[0])* getRampTmp) *STABLE_PITCH_MOVE;
	GMPPositionPID[1].ref = (first_pit_angle[1] + (Gimbal_Target.pitch_angle_target-19.92 - first_pit_angle[1])* getRampTmp) *STABLE_PITCH_MOVE;
	for(int i=0;i<2;i++){
	GMPPositionPID[i].fdb = GMPitchEncoder[i].ecd_angle*STABLE_PITCH_MOVE;
	GMPPositionPID[i].Calc(&GMPPositionPID[i]);


	GMPSpeedPID[i].ref = GMPPositionPID[i].output/STABLE_PITCH_MOVE;
	GMPSpeedPID[i].fdb = GET_PITCH_ANGULAR_SPEED;
	GMPSpeedPID[i].Calc(&GMPSpeedPID[i]);
	}

	GMPSpeedPID[1].output=-GMPSpeedPID[1].output;
#else
	//GMPPositionPID[0].ref = first_pit_angle[0] + (Gimbal_Target.pitch_angle_target - first_pit_angle[0]) * GMPitchRamp.Calc(&GMPitchRamp);
	GMPPositionPID[0].ref = (first_pit_angle[0] + (Gimbal_Target.pitch_angle_target - first_pit_angle[0])* GMPitchRamp.Calc(&GMPitchRamp))*STABLE_PITCH_MOVE;
	GMPPositionPID[0].fdb = GMPitchEncoder[0].ecd_angle*STABLE_PITCH_MOVE;
	GMPPositionPID[0].Calc(&GMPPositionPID[0]);
	

	GMPSpeedPID[0].ref = ((float)GMPPositionPID[0].output/STABLE_PITCH_MOVE);
	GMPSpeedPID[0].fdb = (GET_PITCH_ANGULAR_SPEED);	
	//GMPSpeedPID[0].fdb=(-PitchInfo[0].speed);
	GMPSpeedPID[0].Calc(&GMPSpeedPID[0]);

	GMPSpeedPID[1].output=-GMPSpeedPID[0].output;
#endif	

}

void SetGimbalMotorOutput(void) {
	Set_Gimbal_Current(CAN2, -(int16_t)GMYSpeedPID.output, -(int16_t)GMPSpeedPID[0].output,-(int16_t)GMPSpeedPID[1].output);
}

void RammerSpeedPID(int num, int16_t TargetSpeed) {
	RAMMERSpeedPID[num].ref = TargetSpeed;
	RAMMERSpeedPID[num].fdb = Rammer[num].speed;
	RAMMERSpeedPID[num].Calc(&RAMMERSpeedPID[num]);
}

void FrictionSpeedPID1(int num, int16_t TargetSpeed) {
	FRICTIONSpeedPID1[num].ref = TargetSpeed;
	FRICTIONSpeedPID1[num].fdb = FrictionT1[num].speed;
	FRICTIONSpeedPID1[num].Calc(&FRICTIONSpeedPID1[num]);
}
void FrictionSpeedPID2(int num, int16_t TargetSpeed) {
	FRICTIONSpeedPID2[num].ref = TargetSpeed;
	FRICTIONSpeedPID2[num].fdb = FrictionT2[num].speed;
	FRICTIONSpeedPID2[num].Calc(&FRICTIONSpeedPID2[num]);
}

/**
  * @author         ���˻�
  * @brief          ǹ����������
  * @date           2019.7.9
  * @param[in]      �Ӳ���ϵͳ��ȡ��ǹ�������� �Ӳ���ϵͳ��ȡ��ʵʱ����
  * @retval         ����ֹͣ����1�����򷵻�0
  */
#define SENTRY_HEAT_THRESHOLD   480
#define VELOCITY_THRESHOLD      30

int8_t heat_control(volatile int16_t const current_heat, volatile float const current_velocity) {
	//    static int8_t available_bullet_num = 0;
	//    static int8_t launched_bullet_num = 0;
	//    static float velocity_last = 0;
	//    static float velocity_now = 0;
	//    static int16_t heat_last = 0;
	//    static int16_t heat_now = 480;
	static int8_t heat_control_flag = 0;

	//    available_bullet_num = (SENTRY_HEAT_THRESHOLD - current_heat) / VELOCITY_THRESHOLD - 1;

	//    velocity_last = velocity_now;
	//    velocity_now = current_velocity;
	//    
	//    heat_last = heat_now;
	//    heat_now = current_heat;
	//    if(heat_last != heat_now){
	//        launched_bullet_num = 0;           //�������£����ѷ����ӵ�����
	//    }
	//    
	//    if(velocity_now != velocity_last){     //�������ݸ���
	//        launched_bullet_num++;             //�ѷ����ӵ���һ
	//    }
	//    if(launched_bullet_num >= available_bullet_num){         //�ѷ����ӵ������ڵ��ڿɷ����ӵ���
	//        heat_control_flag = 1;             //����ֹͣ
	//        launched_bullet_num = 0;           //�ӵ�����
	//    }
	//    else{
	//        heat_control_flag = 0;
	//    }
	//���������ݸ���Ƶ�ʽϸ�ʱ����ֱ��������Ĺ�ʽ���ƣ�����Ч
	if ((SENTRY_HEAT_THRESHOLD - current_heat) <= 35) {
		heat_control_flag = 1;
	}
	else {
		heat_control_flag = 0;
	}

	return heat_control_flag;
}

/**
  * @brief          �����������
  * @author         ���˻�
   *@param[in]      NULL
  * @retval         ���ؿ�
  */
#define RAMMER_TORQUE_THRESHOLD  13000  //��תŤ��
#define RAMMER_INVERSE_TIME_MS   800   //��תʱ��
#define RAMMER_INVERSE_SPEED     -2000  //��ת�ٶ�
#define RAMMER_NORMAL_SPEED      4000   //���̸�ת��  ת/min
void ShootControlLoop(void) {
	volatile static int8_t  heat_control_flag[2] = { 0,0 };
	volatile static int16_t rammer_speed[2] = { 0,0 };
	volatile static int16_t t_inversion[2] = { 0,0 };
	for (int i = 0; i < 2; i++) {
		heat_control_flag[i] = heat_control(Get_Sentry_HeatData(), Get_Sentry_BulletSpeed());
		if (Rammer[i].torque > RAMMER_TORQUE_THRESHOLD) {
			//Ť��̫��˵����ת����״̬��ͬʱ��ס��
			//���÷�תʱ��RAMMER_INVERSE_TIME_MS / 2
			t_inversion[i] = RAMMER_INVERSE_TIME_MS / 2;
		}
		if (t_inversion[i] > 0) {
			//��ת
			rammer_speed[i] = RAMMER_INVERSE_SPEED;
			t_inversion[i]--;
		}else if(Get_Flag(Shoot) == 0 ||FriOk[i]==0)
			//��Ħ����û������û���������ת
			rammer_speed[i]=0;
		else {
			if(i==1)
			rammer_speed[i] = -(RAMMER_NORMAL_SPEED+300);
			else
				rammer_speed[i] = RAMMER_NORMAL_SPEED;
		}



//		if (Get_Flag(Shoot) == 0 || Get_Flag(Auto_aim_debug) == 1 || heat_control_flag[i] == 1 || pit_angle_limit_flag == 1) {
//			rammer_speed[i] = 0;
//			if(FriOk[i]==0)
//				rammer_speed[i]=0;
//		}
		//    else if(GET_PITCH_ANGLE > -10){
		//        rammer_speed = 1500;
		//    }
		//    else if(GET_PITCH_ANGLE < -15 && Get_RunAway_State() == 0){
		//        rammer_speed = RAMMER_NORMAL_SPEED;
		//    }


		RammerSpeedPID(i, rammer_speed[i]);

	}
	Set_Rammer_Current((int16_t)RAMMERSpeedPID[0].output, (int16_t)RAMMERSpeedPID[1].output);
}

/**
  * @brief          Ħ���ֳ�ʼ��
  * @author         ���˻�
   *@param[in]      NULL
  * @retval         ���ؿ�
  */
void friction_init(void) {
	FrictionRamp1.SetScale(&FrictionRamp1, FRICTION_RAMP_TICK_COUNT);
	FrictionRamp1.ResetCounter(&FrictionRamp1);
	FrictionRamp2.SetScale(&FrictionRamp2, FRICTION_RAMP_TICK_COUNT);
	FrictionRamp2.ResetCounter(&FrictionRamp2);
	FRICTIONSpeedPID1[0].Reset(&FRICTIONSpeedPID1[0]);
	FRICTIONSpeedPID1[1].Reset(&FRICTIONSpeedPID1[1]);
	FRICTIONSpeedPID2[0].Reset(&FRICTIONSpeedPID2[0]);
	FRICTIONSpeedPID2[1].Reset(&FRICTIONSpeedPID2[1]);
	SetFrictionState(FRICTION_WHEEL_OFF);
}

/**
  * @brief          Ħ���ֿ��ؿ��ƣ�ͨ����ȡö��FrictionWheelState_e�����ж�״̬������һ��ת��������һ��ת��
  * @author         ���˻�
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void friction_control(void) {
	//һ��ǹ�����������
	static int first_start_friction = 1;
	static int first_stop_friction = 0;
	static int stop_flag = 0;
	static int friction_wheel_speed_1[2] = { 0,0 };									//��һ��ǹ�ܵ����������Ŀ���ٶ�						
	static int friction_wheel_speed_2[2] = { 0,0 };									//�ڶ���ǹ�ܵ����������Ŀ���ٶ�
	static int16_t friction_wheel_initial_speed_1[2] = { 0,0 };
	static int16_t friction_wheel_initial_speed_2[2] = { 0,0 };
	float  getRampTmp = 0;																					//һ��ǹ�ܹ���һ��б�º���
	uint8_t oneIsDone=0;
	
	if (workState == SHOOT_STATE) {
		if (GET_PITCH1_ANGLE <= -25) {
			Set_FRICTION_WHEEL_MAX_DUTY(-1205-2000);
		}
		else {
			Set_FRICTION_WHEEL_MAX_DUTY(-1320-2000);
		}
	}

	if (GetFrictionState() == FRICTION_WHEEL_ON) {                //Ħ���ֿ�
		if (first_start_friction == 1) {
			FrictionRamp1.ResetCounter(&FrictionRamp1);
			FrictionRamp2.ResetCounter(&FrictionRamp2);
			friction_wheel_initial_speed_1[0] = FrictionT1[0].speed;
			friction_wheel_initial_speed_1[1] = FrictionT1[1].speed;
			first_stop_friction = 1;
			first_start_friction = 0;
			oneIsDone=0;																							//������ʶ��һǹ�ܸո�������ͣ������һ˲��
		}
		if ((!FrictionRamp1.IsOverflow(&FrictionRamp1)) || (!FrictionRamp2.IsOverflow(&FrictionRamp2))) {

			//������һ��ǹ��
			getRampTmp = FrictionRamp1.Calc(&FrictionRamp1);
			friction_wheel_speed_1[0] = friction_wheel_initial_speed_1[0] + (FRICTION_WHEEL_MAX_DUTY1_1 - friction_wheel_initial_speed_1[0])*getRampTmp;
			friction_wheel_speed_1[1] = friction_wheel_initial_speed_1[1] + (FRICTION_WHEEL_MAX_DUTY1_2 - friction_wheel_initial_speed_1[1])*getRampTmp;
			
			if (FrictionRamp1.IsOverflow(&FrictionRamp1)) {
				//�������Ժ�������һ��
				FriOk[0]=1;
				if(!oneIsDone){
					oneIsDone=1;
					friction_wheel_initial_speed_2[0] = FrictionT2[0].speed;
					friction_wheel_initial_speed_2[1] = FrictionT2[1].speed;		
				}
				getRampTmp = FrictionRamp2.Calc(&FrictionRamp2);
				friction_wheel_speed_2[0] = friction_wheel_initial_speed_2[0] + (FRICTION_WHEEL_MAX_DUTY2_1 - friction_wheel_initial_speed_2[0])*getRampTmp;
				friction_wheel_speed_2[1] = friction_wheel_initial_speed_2[1] + (FRICTION_WHEEL_MAX_DUTY2_2 - friction_wheel_initial_speed_2[1])*getRampTmp;
				if(FrictionRamp2.IsOverflow(&FrictionRamp2)){
										FriOk[1]=1;
				           stop_flag = 1;
				                //�˴���Ӳ��̿�
				}
			}
		}
		else {
			//��������Ժ�ֱ�Ӹ�ָ���ٶ�
			//�������¶ȿ���
			friction_wheel_speed_1[0] = FRICTION_WHEEL_MAX_DUTY1_1;
			friction_wheel_speed_1[1] = FRICTION_WHEEL_MAX_DUTY1_2;
			friction_wheel_speed_2[0] = FRICTION_WHEEL_MAX_DUTY2_1;
			friction_wheel_speed_2[1] = FRICTION_WHEEL_MAX_DUTY2_2;
		}

		//        friction_wheel_speed_1 = FRICTION_WHEEL_MAX_DUTY;
		//        friction_wheel_speed_2 = FRICTION_WHEEL_MAX_DUTY;
	}
	else if (GetFrictionState() == FRICTION_WHEEL_OFF) {          //Ħ���ֹ�
		//�˴���Ӳ��̹�
		if (stop_flag == 1) {
			if (first_stop_friction == 1) {
				FrictionRamp1.ResetCounter(&FrictionRamp1);
				FrictionRamp2.ResetCounter(&FrictionRamp2);
				friction_wheel_initial_speed_2[0] = FrictionT2[0].speed;
				friction_wheel_initial_speed_2[1] = FrictionT2[1].speed;
				FriOk[0]=FriOk[1]=0;
				first_start_friction = 1;
				first_stop_friction = 0;
				oneIsDone=0;																							//������ʶ��һǹ�ܸո�������ͣ������һ˲��
			}
			//�Ƚ�һ��ǹ��ͣ��
			getRampTmp = FrictionRamp2.Calc(&FrictionRamp2);
			friction_wheel_speed_2[0] = friction_wheel_initial_speed_2[0] + (0 - friction_wheel_initial_speed_2[0])*getRampTmp;
			friction_wheel_speed_2[1] = friction_wheel_initial_speed_2[1] + (0 - friction_wheel_initial_speed_2[1])*getRampTmp;
			if (FrictionRamp2.IsOverflow(&FrictionRamp2)) {
				//ͣ���Ժ�ͣ��һ��ǹ��
				if(!oneIsDone){
					oneIsDone=1;
					friction_wheel_initial_speed_1[0] = FrictionT1[0].speed;
					friction_wheel_initial_speed_1[1] = FrictionT1[1].speed;
				}			
				getRampTmp = FrictionRamp1.Calc(&FrictionRamp1);
				friction_wheel_speed_1[0] = friction_wheel_initial_speed_1[0] + (0 - friction_wheel_initial_speed_1[0])*getRampTmp;
				friction_wheel_speed_1[1] = friction_wheel_initial_speed_1[1] + (0 - friction_wheel_initial_speed_1[1])*getRampTmp;
				if (FrictionRamp1.IsOverflow(&FrictionRamp1)) {
					//��ͣ���Ժ󣬾�ֱ�Ӹ��ٶ���
					stop_flag = 0;
				}
			}
		}else if(stop_flag == 0){
				friction_wheel_speed_1[0]=0;
				friction_wheel_speed_1[1]=0;
				friction_wheel_speed_2[0]=0;
				friction_wheel_speed_2[1]=0;
	
		}
		//        friction_wheel_speed_1 = 1000;
		//        friction_wheel_speed_2 = 1000;
		//        first_start_friction = 1;
	}
	for (int i = 0; i < 2; i++) {
		FrictionSpeedPID1(i, friction_wheel_speed_1[i]);
		FrictionSpeedPID2(i, friction_wheel_speed_2[i]);

	}

	//SetFrictionWheelSpeed_1(friction_wheel_speed_1);
	//SetFrictionWheelSpeed_2(friction_wheel_speed_2);
//	Set_Friction_Current((int16_t)FRICTIONSpeedPID1[0].output, (int16_t)FRICTIONSpeedPID1[1].output, (int16_t)FRICTIONSpeedPID2[0].output, (int16_t)FRICTIONSpeedPID2[1].output);
	Set_Friction_Current((int16_t)FRICTIONSpeedPID1[0].output, (int16_t)FRICTIONSpeedPID1[1].output, (int16_t)FRICTIONSpeedPID2[0].output, (int16_t)FRICTIONSpeedPID2[1].output);
}

void Set_FRICTION_WHEEL_MAX_DUTY(uint32_t x) {
	FRICTION_WHEEL_MAX_DUTY1_1 = x;
}

FrictionWheelState_e GetFrictionState() {
	return friction_wheel_state;
}

void SetFrictionState(FrictionWheelState_e v) {
	friction_wheel_state = v;
}

/**
  * @author         ���˻�
  * @brief          ��̨���������ʼ��
  * @date           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void ControtTaskInit(void) {
	GMPitchRamp.SetScale(&GMPitchRamp, PREPARE_TIME_TICK_MS);
	GMYawRamp.SetScale(&GMYawRamp, PREPARE_TIME_TICK_MS);
	GMPitchRamp.ResetCounter(&GMPitchRamp);
	GMYawRamp.ResetCounter(&GMYawRamp);
	/*��ʼ������������PID*/
	RAMMERSpeedPID[0].Reset(&RAMMERSpeedPID[0]);
	RAMMERSpeedPID[1].Reset(&RAMMERSpeedPID[1]);
	/*��ʼ������pitch����PID*/
	GMPPositionPID[0].Reset(&GMPPositionPID[0]);
	GMPPositionPID[1].Reset(&GMPPositionPID[1]);
	GMPSpeedPID[0].Reset(&GMPSpeedPID[0]);
	GMPSpeedPID[1].Reset(&GMPSpeedPID[1]);
	/*��ʼ��yaw���pid*/
	GMYPositionPID.Reset(&GMYPositionPID);
	GMYSpeedPID.Reset(&GMYSpeedPID);
	/*��ʼ��Ħ����*/
	friction_init();
	/*��ʼ������(�ٶȺ�λ��)*/
	Chassis_Now_Pos=0;
	Chassis_Target=0;
	kalman_filter_init(&kalman_filter_F, &kalman_filter_init_I);
	SetWorkState(PREPARE_STATE);
}

/**
  * @author         ���˻�
  * @brief          ��̨������������������timer6����1KHz����������
  * @date           2019.5.4
  * @param[in]      NULL
  * @retval         ���ؿ�
  */
void Control_Task(void) {
	volatile static uint8_t time_tick = 0;
	if (minipc_alive_count++ == 1000) {
		SetUpperMonitorOnline(0);
	}
	if (AimTic < 1000) { AimTic++; }  //for auto aim forcasting
	WorkStateFSM();
	GMYawPitchModeSwitch();
	GMYawControlLoop();
	GMPitchControlLoop();
#if Monitor_GM_Encoder == 0 && DISABLE_GIMBLA_OUTPUT == 0
	SetGimbalMotorOutput();
#endif
//	if ((++time_tick) % 2 == 0) {
		friction_control();
		ShootControlLoop();
//	time_tick = 0;
//	}
//	else {
//		Send_Gimbal_Info(Is_Shoot_State(), Is_Control_State(), Get_ChassisSpeed_Target());
//		if(!getAtSYNing())
//			serialSendData();			
//	}
}

void Set_Chassis_Now_Pos(uint8_t v){
	Chassis_Now_Pos=v;
}
uint8_t Get_Chassis_Now_Pos(){
	return Chassis_Now_Pos;
}
