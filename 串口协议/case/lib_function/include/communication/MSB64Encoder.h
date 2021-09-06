#ifndef __BASE64_ENCODE_H__
#define __BASE64_ENCODE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
char* base64_encode(const uint8_t* text, int sz, int* out_esz);
//void base64_encode(const uint8_t* text, int sz, int* out_esz,char* send_send);
void base64_encode_free(char* result);
void	base64_encode_free_buffer();
#ifdef __cplusplus
}
#endif
#endif

