#ifndef __MPU6050_INTERRUPT_H__
#define __MPU6050_INTERRUPT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stm32f4xx.h>

void MPU6050_IntConfiguration(void);
extern uint8_t isMPU6050_is_DRY;

#ifdef __cplusplus
}
#endif
#endif
