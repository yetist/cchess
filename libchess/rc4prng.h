/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */
#include <stdint.h>

#ifndef RC4PRNG_H
#define RC4PRNG_H

typedef struct _RC4Struct RC4Struct;

struct _RC4Struct {
  uint8_t s[256];
  int x, y;
};

void     RC4_Init     (RC4Struct *rc4, void *lpKey, int nKeyLen);
void     RC4_InitZero (RC4Struct *rc4);
void     RC4_InitRand (RC4Struct *rc4);
uint8_t  RC4_NextByte (RC4Struct *rc4);
uint32_t RC4_NextLong (RC4Struct *rc4);

#endif
