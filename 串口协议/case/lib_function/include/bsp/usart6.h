#ifndef _USART6_H_
#define _USART6_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

void USART6_Configuration(u32 bound);
void USART6_Init(u32 bound);
void USART6_Print(uint8_t* ch, int len);
void USART6_sendChar(uint8_t ch); 
void USART6_Print_num(int buffer);
#ifdef __cplusplus
}
#endif
#endif
