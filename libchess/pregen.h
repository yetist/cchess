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

#include <assert.h>
#include "rc4prng.h"

#ifndef PREGEN_H
#define PREGEN_H

#define __ASSERT(a) assert(a)
#define __ASSERT_PIECE(pc) assert((pc) >= 16 && (pc) <= 47)
#define __ASSERT_SQUARE(sq) assert(IN_BOARD(sq))
#define __ASSERT_BITRANK(w) assert((w) < (1 << 9))
#define __ASSERT_BITFILE(w) assert((w) < (1 << 10))
#define __ASSERT_BOUND_2(a, b, c, d) assert((a) <= (b) && (b) <= (c) && (c) <= (d))

#define RANK_TOP 3
#define RANK_BOTTOM 12
#define FILE_LEFT 3
#define FILE_CENTER 7
#define FILE_RIGHT 11

bool IN_BOARD(int sq);
bool IN_FORT(int sq);
bool CAN_PROMOTE(int sq);
int8_t LEGAL_SPAN_TAB(int nDisp);
int8_t KNIGHT_PIN_TAB(int nDisp);
int RANK_Y(int sq);
int FILE_X(int sq);
int COORD_XY(int x, int y);
int SQUARE_FLIP(int sq);
int FILE_FLIP(int x);
int RANK_FLIP(int y);
int OPP_SIDE(int sd);
int SQUARE_FORWARD(int sq, int sd);
int SQUARE_BACKWARD(int sq, int sd);
bool KING_SPAN(int sqSrc, int sqDst);
bool ADVISOR_SPAN(int sqSrc, int sqDst);
bool BISHOP_SPAN(int sqSrc, int sqDst);
int BISHOP_PIN(int sqSrc, int sqDst);
int KNIGHT_PIN(int sqSrc, int sqDst);
bool WHITE_HALF(int sq);
bool BLACK_HALF(int sq);
bool HOME_HALF(int sq, int sd);
bool AWAY_HALF(int sq, int sd);
bool SAME_HALF(int sqSrc, int sqDst);
bool DIFF_HALF(int sqSrc, int sqDst);
int RANK_DISP(int y);
int FILE_DISP(int x);
void PreGenInit(void);

// 借助“位行”和“位列”生成车炮着法的预置结构
typedef struct _SlideMove SlideMove;
// 借助“位行”和“位列”判断车炮着法合理性的预置结构
typedef struct _SlideMask SlideMask;
typedef struct _Zobrist Zobrist;
typedef struct _PreGen PreGen;
// 局面预评价结构
typedef struct _PreEval PreEval;

struct _SlideMove {
  uint8_t ucNonCap[2];    // 不吃子能走到的最大一格/最小一格
  uint8_t ucRookCap[2];   // 车吃子能走到的最大一格/最小一格
  uint8_t ucCannonCap[2]; // 炮吃子能走到的最大一格/最小一格
  uint8_t ucSuperCap[2];  // 超级炮(隔两子吃子)能走到的最大一格/最小一格
}; // smv

struct _SlideMask {
  uint16_t wNonCap, wRookCap, wCannonCap, wSuperCap;
}; // sms


struct _Zobrist {
  uint32_t dwKey;
  uint32_t dwLock0;
  uint32_t dwLock1;
};

void InitZero(Zobrist *zobr);
void InitRC4(Zobrist *zobr, RC4Struct *rc4);

void Xor1(Zobrist *zobr, const Zobrist zobr2);
void Xor2(Zobrist *zobr, const Zobrist zobr1, const Zobrist zobr2);

struct _PreGen {
  // Zobrist键值表，分Zobrist键值和Zobrist校验锁两部分
  Zobrist zobrPlayer;
  Zobrist zobrTable[14][256];

  uint16_t wBitRankMask[256]; // 每个格子的位行的屏蔽位
  uint16_t wBitFileMask[256]; // 每个格子的位列的屏蔽位

  /* 借助“位行”和“位列”生成车炮着法和判断车炮着法合理性的预置数组
   *
   * “位行”和“位列”技术是ElephantEye的核心技术，用来处理车和炮的着法生成、将军判断和局面分析。
   * 以初始局面红方右边的炮在该列的行动为例，首先必须知道该列的“位列”，即"1010000101b"，
   * ElephantEye有两种预置数组，即"...MoveTab"和"...MaskTab"，用法分别是：
   * 一、如果要知道该子向前吃子的目标格(起始格是2，目标格是9)，那么希望查表就能知道这个格子，
   * 　　预先生成一个数组"FileMoveTab_CannonCap[10][1024]"，使得"FileMoveTab_CannonCap[2][1010000101b] == 9"就可以了。
   * 二、如果要判断该子能否吃到目标格(同样以起始格是2，目标格是9为例)，那么需要知道目标格的位列，即"0000000001b"，
   * 　　只要把"...MoveTab"的格子以“屏蔽位”的形式重新记作数组"...MaskTab"就可以了，用“与”操作来判断能否吃到目标格，
   * 　　通常一个"...MaskTab"单元会包括多个屏蔽位，判断能否吃到同行或同列的某个格子时，只需要做一次判断就可以了。
   */
  SlideMove smvRankMoveTab[9][512];   // 36,864 字节
  SlideMove smvFileMoveTab[10][1024]; // 81,920 字节
  SlideMask smsRankMaskTab[9][512];   // 36,864 字节
  SlideMask smsFileMaskTab[10][1024]; // 81,920 字节

  /* 其余棋子(不适合用“位行”和“位列”)的着法预生成数组
   *
   * 这部分数组是真正意义上的“着法预生成”数组，可以根据某个棋子的起始格直接查数组，得到所有的目标格。
   * 使用数组时，可以根据起始格来确定一个指针"g_...Moves[Square]"，这个指针指向一系列目标格，以0结束。
   * 为了对齐地址，数组[256][n]中n总是4的倍数，而且必须大于n(因为数组包括了结束标识符0)，除了象眼和马腿数组。
   */
  uint8_t ucsqKingMoves[256][8];
  uint8_t ucsqAdvisorMoves[256][8];
  uint8_t ucsqBishopMoves[256][8];
  uint8_t ucsqBishopPins[256][4];
  uint8_t ucsqKnightMoves[256][12];
  uint8_t ucsqKnightPins[256][8];
  uint8_t ucsqPawnMoves[2][256][4];
};

struct _PreEval {
  bool bPromotion;
  int vlAdvanced;
  uint8_t ucvlWhitePieces[7][256];
  uint8_t ucvlBlackPieces[7][256];
};

#endif
