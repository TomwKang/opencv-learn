#ifndef _USART7_H_
#define _USART7_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

void USART7_Configuration(u32 bound);
void USART7_Init(u32 bound);
void USART7_Print(uint8_t* ch, int len);
void USART7_sendChar(uint8_t ch); 
void USART7_Print_num(int buffer);
void ProcUsartData();
#ifdef __cplusplus
}
#endif
#endif
