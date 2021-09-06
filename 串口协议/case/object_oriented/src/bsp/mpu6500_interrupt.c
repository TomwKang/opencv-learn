
#include "main.h"

uint8_t isMPU6050_is_DRY = 0;   // mpu6050 interrupt�жϱ�־

void MPU6050_IntConfiguration(void)
{
	GPIO_InitTypeDef    gpio;
    NVIC_InitTypeDef    nvic;
    EXTI_InitTypeDef    exti;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,  ENABLE);  
	
	gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOE, &gpio);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,GPIO_PinSource1); 
	
    exti.EXTI_Line = EXTI_Line1;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;//�½����ж�
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);
    
    nvic.NVIC_IRQChannel = EXTI1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void EXTI1_IRQHandler(void)         //�ж�Ƶ��1KHz
{   
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {    
        EXTI_ClearFlag(EXTI_Line1);          
        EXTI_ClearITPendingBit(EXTI_Line1);
        //��ȡԭʼ����
        isMPU6050_is_DRY = 1;   //mpu6050�жϱ�־
        GetPitchYawGxGyGz();//��ȡ��̬����,�����Ѿ������������ʽ	
    }
}
