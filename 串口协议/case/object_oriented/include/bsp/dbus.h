#ifndef _DEBUS_H_
#define _DEBUS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

static uint8_t Dbus_rx_buffer[25];
void Dbus_Init(void);

#ifdef __cplusplus
}
#endif
#endif
