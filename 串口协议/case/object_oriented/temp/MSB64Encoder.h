#ifndef __BASE64_ENCODE_H__
#define __BASE64_ENCODE_H__

#include <stdint.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
char*
base64_encode(const uint8_t* text, int sz, int* encode_sz);

void
base64_encode_free(char* result);

#endif

