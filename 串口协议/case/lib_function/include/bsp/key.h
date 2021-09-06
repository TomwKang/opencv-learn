#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h" 

#define KEY    PBin(2)

void KEY_Configuration(void);
void Key_Touch(void);
void KEY_Init(void);
#ifdef __cplusplus
}
#endif
#endif
