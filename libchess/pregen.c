/*
pregen.h/pregen.cpp - Source Code for ElephantEye, Part II

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
Copyright (C) 2004-2007 www.elephantbase.net
Copyright (c) 2025 yetist <yetist@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>
#include <stdbool.h>
#include "pregen.h"

extern const bool cbcInBoard[256];    // 棋盘区域表
extern const bool cbcInFort[256];     // 城池区域表
extern const bool cbcCanPromote[256]; // 升变区域表
extern const int8_t ccLegalSpanTab[512];   // 合理着法跨度表
extern const int8_t ccKnightPinTab[512];   // 马腿表

bool IN_BOARD(int sq) {
  return cbcInBoard[sq];
}

bool IN_FORT(int sq) {
  return cbcInFort[sq];
}

bool CAN_PROMOTE(int sq) {
  return cbcCanPromote[sq];
}

int8_t LEGAL_SPAN_TAB(int nDisp) {
  return ccLegalSpanTab[nDisp];
}

int8_t KNIGHT_PIN_TAB(int nDisp) {
  return ccKnightPinTab[nDisp];
}

int RANK_Y(int sq) {
  return sq >> 4;
}

int FILE_X(int sq) {
  return sq & 15;
}

int COORD_XY(int x, int y) {
  return x + (y << 4);
}

int SQUARE_FLIP(int sq) {
  return 254 - sq;
}

int FILE_FLIP(int x) {
  return 14 - x;
}

int RANK_FLIP(int y) {
  return 15 - y;
}

int OPP_SIDE(int sd) {
  return 1 - sd;
}

int SQUARE_FORWARD(int sq, int sd) {
  return sq - 16 + (sd << 5);
}

int SQUARE_BACKWARD(int sq, int sd) {
  return sq + 16 - (sd << 5);
}

bool KING_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 1;
}

bool ADVISOR_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 2;
}

bool BISHOP_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 3;
}

int BISHOP_PIN(int sqSrc, int sqDst) {
  return (sqSrc + sqDst) >> 1;
}

int KNIGHT_PIN(int sqSrc, int sqDst) {
  return sqSrc + KNIGHT_PIN_TAB(sqDst - sqSrc + 256);
}

bool WHITE_HALF(int sq) {
  return (sq & 0x80) != 0;
}

bool BLACK_HALF(int sq) {
  return (sq & 0x80) == 0;
}

bool HOME_HALF(int sq, int sd) {
  return (sq & 0x80) != (sd << 7);
}

bool AWAY_HALF(int sq, int sd) {
  return (sq & 0x80) == (sd << 7);
}

bool SAME_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

bool DIFF_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) != 0;
}

int RANK_DISP(int y) {
  return y << 4;
}

int FILE_DISP(int x) {
  return x;
}


void InitZero(Zobrist *zobr)
{
    zobr->dwKey = 0;
    zobr->dwLock0 = 0;
    zobr->dwLock1 = 0;
}

void InitRC4(Zobrist *zobr, RC4Struct *rc4)
{
    zobr->dwKey = RC4_NextLong(rc4);
    zobr->dwLock0 = RC4_NextLong(rc4);
    zobr->dwLock1 = RC4_NextLong(rc4);
}

void Xor1(Zobrist *zobr, const Zobrist zobr2)
{
    zobr->dwKey ^= zobr2.dwKey;
    zobr->dwLock0 ^= zobr2.dwLock0;
    zobr->dwLock1 ^= zobr2.dwLock1;
}

void Xor2(Zobrist *zobr, const Zobrist zobr1, const Zobrist zobr2)
{
    zobr->dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
    zobr->dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
    zobr->dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
}


const bool cbcInBoard[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const bool cbcInFort[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const bool cbcCanPromote[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const int8_t ccLegalSpanTab[512] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};

const int8_t ccKnightPinTab[512] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -16, 0, -16, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 16, 0, 16, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};

PreGen preGen;
PreEval preEval;

// 这四个数组用来判断棋子的走子方向，以马为例就是：sqDst = sqSrc + cnKnightMoveTab[i]
static const int cnKingMoveTab[4] = { -0x10, -0x01, +0x01, +0x10 };
static const int cnAdvisorMoveTab[4] = { -0x11, -0x0f, +0x0f, +0x11 };
static const int cnBishopMoveTab[4] = { -0x22, -0x1e, +0x1e, +0x22 };
static const int cnKnightMoveTab[8] = { -0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21 };

void
PreGenInit (void)
{
    int i, j, k, n, sqSrc, sqDst;
    RC4Struct rc4;
    SlideMove smv;
    SlideMask sms;

    // 首先初始化Zobrist键值表
    RC4_InitZero (&rc4);
    //PreGen.zobrPlayer.InitRC4 (rc4);
    InitRC4(&preGen.zobrPlayer, &rc4);
    for (i = 0; i < 14; i++)
    {
        for (j = 0; j < 256; j++)
        {
            //preGen.zobrTable[i][j].InitRC4 (rc4);
            Zobrist zobr;
            zobr = preGen.zobrTable[i][j];
            InitRC4(&zobr, &rc4);
        }
    }

    // 然后初始化屏蔽位行和屏蔽位列
    // 注：位行和位列不包括棋盘以外的位，所以就会频繁使用"+/- RANK_TOP/FILE_LEFT"
    for (sqSrc = 0; sqSrc < 256; sqSrc++)
    {
        if (IN_BOARD (sqSrc))
        {
            preGen.wBitRankMask[sqSrc] = 1 << (FILE_X (sqSrc) - FILE_LEFT);
            preGen.wBitFileMask[sqSrc] = 1 << (RANK_Y (sqSrc) - RANK_TOP);
        } else {
            preGen.wBitRankMask[sqSrc] = 0;
            preGen.wBitFileMask[sqSrc] = 0;
        }
    }

    // 然后生成车炮横向的预置数组(数组的应用参阅"pregen.h")
    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 512; j++)
        {
            // 初始化借助于“位行”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"SlideMoveTab"，假设没有着法，用起始格填充
            smv.ucNonCap[0] = smv.ucNonCap[1] = smv.ucRookCap[0] = smv.ucRookCap[1] =
                smv.ucCannonCap[0] = smv.ucCannonCap[1] = smv.ucSuperCap[0] = smv.ucSuperCap[1] = i + FILE_LEFT;
            sms.wNonCap = sms.wRookCap = sms.wCannonCap = sms.wSuperCap = 0;
            // 提示：参阅"pregen.h"，...[0]表示最大一格，向右移动和下移动都用[0]，反之亦然
            // 2. 考虑向右移动的目标格，填充...[0]，
            for (k = i + 1; k <= 8; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucRookCap[0] = FILE_DISP (k + FILE_LEFT);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[0] = FILE_DISP (k + FILE_LEFT);
                sms.wNonCap |= 1 << k;
            }
            for (k++; k <= 8; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucCannonCap[0] = FILE_DISP (k + FILE_LEFT);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k++; k <= 8; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucSuperCap[0] = FILE_DISP (k + FILE_LEFT);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 3. 考虑向左移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucRookCap[1] = FILE_DISP (k + FILE_LEFT);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[1] = FILE_DISP (k + FILE_LEFT);
                sms.wNonCap |= 1 << k;
            }
            for (k--; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucCannonCap[1] = FILE_DISP (k + FILE_LEFT);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k--; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucSuperCap[1] = FILE_DISP (k + FILE_LEFT);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 4. 为"smv"和"sms"的值作断言
            __ASSERT_BOUND_2 (3, smv.ucNonCap[1], smv.ucNonCap[0], 11);
            __ASSERT_BOUND_2 (3, smv.ucRookCap[1], smv.ucRookCap[0], 11);
            __ASSERT_BOUND_2 (3, smv.ucCannonCap[1], smv.ucCannonCap[0], 11);
            __ASSERT_BOUND_2 (3, smv.ucSuperCap[1], smv.ucSuperCap[0], 11);
            __ASSERT_BITRANK (sms.wNonCap);
            __ASSERT_BITRANK (sms.wRookCap);
            __ASSERT_BITRANK (sms.wCannonCap);
            __ASSERT_BITRANK (sms.wSuperCap);
            // 5. 将临时变量"smv"和"sms"拷贝到着法预生成数组中
            preGen.smvRankMoveTab[i][j] = smv;
            preGen.smsRankMaskTab[i][j] = sms;
        }
    }

    // 然后生成车炮纵向的预置数组(数组的应用参阅"pregen.h")
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 1024; j++)
        {
            // 初始化借助于“位列”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"smv"，假设没有着法，用起始格填充
            smv.ucNonCap[0] = smv.ucNonCap[1] = smv.ucRookCap[0] = smv.ucRookCap[1] =
                smv.ucCannonCap[0] = smv.ucCannonCap[1] = smv.ucSuperCap[0] = smv.ucSuperCap[1] = (i + RANK_TOP) * 16;
            sms.wNonCap = sms.wRookCap = sms.wCannonCap = sms.wSuperCap = 0;
            // 2. 考虑向下移动的目标格，填充...[0]
            for (k = i + 1; k <= 9; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucRookCap[0] = RANK_DISP (k + RANK_TOP);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[0] = RANK_DISP (k + RANK_TOP);
                sms.wNonCap |= 1 << k;
            }
            for (k++; k <= 9; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucCannonCap[0] = RANK_DISP (k + RANK_TOP);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k++; k <= 9; k++)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucSuperCap[0] = RANK_DISP (k + RANK_TOP);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 3. 考虑向上移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucRookCap[1] = RANK_DISP (k + RANK_TOP);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[1] = RANK_DISP (k + RANK_TOP);
                sms.wNonCap |= 1 << k;
            }
            for (k--; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucCannonCap[1] = RANK_DISP (k + RANK_TOP);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k--; k >= 0; k--)
            {
                if ((j & (1 << k)) != 0)
                {
                    smv.ucSuperCap[1] = RANK_DISP (k + RANK_TOP);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 4. 为"smv"和"sms"的值作断言
            __ASSERT_BOUND_2 (3, smv.ucNonCap[1] >> 4, smv.ucNonCap[0] >> 4, 12);
            __ASSERT_BOUND_2 (3, smv.ucRookCap[1] >> 4, smv.ucRookCap[0] >> 4, 12);
            __ASSERT_BOUND_2 (3, smv.ucCannonCap[1] >> 4, smv.ucCannonCap[0] >> 4, 12);
            __ASSERT_BOUND_2 (3, smv.ucSuperCap[1] >> 4, smv.ucSuperCap[0] >> 4, 12);
            __ASSERT_BITFILE (sms.wNonCap);
            __ASSERT_BITFILE (sms.wRookCap);
            __ASSERT_BITFILE (sms.wCannonCap);
            __ASSERT_BITFILE (sms.wSuperCap);
            // 5. 将临时变量"smv"和"sms"拷贝到着法预生成数组中
            preGen.smvFileMoveTab[i][j] = smv;
            preGen.smsFileMaskTab[i][j] = sms;
        }
    }

    // 接下来生成着法预生成数组，连同将军预判数组
    for (sqSrc = 0; sqSrc < 256; sqSrc++)
    {
        if (IN_BOARD (sqSrc))
        {
            // 生成帅(将)的着法预生成数组
            n = 0;
            for (i = 0; i < 4; i++)
            {
                sqDst = sqSrc + cnKingMoveTab[i];
                if (IN_FORT (sqDst))
                {
                    preGen.ucsqKingMoves[sqSrc][n] = sqDst;
                    n++;
                }
            }
            __ASSERT (n <= 4);
            preGen.ucsqKingMoves[sqSrc][n] = 0;
            // 生成仕(士)的着法预生成数组
            n = 0;
            for (i = 0; i < 4; i++)
            {
                sqDst = sqSrc + cnAdvisorMoveTab[i];
                if (IN_FORT (sqDst))
                {
                    preGen.ucsqAdvisorMoves[sqSrc][n] = sqDst;
                    n++;
                }
            }
            __ASSERT (n <= 4);
            preGen.ucsqAdvisorMoves[sqSrc][n] = 0;
            // 生成相(象)的着法预生成数组，包括象眼数组
            n = 0;
            for (i = 0; i < 4; i++)
            {
                sqDst = sqSrc + cnBishopMoveTab[i];
                if (IN_BOARD (sqDst) && SAME_HALF (sqSrc, sqDst))
                {
                    preGen.ucsqBishopMoves[sqSrc][n] = sqDst;
                    preGen.ucsqBishopPins[sqSrc][n] = BISHOP_PIN (sqSrc, sqDst);
                    n++;
                }
            }
            __ASSERT (n <= 4);
            preGen.ucsqBishopMoves[sqSrc][n] = 0;
            // 生成马的着法预生成数组，包括马腿数组
            n = 0;
            for (i = 0; i < 8; i++)
            {
                sqDst = sqSrc + cnKnightMoveTab[i];
                if (IN_BOARD (sqDst))
                {
                    preGen.ucsqKnightMoves[sqSrc][n] = sqDst;
                    preGen.ucsqKnightPins[sqSrc][n] = KNIGHT_PIN (sqSrc, sqDst);
                    n++;
                }
            }
            __ASSERT (n <= 8);
            preGen.ucsqKnightMoves[sqSrc][n] = 0;
            // 生成兵(卒)的着法预生成数组
            for (i = 0; i < 2; i++)
            {
                n = 0;
                sqDst = SQUARE_FORWARD (sqSrc, i);
                sqDst = sqSrc + (i == 0 ? -16 : 16);
                if (IN_BOARD (sqDst))
                {
                    preGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
                    n++;
                }
                if (AWAY_HALF (sqSrc, i))
                {
                    for (j = -1; j <= 1; j += 2)
                    {
                        sqDst = sqSrc + j;
                        if (IN_BOARD (sqDst))
                        {
                            preGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
                            n++;
                        }
                    }
                }
                __ASSERT (n <= 3);
                preGen.ucsqPawnMoves[i][sqSrc][n] = 0;
            }
        }
    }

    // 最后清空局面预评价结构
    memset (&preEval, 0, sizeof (PreEval));
    preEval.bPromotion = false; // 缺省是不允许升变的
}
