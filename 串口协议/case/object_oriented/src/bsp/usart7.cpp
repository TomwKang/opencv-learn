#include "usart7.h"
#include "string.h"
#include "FIFO.h"
#include "led.h"
#include "minipc_task.h"
#include "communication.h"
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#endif


#define rxBufferSize 1020
#define waitingTime 800 
#define procNum 4000
uint16_t receivedBytes = 0;		
uint8_t couldRec=1;

//每20ms处理一次串口接收到的数据

static unsigned char rx_buffer[rxBufferSize+2];				//多两个字节的缓冲空间，防止意外溢出导致的崩溃
void USART7_Configuration(u32 bound){
  USART_InitTypeDef usart7;
  GPIO_InitTypeDef  gpio;
  NVIC_InitTypeDef  nvic;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7 ,ENABLE);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_UART7);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_UART7);
  gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_100MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE,&gpio);
  usart7.USART_BaudRate = bound;
  usart7.USART_WordLength = USART_WordLength_8b;
  usart7.USART_StopBits = USART_StopBits_1;
  usart7.USART_Parity = USART_Parity_No;
  usart7.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  usart7.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(UART7,&usart7);
  USART_ITConfig(UART7,USART_IT_RXNE,ENABLE);
  USART_Cmd(UART7,ENABLE);
  nvic.NVIC_IRQChannel = UART7_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&nvic);
	memset(rx_buffer,0,sizeof(rx_buffer));
}

void USART7_sendChar(uint8_t ch) {
    while ((UART7->SR & 0X40) == 0);
    UART7->DR = (u8)ch;
}
void USART7_Print(uint8_t* ch, int len) {
    for (int i = 0; i < len; i++) {
        USART7_sendChar((u8)ch[i]);
    }
}
void USART7_Print_num(int buffer){
	char end[3]="\r\n";
	int16_t temp1[20];
	int8_t i=0;
	int temp2=0;
	temp2=buffer;
		if(buffer<0)
			temp2=-temp2;
		if(temp2!=0)
		while(temp2!=0){
			temp1[i++]=temp2%10;
			temp2/=10;
		}
		else
			temp1[i++]=0;
		
		i--;
		
		if(buffer<0)
			USART7_sendChar('-');			
		for(;i>=0;i--)
			USART7_sendChar((char)(temp1[i]+'0'));
		USART7_Print((uint8_t*)end,3);


}

extern "C" void UART7_IRQHandler(void){	
	
	if(USART_GetITStatus(UART7, USART_IT_RXNE) != RESET){
		//每次都接收，是否出错由接收处理函数处理
		

		static uint8_t count=0;
    uint8_t rxData = USART_ReceiveData(UART7);
//		if(couldRec)
		rx_buffer[receivedBytes++]=rxData;
		/*
		if(receivedBytes>=3){
			serialRecvData((char *)rx_buffer,receivedBytes);        		//调用处理函数
			//memset(rx_buffer,0,sizeof(rx_buffer));							//清空接收缓存
			receivedBytes=0;
		}*/

		
  }
	
}

void USART7_Init(u32 bound){
  USART7_Configuration(bound);
}
void ProcUsartData(){

		if(receivedBytes>=procNum){
			couldRec=0;
			serialRecvData((char *)rx_buffer,receivedBytes);        		//调用处理函数
			receivedBytes=0;
			couldRec=1;
		}
}
