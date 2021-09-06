#include "bsp.h"
#include "sys.h"
#include "control_task.h"
#include "remote_task.h"
#include "gun.h"
#include "laser.h"
#include "usart6.h"
#include "key.h"
#include "led.h"
#include "beep.h"
#include "timer.h"
#include "spi.h"
#include "imu.h"
#include "can1.h"
#include "can2.h"
#include "dbus.h" 
#include "delay.h"
#include "usart2.h"
#include "usart3.h"
#include "flags.h"
#include "power.h"
#include "adc.h"
#include "communication.h"

void BSP_Pre_Init(void){
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    KEY_Init();
    LED_Init(); 
    BEEP_Init();

    delay_ms(100);
    //USART2_Init(115200);
    USART3_Init(115200);
    //USART6_Init(115200);
    
    SPI5_Init();
    power_init();  //24V��Դ���ο���
}

void BSP_Init(void)
{
    ControtTaskInit();
    Flags_Init();

    Gun_Init();
#if Calibrate_Snail_ESC == 1       //����У׼��������--1ms�ߵ�ƽ ������--2ms�ߵ�ƽ
    SetFrictionWheelSpeed(2000);        
    delay_ms(3000);
    SetFrictionWheelSpeed(1000);
    delay_ms(3000);
#endif
#if Calibrate_Snail_ESC == 2       //ת���л�����Ҫ�ֶ�ת��Ħ���ָı�Ħ���ֵķ���
    SetFrictionWheelSpeed(2000);
    delay_ms(6000);
    SetFrictionWheelSpeed(1000);
    delay_ms(3000);
#endif
//    Laser_Init();
    delay_ms(100);
    imu_init();
    CAN1_Init();
    CAN2_Init();
    Dbus_Init();
    TIM6_Init();
    TIM6_Start();
		TIM3_Init(84,5000);
		
		
		//��ʼ������Э��
	initSerial(0x03,115200,0x01,0x02);
	serialAddNewSendData("COL", 'P', 0);
	//serialAddNewSendData("P1", 'P', 0);					//��ǰλ��
	//serialAddNewSendData("P2", 'P', 0);					//pitch��Ƕ�
	//serialAddNewSendData("P3", 'P', 0);					//yaw��Ƕ�
	//serialAddNewSendData("P4", 'P', 0);					//����
}
