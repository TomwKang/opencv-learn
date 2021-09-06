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


#define rxBufferSize 1200
#define waitingTime 800 
#define procNumAtFirst 800
//#define procNumAfter 1800


uint16_t procNum=procNumAtFirst;
uint16_t receivedBytes = 0;		
uint8_t couldRec=1;
//ÿ20ms����һ�δ��ڽ��յ�������

static unsigned char rx_buffer[rxBufferSize+2];				//�������ֽڵĻ���ռ䣬��ֹ����������µı���




void Com_USART_Configuration(u32 bound){
  USART_InitTypeDef usart;
  GPIO_InitTypeDef  gpio;
  NVIC_InitTypeDef  nvic;
  Com_Periph_GPIO;
	Com_UART_PeriphClock;
  GPIO_PinAFConfig(Com_UART_GPIO,Com_UART_GPIO_PinSource_1,Com_UART_GPIOAF_Name);
	GPIO_PinAFConfig(Com_UART_GPIO,Com_UART_GPIO_PinSource_2,Com_UART_GPIOAF_Name);
  gpio.GPIO_Pin = Com_UART_GPIO_Pin;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_100MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(Com_UART_GPIO,&gpio);
  usart.USART_BaudRate = bound;
  usart.USART_WordLength = USART_WordLength_8b;
  usart.USART_StopBits = USART_StopBits_1;
  usart.USART_Parity = USART_Parity_No;
  usart.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(communicationUart,&usart);
  USART_ITConfig(communicationUart,USART_IT_RXNE,ENABLE);
  USART_Cmd(communicationUart,ENABLE);
  nvic.NVIC_IRQChannel = Com_UART_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 1;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&nvic);
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
//			if(serialObject.getRecvRenm()>9000)
//				procNum=procNumAfter;
			
			serialObject.comRecv((char *)rx_buffer,receivedBytes);
			#else
//			if(serialGetSeq()>9000)
//				procNum=procNumAfter;
			
			serialRecvData((char *)rx_buffer,receivedBytes);        		//���ô�����				
			#endif
			receivedBytes=0;
			couldRec=1;																									//����
		}
}







