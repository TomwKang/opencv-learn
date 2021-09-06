#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

//�ĸ�24v ��� ���ο��� ��� 709us
#define POWER_CTRL_ONE_BY_ONE_TIME 709

#define POWER1_CTRL_SWITCH 0
#define POWER2_CTRL_SWITCH 1
#define POWER3_CTRL_SWITCH 2
#define POWER4_CTRL_SWITCH 3

extern void power_init(void);
extern void power_ctrl_on(uint8_t num);
extern void power_ctrl_off(uint8_t num);
extern void power_ctrl_toggle(uint8_t num);

#ifdef __cplusplus
}
#endif
#endif
