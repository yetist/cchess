/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */
#include <sys/time.h>
#include "rc4prng.h"

static void swap(uint8_t a, uint8_t b);
static int64_t GetTime();

static void swap(uint8_t a, uint8_t b)
{
  uint8_t temp;
  temp = a;
  a = b;
  b = temp;
}

// 返回毫秒单位
static int64_t GetTime()
{
  struct timeval tv;
  struct timezone tz;

  gettimeofday (&tv , &tz);

  return tv.tv_sec * 1000 + (tv.tv_usec +500)/1000;
}

void RC4_Init(RC4Struct *rc4, void *lpKey, int nKeyLen)
{
    int i, j;
    rc4->x = rc4->y = j = 0;
    for (i = 0; i < 256; i ++) {
      rc4->s[i] = i;
    }
    for (i = 0; i < 256; i ++) {
      j = (j + rc4->s[i] + ((uint8_t *) lpKey)[i % nKeyLen]) & 255;
      swap(rc4->s[i], rc4->s[j]);
    }
}

void RC4_InitZero(RC4Struct *rc4)
{
  uint32_t dwKey;
  dwKey = 0;
  RC4_Init(rc4, &dwKey, 4);
}

void RC4_InitRand(RC4Struct *rc4)
{
    union {
      uint32_t dw[2];
      uint64_t qw;
    } Seed;
#if defined(__i386__) || defined(__x86_64__)
    Seed.qw = TimeStampCounter();
#else
    Seed.qw = 0;
#endif
    Seed.dw[1] ^= (uint32_t) GetTime();
    RC4_Init(rc4, &Seed, 8);
}

uint8_t RC4_NextByte(RC4Struct *rc4)
{
    rc4->x = (rc4->x + 1) & 255;
    rc4->y = (rc4->y + rc4->s[rc4->x]) & 255;
    swap(rc4->s[rc4->x], rc4->s[rc4->y]);
    return rc4->s[(rc4->s[rc4->x] + rc4->s[rc4->y]) & 255];
}

uint32_t RC4_NextLong(RC4Struct *rc4)
{
    union {
      uint8_t uc[4];
      uint32_t dw;
    } Ret;
    Ret.uc[0] = RC4_NextByte(rc4);
    Ret.uc[1] = RC4_NextByte(rc4);
    Ret.uc[2] = RC4_NextByte(rc4);
    Ret.uc[3] = RC4_NextByte(rc4);
    return Ret.dw;
}
