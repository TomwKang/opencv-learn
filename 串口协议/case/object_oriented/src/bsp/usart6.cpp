#include "usart6.h"
#include "string.h"
#include "FIFO.h"
#include "led.h"
#include "minipc_task.h"
#include "communication.h"
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#endif


#define rxBufferSize 100
#define waitingTime 1000  
uint16_t countTime=0;
//每20ms处理一次串口接收到的数据
FIFO_S_t* UART_TranFifo;
static unsigned char rx_buffer[rxBufferSize+2];				//多两个字节的缓冲空间，防止意外溢出导致的崩溃
void USART6_Configuration(u32 bound){
  USART_InitTypeDef usart6;
  GPIO_InitTypeDef  gpio;
  NVIC_InitTypeDef  nvic;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
  gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_100MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG,&gpio);
  usart6.USART_BaudRate = bound;
  usart6.USART_WordLength = USART_WordLength_8b;
  usart6.USART_StopBits = USART_StopBits_1;
  usart6.USART_Parity = USART_Parity_No;
  usart6.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  usart6.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART6,&usart6);
  USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
  USART_Cmd(USART6,ENABLE);
  nvic.NVIC_IRQChannel = USART6_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&nvic);
  UART_TranFifo = FIFO_S_Create(100);
	memset(rx_buffer,0,sizeof(rx_buffer));
}

void USART6_sendChar(uint8_t ch) {
    while ((USART6->SR & 0X40) == 0);
    USART6->DR = (u8)ch;
}
void USART6_Print(uint8_t* ch, int len) {
    for (int i = 0; i < len; i++) {
        USART6_sendChar((u8)ch[i]);
    }
}
void USART6_Print_num(int buffer){
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
			USART6_sendChar('-');			
		for(;i>=0;i--)
			USART6_sendChar((char)(temp1[i]+'0'));
		USART6_Print((uint8_t*)end,3);


}
/*
uint8_t couldProcessData(uint16_t lastTime1,uint16_t thisTime1){
	uint16_t countWaitingTime=0;
	countWaitingTime=(thisTime1>lastTime1)?(thisTime1-lastTime1):(65535-lastTime1+thisTime1);
	if(countWaitingTime>=waitingTime)
		return 1;
	else
		return 0;
}
*/

extern "C" void USART6_IRQHandler(void){
	/*
  if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET){
    if(!FIFO_S_IsEmpty(UART_TranFifo)){
            uint16_t data = (uint16_t)FIFO_S_Get(UART_TranFifo);
            USART_SendData(USART6, data);
    }
    else{
            USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
    }
  }
  else 
	*/
					
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET){
		//每次都接收，是否出错由接收处理函数处理
		
    static uint8_t receivedBytes = 0;		
		static uint8_t count=0;
    uint8_t rxData = USART_ReceiveData(USART6);
		rx_buffer[receivedBytes++]=rxData;
		if(receivedBytes>=3){
			serialRecvData((char *)rx_buffer,receivedBytes);        		//调用处理函数
			//memset(rx_buffer,0,sizeof(rx_buffer));							//清空接收缓存
			receivedBytes=0;
		}

		/*
		if(countTime++>200){
			countTime=0;
			freeBuffer();
		}*/
		//printf("%c",rxData);
		/*
		rx_buffer[receivedBytes++]=rxData;
		
		if(rxData=='\xff')
			count++;
		if(receivedBytes>=rxBufferSize-1||(count==4)){
			//for(int t=0;t<receivedBytes;t++)
			//printf("%c",rx_buffer[t]);
			//printf("\r\n");
			count=2;
			serialRecvData((char *)rx_buffer,receivedBytes);        		//调用处理函数
			memset(rx_buffer,0,sizeof(rx_buffer));							//清空接收缓存
			receivedBytes=0;
		}*/


		
  }
}

void USART6_Init(u32 bound){
  USART6_Configuration(bound);
}
