#include "beep.h"
#include "delay.h"

const uint16_t tone_tab[] = {
  3822,  3405, 3033, 2863, 2551, 2272, 2024,
  1911,  1702, 1526, 1431, 1275, 1136, 1012,
   955,   851,  758,  715,  637,  568,  506,
};

const Sound_tone_e Mavic_Startup_music[Startup_Success_music_len] = {
  So5L, La6L, Mi3M, Silent
};

void BEEP_Configuration(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    GPIO_PinAFConfig(GPIOH,GPIO_PinSource6,GPIO_AF_TIM12);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOH,&GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM12, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM12,ENABLE);
    TIM_Cmd(TIM12, ENABLE);
}

void Sing(Sound_tone_e tone){
  if(Silent == tone)
    BEEP_CH = 0;
  else{
    BEEP_ARR = tone_tab[tone];
    BEEP_CH = tone_tab[tone] / 2;
  }
}

void Sing_Startup_music(void){
    int index = 0;
    while(index < Startup_Success_music_len){
    Sing(Mavic_Startup_music[index++]);
    delay_xms(200);
    }
}
void Sing_miniPC_online(void){
    int index = 5;
    while (index >= 0) {
        Sing(Mavic_Startup_music[index--]);
        delay_xms(200);
    }
    Sing(Silent);
}
void Sing_bad_case(void){
    static int first_error = 1;
    if(first_error == 1){
        for(int i = 0; i < 3; i++){
            Sing(Mi3M);
            delay_xms(1000);
            Sing(Silent);
            delay_xms(1000);
        }
    }
    first_error = 0;
}

void BEEP_Init(void){
    BEEP_Configuration();
    Sing_Startup_music();
}
