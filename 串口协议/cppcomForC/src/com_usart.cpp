#include "com_usart.h"
#include "ModuleSerial.h"
#include "string.h"
#include "FIFO.h"
#include "led.h"
#include "minipc_task.h"
#include "communication.h"
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#endif


#define rxBufferSize 1200			//1200
#define waitingTime 800 
#define procNumAtFirst 240    //ԭ��800�����ڷ���ÿ2ms���ﵽ��240�ֽں���һ�Σ�Ч������

//#define procNumAfter 1800


uint16_t procNum=procNumAtFirst;
uint16_t receivedBytes = 0;		
uint8_t couldRec=1;
//ÿ20ms����һ�δ��ڽ��յ�������

static unsigned char rx_buffer[rxBufferSize+2];				//�������ֽڵĻ���ռ䣬��ֹ����������µı���




void Com_USART_Configuration(u32 bound){
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	if(communicationUart == USART1){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		GPIO_Init(GPIOB, &GPIO_InitStructure);
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
		
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

		
	}else if(communicationUart == USART6){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
		
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOG,&GPIO_InitStructure);
		
		
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &USART_InitStructure);
    USART_Cmd(USART6, ENABLE);
		
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);		
		
	}else{
		while(1){
			printf("����ͨ�ų�ʼ��ʧ�ܣ�����\r\n");
		}
		
	}
	
	
	
	
	
	
	
	memset(rx_buffer,0,sizeof(rx_buffer));	
}

void Com_USART_sendChar(uint8_t ch) {
    while ((communicationUart->SR & 0X40) == 0);
    communicationUart->DR = (u8)ch;
}
void Com_USART_Print(uint8_t* ch, int len) {
    for (int i = 0; i < len; i++) {
        Com_USART_sendChar((u8)ch[i]);
    }
}


extern "C" void Com_UseUART_IRQHandler{					
	if(USART_GetITStatus(communicationUart, USART_IT_RXNE) != RESET){
		//ÿ�ζ����գ��Ƿ�����ɽ��մ���������
    uint8_t rxData = USART_ReceiveData(communicationUart);
		if(couldRec&&receivedBytes<rxBufferSize)											//û�б����ϣ�ͬʱ����δ����
		rx_buffer[receivedBytes++]=rxData;
				
  }
}

void ProcUsartData(){
		if(receivedBytes>=procNum){
			couldRec=0;																									//����
			#if useObject==1
			serialObject.comRecv((char *)rx_buffer,receivedBytes);
			#else
			serialRecvData((char *)rx_buffer,receivedBytes);        		//���ô�����				
			#endif
			receivedBytes=0;
			couldRec=1;																									//����
		}
}

