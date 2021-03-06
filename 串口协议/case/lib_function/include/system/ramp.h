#ifndef _RAMP__H_
#define _RAMP__H_

#ifdef __cplusplus
extern "C" {
#endif
#include "sys.h"

typedef struct RampGen_t{
    int32_t count;
    int32_t XSCALE;
    float out;
    void (*Init)(struct RampGen_t *ramp, int32_t XSCALE);
    float (*Calc)(struct RampGen_t *ramp);
    void (*SetCounter)(struct RampGen_t *ramp, int32_t count);
    void (*ResetCounter)(struct RampGen_t *ramp);
    void (*SetScale)(struct RampGen_t *ramp, int32_t scale);
    uint8_t (*IsOverflow)(struct RampGen_t *ramp);
}RampGen_t;

#define RAMP_GEN_DAFAULT \
{ \
    .count = 0, \
    .XSCALE = 0, \
    .out = 0, \
    .Init = &RampInit, \
    .Calc = &RampCalc, \
    .SetCounter = &RampSetCounter, \
    .ResetCounter = &RampResetCounter, \
    .SetScale = &RampSetScale, \
    .IsOverflow = &RampIsOverflow, \
} \

uint8_t RampIsOverflow(struct RampGen_t *ramp);
float RampCalc(RampGen_t *ramp);
void RampInit(RampGen_t *ramp, int32_t XSCALE);
void RampSetCounter(struct RampGen_t *ramp, int32_t count);
void RampResetCounter(struct RampGen_t *ramp);
void RampSetScale(struct RampGen_t *ramp, int32_t scale);


#ifdef __cplusplus
}
#endif
#endif
