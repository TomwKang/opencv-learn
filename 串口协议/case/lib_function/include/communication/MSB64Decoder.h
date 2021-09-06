#ifndef __BASE64_DECODE_H__
#define __BASE64_DECODE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "stdio.h"
static int
b64index(uint8_t c);
//char* base64_decode(const uint8_t* text, size_t sz, int* out_size);
char* base64_decode(const uint8_t* text, size_t sz, int* out_size);

void base64_decode_free(char* result);
#ifdef __cplusplus
}
#endif
#endif
