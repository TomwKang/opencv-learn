#ifndef __AUTO_AIM_H__
#define __AUTO_AIM_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

typedef struct GMAngle_t
{
    float yaw;
    float pitch;
}GMAngle_t;

extern GMAngle_t aimProcess(float yaw, float pit, int16_t *tic);

#ifdef __cplusplus
}
#endif
#endif
