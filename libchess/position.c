/*
position.h/position.cpp - Source Code for ElephantEye, Part III

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
#include "position.h"

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define __ASSERT_BOUND(a, b, c) assert((a) <= (b) && (b) <= (c))
#define __ASSERT(a) assert(a)

extern PreGen preGen;
extern PreEval preEval;

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255)
 * pc: 棋子序号(整数，从0到47)
 * pt: 棋子类型序号(整数，从0到6)
 * mv: 着法(整数，从0到65535)
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE")
 * (注：以上四个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(Position类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 棋子类型对应的棋子符号
const char* cszPieceBytes = "KABNRCP";

/* 棋子序号对应的棋子类型
 *
 * ElephantEye的棋子序号从0到47，其中0到15不用，16到31表示红子，32到47表示黑子。
 * 每方的棋子顺序依次是：帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 提示：判断棋子是红子用"pc < 32"，黑子用"pc >= 32"
 */
const int cnPieceTypes[48] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
    0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};


// 棋子的简单分值，只在简单比较时作参考
const int cnSimpleValues[48] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    5,
    1,
    1,
    1,
    1,
    3,
    3,
    4,
    4,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
    5,
    1,
    1,
    1,
    1,
    3,
    3,
    4,
    4,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
};

// 该数组很方便地实现了坐标的镜像(左右对称)
const uint8_t cucsqMirrorTab[256] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x3b,
    0x3a,
    0x39,
    0x38,
    0x37,
    0x36,
    0x35,
    0x34,
    0x33,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x4b,
    0x4a,
    0x49,
    0x48,
    0x47,
    0x46,
    0x45,
    0x44,
    0x43,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x5b,
    0x5a,
    0x59,
    0x58,
    0x57,
    0x56,
    0x55,
    0x54,
    0x53,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x6b,
    0x6a,
    0x69,
    0x68,
    0x67,
    0x66,
    0x65,
    0x64,
    0x63,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x7b,
    0x7a,
    0x79,
    0x78,
    0x77,
    0x76,
    0x75,
    0x74,
    0x73,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x8b,
    0x8a,
    0x89,
    0x88,
    0x87,
    0x86,
    0x85,
    0x84,
    0x83,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x9b,
    0x9a,
    0x99,
    0x98,
    0x97,
    0x96,
    0x95,
    0x94,
    0x93,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xab,
    0xaa,
    0xa9,
    0xa8,
    0xa7,
    0xa6,
    0xa5,
    0xa4,
    0xa3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xbb,
    0xba,
    0xb9,
    0xb8,
    0xb7,
    0xb6,
    0xb5,
    0xb4,
    0xb3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xcb,
    0xca,
    0xc9,
    0xc8,
    0xc7,
    0xc6,
    0xc5,
    0xc4,
    0xc3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

void SetPerpCheck (uint32_t *dwPerpCheck, int nChkChs);
// FEN串中棋子标识，注意这个函数只能识别大写字母，因此用小写字母时，首先必须转换为大写
int
FenPiece (int nArg)
{
    switch (nArg)
    {
    case 'K':
        return 0;
    case 'A':
        return 1;
    case 'B':
    case 'E':
        return 2;
    case 'N':
    case 'H':
        return 3;
    case 'R':
        return 4;
    case 'C':
        return 5;
    case 'P':
        return 6;
    default:
        return 7;
    }
}

static uint32_t BIT_PIECE(int pc) {
  return 1 << (pc - 16);
}

uint32_t WHITE_BITPIECE(int nBitPiece) {
  return nBitPiece;
}

uint32_t BLACK_BITPIECE(int nBitPiece) {
  return nBitPiece << 16;
}

static uint32_t BOTH_BITPIECE(int nBitPiece) {
  return nBitPiece + (nBitPiece << 16);
}

///* ElephantEye的很多代码中都用到"SIDE_TAG()"这个量，红方设为16，黑方设为32。
// * 用"SIDE_TAG() + i"可以方便地选择棋子的类型，"i"从0到15依次是：
// * 帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
// * 例如"i"取"KNIGHT_FROM"到"KNIGHT_TO"，则表示依次检查两个马的位置
// */
int SIDE_TAG(int sd) {
  return 16 + (sd << 4);
}

static int OPP_SIDE_TAG(int sd) {
  return 32 - (sd << 4);
}

static int SIDE_VALUE(int sd, int vl) {
  return (sd == 0 ? vl : -vl);
}

static int PIECE_INDEX(int pc) {
  return pc & 15;
}

char PIECE_BYTE(int pt) {
  return cszPieceBytes[pt];
}

int PIECE_TYPE(int pc) {
  return cnPieceTypes[pc];
}

static int SIMPLE_VALUE(int pc) {
  return cnSimpleValues[pc];
}

static uint8_t SQUARE_MIRROR(int sq) {
  return cucsqMirrorTab[sq];
}

// 着法结构
int SRC(int mv) { // 得到着法的起点
  return mv & 255;
}

int DST(int mv) { // 得到着法的终点
  return mv >> 8;
}

int MOVE(int sqSrc, int sqDst) {   // 由起点和终点得到着法
  return sqSrc + (sqDst << 8);
}

uint32_t MOVE_COORD(int mv) {      // 把着法转换成字符串
  union {
    char c[4];
    uint32_t dw;
  } Ret;
  Ret.c[0] = FILE_X(SRC(mv)) - FILE_LEFT + 'a';
  Ret.c[1] = '9' - RANK_Y(SRC(mv)) + RANK_TOP;
  Ret.c[2] = FILE_X(DST(mv)) - FILE_LEFT + 'a';
  Ret.c[3] = '9' - RANK_Y(DST(mv)) + RANK_TOP;
  // 断言输出着法的合理性
  __ASSERT_BOUND('a', Ret.c[0], 'i');
  __ASSERT_BOUND('0', Ret.c[1], '9');
  __ASSERT_BOUND('a', Ret.c[2], 'i');
  __ASSERT_BOUND('0', Ret.c[3], '9');
  return Ret.dw;
}

int COORD_MOVE(uint32_t dwMoveStr) { // 把字符串转换成着法
  int sqSrc, sqDst;
  char *lpArgPtr;
  lpArgPtr = (char *) &dwMoveStr;
  sqSrc = COORD_XY(lpArgPtr[0] - 'a' + FILE_LEFT, '9' - lpArgPtr[1] + RANK_TOP);
  sqDst = COORD_XY(lpArgPtr[2] - 'a' + FILE_LEFT, '9' - lpArgPtr[3] + RANK_TOP);
  // 对输入着法的合理性不作断言
  // __ASSERT_SQUARE(sqSrc);
  // __ASSERT_SQUARE(sqDst);
  return (IN_BOARD(sqSrc) && IN_BOARD(sqDst) ? MOVE(sqSrc, sqDst) : 0);
}

static int MOVE_MIRROR(int mv) {          // 对着法做镜像
  return MOVE(SQUARE_MIRROR(SRC(mv)), SQUARE_MIRROR(DST(mv)));
}


// 获取着法预生成信息
SlideMove *Position_RankMovePtr(Position* pos, int x, int y)
{
    return preGen.smvRankMoveTab[x - FILE_LEFT] + pos->wBitRanks[y];
}

SlideMove *Position_FileMovePtr(Position* pos, int x, int y)
{
    return preGen.smvFileMoveTab[y - RANK_TOP] + pos->wBitFiles[x];
}

SlideMask *Position_RankMaskPtr(Position* pos, int x, int y)
{
    return preGen.smsRankMaskTab[x - FILE_LEFT] + pos->wBitRanks[y];
}

SlideMask *Position_FileMaskPtr(Position* pos, int x, int y)
{
    return preGen.smsFileMaskTab[y - RANK_TOP] + pos->wBitFiles[x];
}

// 棋盘处理过程
void Position_ClearBoard(Position* pos)
{ // 棋盘初始化
    pos->sdPlayer = 0;
    memset(pos->ucpcSquares, 0, 256);
    memset(pos->ucsqPieces, 0, 48);
    InitZero(&(pos->zobr));
    pos->dwBitPiece = 0;
    memset(pos->wBitRanks, 0, 16 * sizeof(uint16_t));
    memset(pos->wBitFiles, 0, 16 * sizeof(uint16_t));
    pos->vlWhite = pos->vlBlack = 0;
    // "ClearBoard()"后面紧跟的是"SetIrrev()"，来初始化其它成员
}

void Position_ChangeSide(Position* pos)
{ // 交换走棋方
    pos->sdPlayer = OPP_SIDE(pos->sdPlayer);
    Xor1(&pos->zobr, preGen.zobrPlayer);
}

void Position_SaveStatus(Position* pos)
{ // 保存状态
    Rollback *lprbs;
    lprbs = pos->rbsList + pos->nMoveNum;
    lprbs->zobr = pos->zobr;
    lprbs->vlWhite = pos->vlWhite;
    lprbs->vlBlack = pos->vlBlack;
}

void Position_Rollback(Position* pos)
{   // 回滚
    Rollback *lprbs;
    lprbs = pos->rbsList + pos->nMoveNum;
    pos->zobr = lprbs->zobr;
    pos->vlWhite = lprbs->vlWhite;
    pos->vlBlack = lprbs->vlBlack;
}

//////////////////////////////

void Position_SetIrrev(Position* pos)
{    // 把局面设成“不可逆”，即清除回滚着法
    pos->rbsList[0].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
    pos->rbsList[0].mvs.ChkChs = Position_CheckedBy(pos, false);
    pos->nMoveNum = 1;
    pos->nDistance = 0;
    memset(pos->ucRepHash, 0, REP_HASH_MASK + 1);
  }

  // 着法检测过程
bool Position_GoodCap(Position* pos, int mv)
{     // 好的吃子着法检测，这样的着法不记录到历史表和杀手着法表中
    int pcMoved, pcCaptured;
    pcCaptured = pos->ucpcSquares[DST(mv)];
    if (pcCaptured == 0) {
      return false;
    }
    if (!Position_Protected(pos, OPP_SIDE(pos->sdPlayer), DST(mv), 0)) {
      return true;
    }
    pcMoved = pos->ucpcSquares[SRC(mv)];
    return SIMPLE_VALUE(pcCaptured) > SIMPLE_VALUE(pcMoved);
}
                                           //
Move Position_LastMove(Position* pos)
{        // 前一步着法，该着法保存了局面的将军状态
    return pos->rbsList[pos->nMoveNum - 1].mvs;
}

bool Position_CanPromote(Position* pos)
{            // 判断是否能升变
    return (pos->wBitPiece[pos->sdPlayer] & PAWN_BITPIECE) != PAWN_BITPIECE && Position_LastMove(pos).ChkChs <= 0;
}

bool Position_NullOkay(Position* pos)
{              // 允许使用空着裁剪的条件
    return (pos->sdPlayer == 0 ? pos->vlWhite : pos->vlBlack) > NULLOKAY_MARGIN;
}

bool Position_NullSafe(Position* pos)
{              // 空着裁剪可以不检验的条件
    return (pos->sdPlayer == 0 ? pos->vlWhite : pos->vlBlack) > NULLSAFE_MARGIN;
}

bool Position_IsDraw(Position* pos)
{                // 和棋判断
    return (!preEval.bPromotion && (pos->dwBitPiece & BOTH_BITPIECE(ATTACK_BITPIECE)) == 0) ||
        -Position_LastMove(pos).CptDrw >= DRAW_MOVES || pos->nMoveNum == MAX_MOVE_NUM;
}

int Position_DrawValue(Position* pos)
{              // 和棋的分值
    return (pos->nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
}

int Position_RepValue(Position* pos, int vlRep)
{          // 重复局面的分值
    // return vlRep == REP_LOSS ? nDistance - MATE_VALUE : vlRep == REP_WIN ? MATE_VALUE - nDistance : DrawValue();
    // 长将判负的分值，低于BAN_VALUE将不写入置换表(参阅"hash.cpp")
    return vlRep == REP_LOSS ? pos->nDistance - BAN_VALUE : vlRep == REP_WIN ? BAN_VALUE - pos->nDistance : Position_DrawValue(pos);
}

int Position_Material(Position* pos)
{               // 子力平衡，包括先行权因素
    return SIDE_VALUE(pos->sdPlayer, pos->vlWhite - pos->vlBlack) + preEval.vlAdvanced;
}
                                                             //
int Position_GenAllMoves(Position* pos, Move *lpmvs)
{                 // 全部着法生成器
    int nCapNum;
    nCapNum = Position_GenCapMoves(pos, lpmvs);
    return nCapNum + Position_GenNonCapMoves(pos, lpmvs + nCapNum);
}


// 以下是一些棋盘处理过程

// 棋盘上增加棋子
void
Position_AddPiece (Position* pos, int sq, int pc, bool bDel)
{
    int pt;

    __ASSERT_SQUARE (sq);
    __ASSERT_PIECE (pc);
    if (bDel)
    {
        pos->ucpcSquares[sq] = 0;
        pos->ucsqPieces[pc] = 0;
    } else {
        pos->ucpcSquares[sq] = pc;
        pos->ucsqPieces[pc] = sq;
    }
    pos->wBitRanks[RANK_Y (sq)] ^= preGen.wBitRankMask[sq];
    pos->wBitFiles[FILE_X (sq)] ^= preGen.wBitFileMask[sq];
    __ASSERT_BITRANK (pos->wBitRanks[RANK_Y (sq)]);
    __ASSERT_BITFILE (pos->wBitRanks[FILE_X (sq)]);
    pos->dwBitPiece ^= BIT_PIECE (pc);
    pt = PIECE_TYPE (pc);
    if (pc < 32)
    {
        if (bDel)
        {
            pos->vlWhite -= preEval.ucvlWhitePieces[pt][sq];
        } else {
            pos->vlWhite += preEval.ucvlWhitePieces[pt][sq];
        }
    } else {
        if (bDel)
        {
            pos->vlBlack -= preEval.ucvlBlackPieces[pt][sq];
        } else {
            pos->vlBlack += preEval.ucvlBlackPieces[pt][sq];
        }
        pt += 7;
    }
    __ASSERT_BOUND (0, pt, 13);
    Xor1(&pos->zobr, preGen.zobrTable[pt][sq]);
}

// 移动棋子
int
Position_MovePiece (Position* pos, int mv)
{
    int sqSrc, sqDst, pcMoved, pcCaptured, pt;
    uint8_t *lpucvl;
    // 移动棋子包括以下几个步骤：

    // 1. 得到移动的棋子序号和被吃的棋子序号；
    sqSrc = SRC (mv);
    sqDst = DST (mv);
    pcMoved = pos->ucpcSquares[sqSrc];
    __ASSERT_SQUARE (sqSrc);
    __ASSERT_SQUARE (sqDst);
    __ASSERT_PIECE (pcMoved);
    pcCaptured = pos->ucpcSquares[sqDst];
    if (pcCaptured == 0)
    {

        // 2. 如果没有被吃的棋子，那么更新目标格的位行和位列。
        //    换句话说，有被吃的棋子，目标格的位行和位列就不必更新了。
        pos->wBitRanks[RANK_Y (sqDst)] ^= preGen.wBitRankMask[sqDst];
        pos->wBitFiles[FILE_X (sqDst)] ^= preGen.wBitFileMask[sqDst];
        __ASSERT_BITRANK (pos->wBitRanks[RANK_Y (sqDst)]);
        __ASSERT_BITFILE (pos->wBitRanks[FILE_X (sqDst)]);
    } else {

        __ASSERT_PIECE (pcCaptured);
        // 3. 如果有被吃的棋子，那么更新棋子位，从"ucsqPieces"数组中清除被吃棋子
        //    同时更新子力价值、位行位列、Zobrist键值和校验锁
        pos->ucsqPieces[pcCaptured] = 0;
        pos->dwBitPiece ^= BIT_PIECE (pcCaptured);
        pt = PIECE_TYPE (pcCaptured);
        if (pcCaptured < 32)
        {
            pos->vlWhite -= preEval.ucvlWhitePieces[pt][sqDst];
        } else {
            pos->vlBlack -= preEval.ucvlBlackPieces[pt][sqDst];
            pt += 7;
        }
        __ASSERT_BOUND (0, pt, 13);
        Xor1 (&pos->zobr, preGen.zobrTable[pt][sqDst]);
    }

    // 4. 从"ucpcSquares"和"ucsqPieces"数组中移动棋子，注意“格子-棋子联系数组”移动棋子的方法
    //    同时更新位行、位列、子力价值、位行位列、Zobrist键值和校验锁
    pos->ucpcSquares[sqSrc] = 0;
    pos->ucpcSquares[sqDst] = pcMoved;
    pos->ucsqPieces[pcMoved] = sqDst;
    pos->wBitRanks[RANK_Y (sqSrc)] ^= preGen.wBitRankMask[sqSrc];
    pos->wBitFiles[FILE_X (sqSrc)] ^= preGen.wBitFileMask[sqSrc];
    __ASSERT_BITRANK (pos->wBitRanks[RANK_Y (sqSrc)]);
    __ASSERT_BITFILE (pos->wBitRanks[FILE_X (sqSrc)]);
    pt = PIECE_TYPE (pcMoved);
    if (pcMoved < 32)
    {
        lpucvl = preEval.ucvlWhitePieces[pt];
        pos->vlWhite += lpucvl[sqDst] - lpucvl[sqSrc];
    } else {
        lpucvl = preEval.ucvlBlackPieces[pt];
        pos->vlBlack += lpucvl[sqDst] - lpucvl[sqSrc];
        pt += 7;
    }
    __ASSERT_BOUND (0, pt, 13);
    Xor2 (&pos->zobr, preGen.zobrTable[pt][sqDst], preGen.zobrTable[pt][sqSrc]);
    return pcCaptured;
}

// 撤消移动棋子
void
Position_UndoMovePiece (Position* pos, int mv, int pcCaptured)
{
    int sqSrc, sqDst, pcMoved;
    sqSrc = SRC (mv);
    sqDst = DST (mv);
    pcMoved = pos->ucpcSquares[sqDst];
    __ASSERT_SQUARE (sqSrc);
    __ASSERT_SQUARE (sqDst);
    __ASSERT_PIECE (pcMoved);
    pos->ucpcSquares[sqSrc] = pcMoved;
    pos->ucsqPieces[pcMoved] = sqSrc;
    pos->wBitRanks[RANK_Y (sqSrc)] ^= preGen.wBitRankMask[sqSrc];
    pos->wBitFiles[FILE_X (sqSrc)] ^= preGen.wBitFileMask[sqSrc];
    __ASSERT_BITRANK (pos->wBitRanks[RANK_Y (sqSrc)]);
    __ASSERT_BITFILE (pos->wBitRanks[FILE_X (sqSrc)]);
    if (pcCaptured > 0)
    {
        __ASSERT_PIECE (pcCaptured);
        pos->ucpcSquares[sqDst] = pcCaptured;
        pos->ucsqPieces[pcCaptured] = sqDst;
        pos->dwBitPiece ^= BIT_PIECE (pcCaptured);
    } else {
        pos->ucpcSquares[sqDst] = 0;
        pos->wBitRanks[RANK_Y (sqDst)] ^= preGen.wBitRankMask[sqDst];
        pos->wBitFiles[FILE_X (sqDst)] ^= preGen.wBitFileMask[sqDst];
        __ASSERT_BITRANK (pos->wBitRanks[RANK_Y (sqDst)]);
        __ASSERT_BITFILE (pos->wBitRanks[FILE_X (sqDst)]);
    }
}

static int cnBitScanTable[64] = {
  32,  0,  1, 12,  2,  6, -1, 13,  3, -1,  7, -1, -1, -1, -1, 14,
  10,  4, -1, -1,  8, -1, -1, 25, -1, -1, -1, -1, -1, 21, 27, 15,
  31, 11,  5, -1, -1, -1, -1, -1,  9, -1, -1, 24, -1, -1, 20, 26,
  30, -1, -1, -1, -1, 23, -1, 19, 29, -1, 22, 18, 28, 17, 16, -1
};

static int BitScan(uint32_t Operand) {
  uint32_t dw = (Operand << 4) + Operand;
  dw += dw << 6;
  dw = (dw << 16) - dw;
  return cnBitScanTable[dw >> 26];
}

static int Bsf(uint32_t Operand) {
#if defined(__i386__) || defined(__x86_64__)
  int eax;
  asm __volatile__ (
      "bsfl %0, %0"
      : "=a" (eax)
      : "0" (Operand)
      );
  return eax;
#else
  return BitScan(Operand & -Operand);
#endif
}

// 升变
int
Position_Promote (Position* pos, int sq)
{
    int pcCaptured, pcPromoted, pt;
    // 升变包括以下几个步骤：

    // 1. 得到升变前棋子的序号；
    __ASSERT_SQUARE (sq);
    __ASSERT (Position_CanPromote (pos));
    __ASSERT (CAN_PROMOTE (sq));
    pcCaptured = pos->ucpcSquares[sq];
    __ASSERT_PIECE (pcCaptured);
    pcPromoted = SIDE_TAG (pos->sdPlayer) + Bsf (~pos->wBitPiece[pos->sdPlayer] & PAWN_BITPIECE);
    __ASSERT_PIECE (pcPromoted);
    __ASSERT (pos->ucsqPieces[pcPromoted] == 0);

    // 2. 去掉升变前棋子，同时更新子力价值、Zobrist键值和校验锁
    pos->dwBitPiece ^= BIT_PIECE (pcPromoted) ^ BIT_PIECE (pcCaptured);
    pos->ucsqPieces[pcCaptured] = 0;
    pt = PIECE_TYPE (pcCaptured);
    if (pcCaptured < 32)
    {
        pos->vlWhite -= preEval.ucvlWhitePieces[pt][sq];
    } else {
        pos->vlBlack -= preEval.ucvlBlackPieces[pt][sq];
        pt += 7;
    }
    __ASSERT_BOUND (0, pt, 13);
    Xor1 (&pos->zobr, preGen.zobrTable[pt][sq]);

    // 3. 加上升变后棋子，同时更新子力价值、Zobrist键值和校验锁
    pos->ucpcSquares[sq] = pcPromoted;
    pos->ucsqPieces[pcPromoted] = sq;
    pt = PIECE_TYPE (pcPromoted);
    if (pcPromoted < 32)
    {
        pos->vlWhite += preEval.ucvlWhitePieces[pt][sq];
    } else {
        pos->vlBlack += preEval.ucvlBlackPieces[pt][sq];
        pt += 7;
    }
    __ASSERT_BOUND (0, pt, 13);
    Xor1 (&pos->zobr, preGen.zobrTable[pt][sq]);
    return pcCaptured;
}

// 撤消升变
void
Position_UndoPromote (Position* pos, int sq, int pcCaptured)
{
    int pcPromoted;
    __ASSERT_SQUARE (sq);
    __ASSERT_PIECE (pcCaptured);
    pcPromoted = pos->ucpcSquares[sq];
    __ASSERT (PIECE_TYPE (pcPromoted) == 6);
    pos->ucsqPieces[pcPromoted] = 0;
    pos->ucpcSquares[sq] = pcCaptured;
    pos->ucsqPieces[pcCaptured] = sq;
    pos->dwBitPiece ^= BIT_PIECE (pcPromoted) ^ BIT_PIECE (pcCaptured);
}

// 以上是一些棋盘处理过程

// 以下是一些着法处理过程

// 执行一个着法
bool
Position_MakeMove (Position* pos, int mv)
{
    int sq, pcCaptured;
    uint32_t dwOldZobristKey;
    Rollback *lprbs;

    // 如果达到最大着法数，那么判定为非法着法
    if (pos->nMoveNum == MAX_MOVE_NUM)
    {
        return false;
    }
    __ASSERT (pos->nMoveNum < MAX_MOVE_NUM);
    // 执行一个着法要包括以下几个步骤：

    // 1. 保存原来的Zobrist键值
    dwOldZobristKey = pos->zobr.dwKey;
    Position_SaveStatus (pos);

    // 2. 移动棋子，记住吃掉的子(如果有的话)
    sq = SRC (mv);
    if (sq == DST (mv))
    {
        pcCaptured = Position_Promote (pos, sq);
    } else {
        pcCaptured = Position_MovePiece (pos, mv);

        // 3. 如果移动后被将军了，那么着法是非法的，撤消该着法
        if (Position_CheckedBy (pos, CHECK_LAZY) > 0)
        {
            Position_UndoMovePiece (pos, mv, pcCaptured);
            Position_Rollback (pos);
            return false;
        }
    }

    // 4. 交换走子方
    Position_ChangeSide (pos);

    // 5. 把原来的Zobrist键值记录到检测重复的迷你置换表中
    if (pos->ucRepHash[dwOldZobristKey & REP_HASH_MASK] == 0)
    {
        pos->ucRepHash[dwOldZobristKey & REP_HASH_MASK] = pos->nMoveNum;
    }

    // 6. 把着法保存到历史着法列表中，并记住吃掉的子和将军状态
    lprbs = pos->rbsList + pos->nMoveNum;
    lprbs->mvs.wmv = mv;
    lprbs->mvs.ChkChs = Position_CheckedBy (pos, false);

    // 7. 设置和棋着法数(将军和应将不计入)
    if (pcCaptured == 0)
    {
        if (lprbs->mvs.ChkChs == 0)
        {
            lprbs->mvs.ChkChs = - Position_ChasedBy (pos, mv);
        }
        if (Position_LastMove (pos).CptDrw == -100)
        {
            lprbs->mvs.CptDrw = -100;
        } else {
            lprbs->mvs.CptDrw = MIN ((int) Position_LastMove (pos).CptDrw, 0) - (lprbs->mvs.ChkChs > 0 || Position_LastMove (pos).ChkChs > 0 ? 0 : 1);
        }
        __ASSERT_BOUND (-100, lprbs->mvs.CptDrw, 0);
    } else {
        lprbs->mvs.CptDrw = pcCaptured;
        __ASSERT_PIECE (pcCaptured);
    }
    pos->nMoveNum++;
    pos->nDistance++;

    return true;
}

// 撤消一个着法
void
Position_UndoMakeMove (Position *pos)
{
    int sq;
    Rollback *lprbs;
    pos->nMoveNum--;
    pos->nDistance--;
    lprbs = pos->rbsList + pos->nMoveNum;
    sq = SRC (lprbs->mvs.wmv);
    if (sq == DST (lprbs->mvs.wmv))
    {
        __ASSERT_BOUND (ADVISOR_TYPE, PIECE_TYPE (lprbs->mvs.CptDrw), BISHOP_TYPE);
        Position_UndoPromote (pos, sq, lprbs->mvs.CptDrw);
    } else {
        Position_UndoMovePiece (pos, lprbs->mvs.wmv, lprbs->mvs.CptDrw);
    }
    pos->sdPlayer = OPP_SIDE (pos->sdPlayer);
    Position_Rollback (pos);
    if (pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] == pos->nMoveNum)
    {
        pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] = 0;
    }
    __ASSERT (pos->nMoveNum > 0);
}

// 执行一个空着
void
Position_NullMove (Position *pos)
{
    __ASSERT (pos->nMoveNum < MAX_MOVE_NUM);
    if (pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] == 0)
    {
        pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] = pos->nMoveNum;
    }
    Position_SaveStatus (pos);
    Position_ChangeSide (pos);
    pos->rbsList[pos->nMoveNum].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
    pos->nMoveNum++;
    pos->nDistance++;
}

// 撤消一个空着
void
Position_UndoNullMove (Position* pos)
{
    pos->nMoveNum--;
    pos->nDistance--;
    pos->sdPlayer = OPP_SIDE (pos->sdPlayer);
    Position_Rollback (pos);
    if (pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] == pos->nMoveNum)
    {
        pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] = 0;
    }
    __ASSERT (pos->nMoveNum > 0);
}

// 以上是一些着法处理过程

// 以下是一些局面处理过程

// FEN串识别
void
Position_FromFen (Position* pos, const char *szFen)
{
    int i, j, k;
    int pcWhite[7];
    int pcBlack[7];
    const char *lpFen;
    // FEN串的识别包括以下几个步骤：
    // 1. 初始化，清空棋盘
    pcWhite[0] = SIDE_TAG (0) + KING_FROM;
    pcWhite[1] = SIDE_TAG (0) + ADVISOR_FROM;
    pcWhite[2] = SIDE_TAG (0) + BISHOP_FROM;
    pcWhite[3] = SIDE_TAG (0) + KNIGHT_FROM;
    pcWhite[4] = SIDE_TAG (0) + ROOK_FROM;
    pcWhite[5] = SIDE_TAG (0) + CANNON_FROM;
    pcWhite[6] = SIDE_TAG (0) + PAWN_FROM;
    for (i = 0; i < 7; i++)
    {
        pcBlack[i] = pcWhite[i] + 16;
    }
    /* 数组"pcWhite[7]"和"pcBlack[7]"分别代表红方和黑方每个兵种即将占有的序号，
     * 以"pcWhite[7]"为例，由于棋子16到31依次代表“帅仕仕相相马马车车炮炮兵兵兵兵兵”，
     * 所以最初应该是"pcWhite[7] = {16, 17, 19, 21, 23, 25, 27}"，每添加一个棋子，该项就增加1，
     * 这种做法允许添加多余的棋子(例如添加第二个帅，就变成仕了)，但添加前要做边界检测
     */
    Position_ClearBoard (pos);
    lpFen = szFen;
    if (*lpFen == '\0')
    {
        Position_SetIrrev (pos);
        return;
    }
    // 2. 读取棋盘上的棋子
    i = RANK_TOP;
    j = FILE_LEFT;
    while (*lpFen != ' ')
    {
        if (*lpFen == '/')
        {
            j = FILE_LEFT;
            i++;
            if (i > RANK_BOTTOM)
            {
                break;
            }
        } else if (*lpFen >= '1' && *lpFen <= '9')
        {
            j += (*lpFen - '0');
        } else if (*lpFen >= 'A' && *lpFen <= 'Z') {
            if (j <= FILE_RIGHT)
            {
                k = FenPiece (*lpFen);
                if (k < 7)
                {
                    if (pcWhite[k] < 32)
                    {
                        if (pos->ucsqPieces[pcWhite[k]] == 0)
                        {
                            Position_AddPiece (pos, COORD_XY (j, i), pcWhite[k], false);
                            pcWhite[k]++;
                        }
                    }
                }
                j++;
            }
        } else if (*lpFen >= 'a' && *lpFen <= 'z') {
            if (j <= FILE_RIGHT)
            {
                k = FenPiece (*lpFen + 'A' - 'a');
                if (k < 7)
                {
                    if (pcBlack[k] < 48)
                    {
                        if (pos->ucsqPieces[pcBlack[k]] == 0)
                        {
                            Position_AddPiece (pos, COORD_XY (j, i), pcBlack[k], false);
                            pcBlack[k]++;
                        }
                    }
                }
                j++;
            }
        }
        lpFen++;
        if (*lpFen == '\0')
        {
            Position_SetIrrev (pos);
            return;
        }
    }
    lpFen++;
    // 3. 确定轮到哪方走
    if (*lpFen == 'b')
    {
        Position_ChangeSide (pos);
    }
    // 4. 把局面设成“不可逆”
    Position_SetIrrev (pos);
}

// 生成FEN串
void
Position_ToFen (Position* pos, char *szFen)
{
    int i, j, k, pc;
    char *lpFen;

    lpFen = szFen;
    for (i = RANK_TOP; i <= RANK_BOTTOM; i++)
    {
        k = 0;
        for (j = FILE_LEFT; j <= FILE_RIGHT; j++)
        {
            pc = pos->ucpcSquares[COORD_XY (j, i)];
            if (pc != 0)
            {
                if (k > 0)
                {
                    *lpFen = k + '0';
                    lpFen++;
                    k = 0;
                }
                *lpFen = PIECE_BYTE (PIECE_TYPE (pc)) + (pc < 32 ? 0 : 'a' - 'A');
                lpFen++;
            } else {
                k++;
            }
        }
        if (k > 0)
        {
            *lpFen = k + '0';
            lpFen++;
        }
        *lpFen = '/';
        lpFen++;
    }
    *(lpFen - 1) = ' '; // 把最后一个'/'替换成' '
    *lpFen = (pos->sdPlayer == 0 ? 'w' : 'b');
    lpFen++;
    *lpFen = '\0';
}

// 局面镜像
void
Position_Mirror (Position* pos)
{
    int i, sq, nMoveNumSave;
    uint16_t wmvList[MAX_MOVE_NUM];
    uint8_t ucsqList[32];
    // 局面镜像需要按以下步骤依次进行：

    // 1. 记录所有历史着法
    nMoveNumSave = pos->nMoveNum;
    for (i = 1; i < nMoveNumSave; i++)
    {
        wmvList[i] = pos->rbsList[i].mvs.wmv;
    }

    // 2. 撤消所有着法
    for (i = 1; i < nMoveNumSave; i++)
    {
        Position_UndoMakeMove (pos);
    }

    // 3. 把所有棋子从棋盘上拿走，位置记录到"ucsqList"数组；
    for (i = 16; i < 48; i++)
    {
        sq = pos->ucsqPieces[i];
        ucsqList[i - 16] = sq;
        if (sq != 0)
        {
            Position_AddPiece (pos, sq, i, DEL_PIECE);
        }
    }

    // 4. 把拿走的棋子按照镜像位置重新放到棋盘上；
    for (i = 16; i < 48; i++)
    {
        sq = ucsqList[i - 16];
        if (sq != 0)
        {
            Position_AddPiece (pos, SQUARE_MIRROR (sq), i, false);
        }
    }

    // 6. 还原镜像着法
    Position_SetIrrev (pos);
    for (i = 1; i < nMoveNumSave; i++)
    {
        Position_MakeMove (pos, MOVE_MIRROR (wmvList[i]));
    }
}

// 以上是一些局面处理过程

// 以下是一些着法检测过程

// 着法合理性检测，仅用在“杀手着法”的检测中
bool
Position_LegalMove (Position* pos, int mv)
{
    int sqSrc, sqDst, sqPin, pcMoved, pcCaptured, x, y, nSideTag;
    // 着法合理性检测包括以下几个步骤：

    // 1. 检查要走的子是否存在
    nSideTag = SIDE_TAG (pos->sdPlayer);
    sqSrc = SRC (mv);
    sqDst = DST (mv);
    pcMoved = pos->ucpcSquares[sqSrc];
    if ((pcMoved & nSideTag) == 0)
    {
        return false;
    }
    __ASSERT_SQUARE (sqSrc);
    __ASSERT_SQUARE (sqDst);
    __ASSERT_PIECE (pcMoved);

    // 2. 检查吃到的子是否为对方棋子(如果有吃子并且没有升变的话)
    pcCaptured = pos->ucpcSquares[sqDst];
    if (sqSrc != sqDst && (pcCaptured & nSideTag) != 0)
    {
        return false;
    }
    __ASSERT_BOUND (0, PIECE_INDEX (pcMoved), 15);
    switch (PIECE_INDEX (pcMoved))
    {

    // 3. 如果是帅(将)或仕(士)，则先看是否在九宫内，再看是否是合理位移
    case KING_FROM:
        return IN_FORT (sqDst) && KING_SPAN (sqSrc, sqDst);
    case ADVISOR_FROM:
    case ADVISOR_TO:
        if (sqSrc == sqDst)
        {
            // 考虑升变，在底线并且兵(卒)不全时，才可升变
            return CAN_PROMOTE (sqSrc) && Position_CanPromote (pos);
        } else {
            return IN_FORT (sqDst) && ADVISOR_SPAN (sqSrc, sqDst);
        }

    // 4. 如果是相(象)，则先看是否过河，再看是否是合理位移，最后看有没有被塞象眼
    case BISHOP_FROM:
    case BISHOP_TO:
        if (sqSrc == sqDst)
        {
            // 考虑升变，在底线并且兵(卒)不全时，才可升变
            return CAN_PROMOTE (sqSrc) && Position_CanPromote (pos);
        } else {
            return SAME_HALF (sqSrc, sqDst) && BISHOP_SPAN (sqSrc, sqDst) && pos->ucpcSquares[BISHOP_PIN (sqSrc, sqDst)] == 0;
        }

    // 5. 如果是马，则先看看是否是合理位移，再看有没有被蹩马腿
    case KNIGHT_FROM:
    case KNIGHT_TO:
        sqPin = KNIGHT_PIN (sqSrc, sqDst);
        return sqPin != sqSrc && pos->ucpcSquares[sqPin] == 0;

    // 6. 如果是车，则先看是横向移动还是纵向移动，再读取位行或位列的着法预生成数组
    case ROOK_FROM:
    case ROOK_TO:
        x = FILE_X (sqSrc);
        y = RANK_Y (sqSrc);
        if (x == FILE_X (sqDst))
        {
            if (pcCaptured == 0)
            {
                return (Position_FileMaskPtr (pos, x, y)->wNonCap & preGen.wBitFileMask[sqDst]) != 0;
            } else {
                return (Position_FileMaskPtr (pos, x, y)->wRookCap & preGen.wBitFileMask[sqDst]) != 0;
            }
        } else if (y == RANK_Y (sqDst)) {
            if (pcCaptured == 0)
            {
                return (Position_RankMaskPtr (pos, x, y)->wNonCap & preGen.wBitRankMask[sqDst]) != 0;
            } else {
                return (Position_RankMaskPtr (pos, x, y)->wRookCap & preGen.wBitRankMask[sqDst]) != 0;
            }
        } else {
            return false;
        }

    // 7. 如果是炮，判断起来和车一样
    case CANNON_FROM:
    case CANNON_TO:
        x = FILE_X (sqSrc);
        y = RANK_Y (sqSrc);
        if (x == FILE_X (sqDst))
        {
            if (pcCaptured == 0)
            {
                return (Position_FileMaskPtr (pos, x, y)->wNonCap & preGen.wBitFileMask[sqDst]) != 0;
            } else {
                return (Position_FileMaskPtr (pos, x, y)->wCannonCap & preGen.wBitFileMask[sqDst]) != 0;
            }
        } else if (y == RANK_Y (sqDst)) {
            if (pcCaptured == 0)
            {
                return (Position_RankMaskPtr (pos, x, y)->wNonCap & preGen.wBitRankMask[sqDst]) != 0;
            } else {
                return (Position_RankMaskPtr (pos, x, y)->wCannonCap & preGen.wBitRankMask[sqDst]) != 0;
            }
        } else {
            return false;
        }

    // 8. 如果是兵(卒)，则按红方和黑方分情况讨论
    default:
        if (AWAY_HALF (sqDst, pos->sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1))
        {
            return true;
        } else {
            return sqDst == SQUARE_FORWARD (sqSrc, pos->sdPlayer);
        }
    }
}

// 将军检测
int
Position_CheckedBy (Position* pos, bool bLazy)
{
    int pcCheckedBy, i, sqSrc, sqDst, sqPin, pc, x, y, nOppSideTag;
    SlideMask *lpsmsRank, *lpsmsFile;

    pcCheckedBy = 0;
    nOppSideTag = OPP_SIDE_TAG (pos->sdPlayer);
    // 将军判断包括以下几部分内容：

    // 1. 判断帅(将)是否在棋盘上
    sqSrc = pos->ucsqPieces[SIDE_TAG (pos->sdPlayer)];
    if (sqSrc == 0)
    {
        return 0;
    }
    __ASSERT_SQUARE (sqSrc);

    // 2. 获得帅(将)所在格子的位行和位列
    x = FILE_X (sqSrc);
    y = RANK_Y (sqSrc);
    lpsmsRank = Position_RankMaskPtr (pos, x, y);
    lpsmsFile = Position_FileMaskPtr (pos, x, y);

    // 3. 判断是否将帅对脸
    sqDst = pos->ucsqPieces[nOppSideTag + KING_FROM];
    if (sqDst != 0)
    {
        __ASSERT_SQUARE (sqDst);
        if (x == FILE_X (sqDst) && (lpsmsFile->wRookCap & preGen.wBitFileMask[sqDst]) != 0)
        {
            return CHECK_MULTI;
        }
    }

    // 4. 判断是否被马将军
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i++)
    {
        sqDst = pos->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            sqPin = KNIGHT_PIN (sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
            if (sqPin != sqDst && pos->ucpcSquares[sqPin] == 0)
            {
                if (bLazy || pcCheckedBy > 0)
                {
                    return CHECK_MULTI;
                }
                pcCheckedBy = nOppSideTag + i;
                __ASSERT_PIECE (pcCheckedBy);
            }
        }
    }

    // 5. 判断是否被车将军或将帅对脸
    for (i = ROOK_FROM; i <= ROOK_TO; i++)
    {
        sqDst = pos->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            if (x == FILE_X (sqDst))
            {
                if ((lpsmsFile->wRookCap & preGen.wBitFileMask[sqDst]) != 0)
                {
                    if (bLazy || pcCheckedBy > 0)
                    {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE (pcCheckedBy);
                }
            } else if (y == RANK_Y (sqDst)) {
                if ((lpsmsRank->wRookCap & preGen.wBitRankMask[sqDst]) != 0)
                {
                    if (bLazy || pcCheckedBy > 0)
                    {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE (pcCheckedBy);
                }
            }
        }
    }

    // 6. 判断是否被炮将军
    for (i = CANNON_FROM; i <= CANNON_TO; i++)
    {
        sqDst = pos->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            if (x == FILE_X (sqDst))
            {
                if ((lpsmsFile->wCannonCap & preGen.wBitFileMask[sqDst]) != 0)
                {
                    if (bLazy || pcCheckedBy > 0)
                    {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE (pcCheckedBy);
                }
            } else if (y == RANK_Y (sqDst)) {
                if ((lpsmsRank->wCannonCap & preGen.wBitRankMask[sqDst]) != 0)
                {
                    if (bLazy || pcCheckedBy > 0)
                    {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE (pcCheckedBy);
                }
            }
        }
    }

    // 7. 判断是否被兵(卒)将军
    for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2)
    {
        // 如果帅(将)在边线(ElephantEye允许)，那么断言不成立
        // __ASSERT_SQUARE(sqDst);
        pc = pos->ucpcSquares[sqDst];
        if ((pc & nOppSideTag) != 0 && PIECE_INDEX (pc) >= PAWN_FROM)
        {
            if (bLazy || pcCheckedBy > 0)
            {
                return CHECK_MULTI;
            }
            pcCheckedBy = nOppSideTag + i;
            __ASSERT_PIECE (pcCheckedBy);
        }
    }
    pc = pos->ucpcSquares[SQUARE_FORWARD (sqSrc, pos->sdPlayer)];
    if ((pc & nOppSideTag) != 0 && PIECE_INDEX (pc) >= PAWN_FROM)
    {
        if (bLazy || pcCheckedBy > 0)
        {
            return CHECK_MULTI;
        }
        pcCheckedBy = nOppSideTag + i;
        __ASSERT_PIECE (pcCheckedBy);
    }
    return pcCheckedBy;
}

// 判断是否被将死
bool
Position_IsMate (Position* pos)
{
    int i, nGenNum;
    Move mvsGen[MAX_GEN_MOVES];
    nGenNum = Position_GenCapMoves (pos, mvsGen);
    for (i = 0; i < nGenNum; i++)
    {
        if (Position_MakeMove (pos, mvsGen[i].wmv))
        {
            Position_UndoMakeMove (pos);
            return false;
        }
    }
    // 着法生成分两部分做，这样可以节约时间
    nGenNum = Position_GenNonCapMoves (pos, mvsGen);
    for (i = 0; i < nGenNum; i++)
    {
        if (Position_MakeMove (pos, mvsGen[i].wmv))
        {
            Position_UndoMakeMove (pos);
            return false;
        }
    }
    return true;
}

// 设置将军状态位
void SetPerpCheck (uint32_t *dwPerpCheck, int nChkChs)
{
    if (nChkChs == 0)
    {
        *dwPerpCheck = 0;
    } else if (nChkChs > 0) {
        *dwPerpCheck &= 0x10000;
    } else {
        *dwPerpCheck &= (1 << -nChkChs);
    }
}

// 重复局面检测
int
Position_RepStatus (Position* pos, int nRecur)
{
    // 参数"nRecur"指重复次数，在搜索中取1以提高搜索效率(默认值)，根结点处取3以适应规则
    int sd;
    uint32_t dwPerpCheck, dwOppPerpCheck;
    const Rollback *lprbs;
    /* 重复局面检测包括以下几个步骤：
     *
     * 1. 首先判断检测重复的迷你置换表中是否可能有当前局面，如果没有可能，就用不着判断了
     *    置换表"ucRepHash"是ElephantEye的一个特色，局面每执行一个着法时，就会在置换表项中记录下当前的"nMoveNum"
     *    如果置换表项已经填有其他局面，就不必覆盖了，参阅"MakeMove()"函数
     *    因此撤消着法时，只要查找置换表项的值是否等于当前的"nMoveNum"，如果相等则清空该项
     *    如果不等于当前的"nMoveNum"，则说明之前还有局面占有这个置换表项，不必清空该项，参阅"position.h"中的"UndoMakeMove()"函数
     */
    if (pos->ucRepHash[pos->zobr.dwKey & REP_HASH_MASK] == 0)
    {
        return REP_NONE;
    }

    // 2. 初始化
    sd = OPP_SIDE (pos->sdPlayer);
    dwPerpCheck = dwOppPerpCheck = 0x1ffff;
    lprbs = pos->rbsList + pos->nMoveNum - 1;

    // 3. 检查上一个着法，如果是空着或吃子着法，就不可能有重复了
    while (lprbs->mvs.wmv != 0 && lprbs->mvs.CptDrw <= 0)
    {
        __ASSERT (lprbs >= pos->rbsList);

        // 4. 判断双方的长打级别，0表示无长打，0xffff表示长捉，0x10000表示长将
        if (sd == pos->sdPlayer)
        {
            SetPerpCheck (&dwPerpCheck, lprbs->mvs.ChkChs);

            // 5. 寻找重复局面，如果重复次数达到预定次数，则返回重复记号
            if (lprbs->zobr.dwLock0 == pos->zobr.dwLock0 && lprbs->zobr.dwLock1 == pos->zobr.dwLock1)
            {
                nRecur--;
                if (nRecur == 0)
                {
                    dwPerpCheck = ((dwPerpCheck & 0xffff) == 0 ? dwPerpCheck : 0xffff);
                    dwOppPerpCheck = ((dwOppPerpCheck & 0xffff) == 0 ? dwOppPerpCheck : 0xffff);
                    return dwPerpCheck > dwOppPerpCheck ? REP_LOSS : dwPerpCheck < dwOppPerpCheck ? REP_WIN
                                                                                                  : REP_DRAW;
                }
            }

        } else {
            SetPerpCheck (&dwOppPerpCheck, lprbs->mvs.ChkChs);
        }

        sd = OPP_SIDE (sd);
        lprbs--;
    }
    return REP_NONE;
}

// 棋子保护判断
bool
Position_Protected (Position* pos, int sd, int sqSrc, int sqExcept)
{
    // 参数"sqExcept"表示排除保护的棋子(指格子编号)，考虑被牵制子的保护时，需要排除牵制目标子的保护
    int i, sqDst, sqPin, pc, x, y, nSideTag;
    SlideMask *lpsmsRank, *lpsmsFile;
    // 棋子保护判断包括以下几个步骤：

    __ASSERT_SQUARE (sqSrc);
    nSideTag = SIDE_TAG (sd);
    if (HOME_HALF (sqSrc, sd))
    {
        if (IN_FORT (sqSrc))
        {

            // 1. 判断受到帅(将)的保护
            sqDst = pos->ucsqPieces[nSideTag + KING_FROM];
            if (sqDst != 0 && sqDst != sqExcept)
            {
                __ASSERT_SQUARE (sqDst);
                if (KING_SPAN (sqSrc, sqDst))
                {
                    return true;
                }
            }

            // 2. 判断受到仕(士)的保护
            for (i = ADVISOR_FROM; i <= ADVISOR_TO; i++)
            {
                sqDst = pos->ucsqPieces[nSideTag + i];
                if (sqDst != 0 && sqDst != sqExcept)
                {
                    __ASSERT_SQUARE (sqDst);
                    if (ADVISOR_SPAN (sqSrc, sqDst))
                    {
                        return true;
                    }
                }
            }
        }

        // 3. 判断受到相(象)的保护
        for (i = BISHOP_FROM; i <= BISHOP_TO; i++)
        {
            sqDst = pos->ucsqPieces[nSideTag + i];
            if (sqDst != 0 && sqDst != sqExcept)
            {
                __ASSERT_SQUARE (sqDst);
                if (BISHOP_SPAN (sqSrc, sqDst) && pos->ucpcSquares[BISHOP_PIN (sqSrc, sqDst)] == 0)
                {
                    return true;
                }
            }
        }
    } else {

        // 4. 判断受到过河兵(卒)横向的保护
        for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2)
        {
            // 如果棋子在边线，那么断言不成立
            // __ASSERT_SQUARE(sqDst);
            if (sqDst != sqExcept)
            {
                pc = pos->ucpcSquares[sqDst];
                if ((pc & nSideTag) != 0 && PIECE_INDEX (pc) >= PAWN_FROM)
                {
                    return true;
                }
            }
        }
    }

    // 5. 判断受到兵(卒)纵向的保护
    sqDst = SQUARE_BACKWARD (sqSrc, sd);
    // 如果棋子在底线，那么断言不成立
    // __ASSERT_SQUARE(sqDst);
    if (sqDst != sqExcept)
    {
        pc = pos->ucpcSquares[sqDst];
        if ((pc & nSideTag) != 0 && PIECE_INDEX (pc) >= PAWN_FROM)
        {
            return true;
        }
    }

    // 6. 判断受到马的保护
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i++)
    {
        sqDst = pos->ucsqPieces[nSideTag + i];
        if (sqDst != 0 && sqDst != sqExcept)
        {
            __ASSERT_SQUARE (sqDst);
            sqPin = KNIGHT_PIN (sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
            if (sqPin != sqDst && pos->ucpcSquares[sqPin] == 0)
            {
                return true;
            }
        }
    }

    x = FILE_X (sqSrc);
    y = RANK_Y (sqSrc);
    lpsmsRank = Position_RankMaskPtr (pos, x, y);
    lpsmsFile = Position_FileMaskPtr (pos, x, y);

    // 7. 判断受到车的保护，参阅"position.cpp"里的"CheckedBy()"函数
    for (i = ROOK_FROM; i <= ROOK_TO; i++)
    {
        sqDst = pos->ucsqPieces[nSideTag + i];
        if (sqDst != 0 && sqDst != sqSrc && sqDst != sqExcept)
        {
            if (x == FILE_X (sqDst))
            {
                if ((lpsmsFile->wRookCap & preGen.wBitFileMask[sqDst]) != 0)
                {
                    return true;
                }
            } else if (y == RANK_Y (sqDst)) {
                if ((lpsmsRank->wRookCap & preGen.wBitRankMask[sqDst]) != 0)
                {
                    return true;
                }
            }
        }
    }

    // 8. 判断受到炮的保护，参阅"position.cpp"里的"CheckedBy()"函数
    for (i = CANNON_FROM; i <= CANNON_TO; i++)
    {
        sqDst = pos->ucsqPieces[nSideTag + i];
        if (sqDst && sqDst != sqSrc && sqDst != sqExcept)
        {
            if (x == FILE_X (sqDst))
            {
                if ((lpsmsFile->wCannonCap & preGen.wBitFileMask[sqDst]) != 0)
                {
                    return true;
                }
            } else if (y == RANK_Y (sqDst)) {
                if ((lpsmsRank->wCannonCap & preGen.wBitRankMask[sqDst]) != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/* 计算MVV(LVA)值的函数
 *
 * MVV(LVA)指的是：如果被吃子无保护，那么取值MVV，否则取值MVV-LVA。
 * 由于ElephantEye的MVV(LVA)值在计算完毕后再加了1，并且有其它考虑，因此有以下几种含义：
 * a. MVV(LVA)大于1，说明被吃子价值大于攻击子(表面上是赚的)，这种吃子将首先搜索，静态搜索也将考虑这种吃子；
 * b. MVV(LVA)等于1，说明被吃子有一定价值(吃车马炮或吃过河兵卒，即便表面上是亏的，也值得一试)，静态搜索也将考虑这种吃子；
 * c. MVV(LVA)等于0，说明被吃子没有价值，静态搜索将不考虑这种吃子。
 *
 * MVV价值表"SIMPLE_VALUE"是按照帅(将)=5、车=4、马炮=3、兵(卒)=2、仕(士)相(象)=1设定的；
 * LVA价值直接体现在吃子着法生成器中。
 */
int
Position_MvvLva (Position* pos, int sqDst, int pcCaptured, int nLva)
{
    int nMvv, nLvaAdjust;
    nMvv = SIMPLE_VALUE (pcCaptured);
    nLvaAdjust = (Position_Protected (pos, OPP_SIDE (pos->sdPlayer), sqDst, 0) ? nLva : 0);
    if (nMvv >= nLvaAdjust)
    {
        return nMvv - nLvaAdjust + 1;
    } else {
        return (nMvv >= 3 || HOME_HALF (sqDst, pos->sdPlayer)) ? 1 : 0;
    }
}

// 吃子着法生成器，按MVV(LVA)设定分值
int
Position_GenCapMoves (Position* pos, Move *lpmvs)
{
    int i, sqSrc, sqDst, pcCaptured;
    int x, y, nSideTag, nOppSideTag;
    bool bCanPromote;
    SlideMove *lpsmv;
    uint8_t *lpucsqDst, *lpucsqPin;
    Move *lpmvsCurr;
    // 生成吃子着法的过程包括以下几个步骤：

    lpmvsCurr = lpmvs;
    nSideTag = SIDE_TAG (pos->sdPlayer);
    nOppSideTag = OPP_SIDE_TAG (pos->sdPlayer);
    bCanPromote = preEval.bPromotion && Position_CanPromote (pos);

    // 1. 生成帅(将)的着法
    sqSrc = pos->ucsqPieces[nSideTag + KING_FROM];
    if (sqSrc != 0)
    {
        __ASSERT_SQUARE (sqSrc);
        lpucsqDst = preGen.ucsqKingMoves[sqSrc];
        sqDst = *lpucsqDst;
        while (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            // 找到一个着法后，首先判断吃到的棋子是否是对方棋子，技巧是利用"nOppSideTag"的标志(16和32颠倒)，
            // 如果是对方棋子，则保存MVV(LVA)值，即如果被吃子无保护，则只记MVV，否则记MVV-LVA(如果MVV>LVA的话)。
            pcCaptured = pos->ucpcSquares[sqDst];
            if ((pcCaptured & nOppSideTag) != 0)
            {
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 5); // 帅(将)的价值是5
                lpmvsCurr++;
            }
            lpucsqDst++;
            sqDst = *lpucsqDst;
        }
    }

    // 2. 生成仕(士)的着法
    for (i = ADVISOR_FROM; i <= ADVISOR_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqAdvisorMoves[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 1); // 仕(士)的价值是1
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
            }
            if (bCanPromote && CAN_PROMOTE (sqSrc))
            {
                lpmvsCurr->wmv = MOVE (sqSrc, sqSrc);
                lpmvsCurr->wvl = 0;
                lpmvsCurr++;
            }
        }
    }

    // 3. 生成相(象)的着法
    for (i = BISHOP_FROM; i <= BISHOP_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqBishopMoves[sqSrc];
            lpucsqPin = preGen.ucsqBishopPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[*lpucsqPin] == 0)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nOppSideTag) != 0)
                    {
                        __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                        lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                        lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 1); // 相(象)的价值是1
                        lpmvsCurr++;
                    }
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
                lpucsqPin++;
            }
            if (bCanPromote && CAN_PROMOTE (sqSrc))
            {
                lpmvsCurr->wmv = MOVE (sqSrc, sqSrc);
                lpmvsCurr->wvl = 0;
                lpmvsCurr++;
            }
        }
    }

    // 4. 生成马的着法
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqKnightMoves[sqSrc];
            lpucsqPin = preGen.ucsqKnightPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[*lpucsqPin] == 0)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nOppSideTag) != 0)
                    {
                        __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                        lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                        lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 3); // 马的价值是3
                        lpmvsCurr++;
                    }
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
                lpucsqPin++;
            }
        }
    }

    // 5. 生成车的着法
    for (i = ROOK_FROM; i <= ROOK_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            x = FILE_X (sqSrc);
            y = RANK_Y (sqSrc);

            lpsmv = Position_RankMovePtr (pos, x, y);
            sqDst = lpsmv->ucRookCap[0] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr++;
                }
            }
            sqDst = lpsmv->ucRookCap[1] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr++;
                }
            }

            lpsmv = Position_FileMovePtr (pos, x, y);
            sqDst = lpsmv->ucRookCap[0] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr++;
                }
            }
            sqDst = lpsmv->ucRookCap[1] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr++;
                }
            }
        }
    }

    // 6. 生成炮的着法
    for (i = CANNON_FROM; i <= CANNON_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            x = FILE_X (sqSrc);
            y = RANK_Y (sqSrc);

            lpsmv = Position_RankMovePtr (pos, x, y);
            sqDst = lpsmv->ucCannonCap[0] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr++;
                }
            }
            sqDst = lpsmv->ucCannonCap[1] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr++;
                }
            }

            lpsmv = Position_FileMovePtr (pos, x, y);
            sqDst = lpsmv->ucCannonCap[0] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr++;
                }
            }
            sqDst = lpsmv->ucCannonCap[1] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            if (sqDst != sqSrc)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr++;
                }
            }
        }
    }

    // 7. 生成兵(卒)的着法
    for (i = PAWN_FROM; i <= PAWN_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqPawnMoves[pos->sdPlayer][sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr->wvl = Position_MvvLva (pos, sqDst, pcCaptured, 2); // 兵(卒)的价值是2
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
            }
        }
    }
    return lpmvsCurr - lpmvs;
}

// 不吃子着法生成器
int
Position_GenNonCapMoves (Position* pos, Move *lpmvs)
{
    int i, sqSrc, sqDst, x, y, nSideTag;
    SlideMove *lpsmv;
    uint8_t *lpucsqDst, *lpucsqPin;
    Move *lpmvsCurr;
    // 生成不吃子着法的过程包括以下几个步骤：

    lpmvsCurr = lpmvs;
    nSideTag = SIDE_TAG (pos->sdPlayer);

    // 1. 生成帅(将)的着法
    sqSrc = pos->ucsqPieces[nSideTag + KING_FROM];
    if (sqSrc != 0)
    {
        __ASSERT_SQUARE (sqSrc);
        lpucsqDst = preGen.ucsqKingMoves[sqSrc];
        sqDst = *lpucsqDst;
        while (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            // 找到一个着法后，首先判断是否吃到棋子
            if (pos->ucpcSquares[sqDst] == 0)
            {
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                lpmvsCurr++;
            }
            lpucsqDst++;
            sqDst = *lpucsqDst;
        }
    }

    // 2. 生成仕(士)的着法
    for (i = ADVISOR_FROM; i <= ADVISOR_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqAdvisorMoves[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[sqDst] == 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
            }
        }
    }

    // 3. 生成相(象)的着法
    for (i = BISHOP_FROM; i <= BISHOP_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqBishopMoves[sqSrc];
            lpucsqPin = preGen.ucsqBishopPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[*lpucsqPin] == 0 && pos->ucpcSquares[sqDst] == 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
                lpucsqPin++;
            }
        }
    }

    // 4. 生成马的着法
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqKnightMoves[sqSrc];
            lpucsqPin = preGen.ucsqKnightPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[*lpucsqPin] == 0 && pos->ucpcSquares[sqDst] == 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
                lpucsqPin++;
            }
        }
    }

    // 5. 生成车和炮的着法，没有必要判断是否吃到本方棋子
    for (i = ROOK_FROM; i <= CANNON_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            x = FILE_X (sqSrc);
            y = RANK_Y (sqSrc);

            lpsmv = Position_RankMovePtr (pos, x, y);
            sqDst = lpsmv->ucNonCap[0] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            while (sqDst != sqSrc)
            {
                __ASSERT (pos->ucpcSquares[sqDst] == 0);
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                lpmvsCurr++;
                sqDst--;
            }
            sqDst = lpsmv->ucNonCap[1] + RANK_DISP (y);
            __ASSERT_SQUARE (sqDst);
            while (sqDst != sqSrc)
            {
                __ASSERT (pos->ucpcSquares[sqDst] == 0);
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                lpmvsCurr++;
                sqDst++;
            }

            lpsmv = Position_FileMovePtr (pos, x, y);
            sqDst = lpsmv->ucNonCap[0] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            while (sqDst != sqSrc)
            {
                __ASSERT (pos->ucpcSquares[sqDst] == 0);
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                lpmvsCurr++;
                sqDst -= 16;
            }
            sqDst = lpsmv->ucNonCap[1] + FILE_DISP (x);
            __ASSERT_SQUARE (sqDst);
            while (sqDst != sqSrc)
            {
                __ASSERT (pos->ucpcSquares[sqDst] == 0);
                __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                lpmvsCurr++;
                sqDst += 16;
            }
        }
    }

    // 6. 生成兵(卒)的着法
    for (i = PAWN_FROM; i <= PAWN_TO; i++)
    {
        sqSrc = pos->ucsqPieces[nSideTag + i];
        if (sqSrc != 0)
        {
            __ASSERT_SQUARE (sqSrc);
            lpucsqDst = preGen.ucsqPawnMoves[pos->sdPlayer][sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0)
            {
                __ASSERT_SQUARE (sqDst);
                if (pos->ucpcSquares[sqDst] == 0)
                {
                    __ASSERT (Position_LegalMove (pos, MOVE (sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE (sqSrc, sqDst);
                    lpmvsCurr++;
                }
                lpucsqDst++;
                sqDst = *lpucsqDst;
            }
        }
    }
    return lpmvsCurr - lpmvs;
}

// “捉”的检测
int
Position_ChasedBy (Position* pos, int mv)
{
    int i, nSideTag, pcMoved, pcCaptured;
    int sqSrc, sqDst, x, y;
    uint8_t *lpucsqDst, *lpucsqPin;
    SlideMove *lpsmv;

    sqSrc = DST (mv);
    pcMoved = pos->ucpcSquares[sqSrc];
    nSideTag = SIDE_TAG (pos->sdPlayer);
    __ASSERT_SQUARE (sqSrc);
    __ASSERT_PIECE (pcMoved);
    __ASSERT_BOUND (0, pcMoved - OPP_SIDE_TAG (pos->sdPlayer), 15);

    // “捉”的判断包括以下几部分内容：
    switch (pcMoved - OPP_SIDE_TAG (pos->sdPlayer))
    {

    // 1. 走了马，判断是否捉车或捉有根的炮兵(卒)
    case KNIGHT_FROM:
    case KNIGHT_TO:
        // 逐一检测马踩的八个位置
        lpucsqDst = preGen.ucsqKnightMoves[sqSrc];
        lpucsqPin = preGen.ucsqKnightPins[sqSrc];
        sqDst = *lpucsqDst;
        while (sqDst != 0)
        {
            __ASSERT_SQUARE (sqDst);
            if (pos->ucpcSquares[*lpucsqPin] == 0)
            {
                pcCaptured = pos->ucpcSquares[sqDst];
                if ((pcCaptured & nSideTag) != 0)
                {
                    pcCaptured -= nSideTag;
                    __ASSERT_BOUND (0, pcCaptured, 15);
                    // 技巧：优化兵种判断的分枝
                    if (pcCaptured <= ROOK_TO)
                    {
                        // 马捉仕(士)、相(象)和马的情况不予考虑
                        if (pcCaptured >= ROOK_FROM)
                        {
                            // 马捉到了车
                            return pcCaptured;
                        }
                    } else {
                        if (pcCaptured <= CANNON_TO)
                        {
                            // 马捉到了炮，要判断炮是否受保护
                            if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                            {
                                return pcCaptured;
                            }
                        } else {
                            // 马捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                            if (AWAY_HALF (sqDst, pos->sdPlayer) && !Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                            {
                                return pcCaptured;
                            }
                        }
                    }
                }
            }
            lpucsqDst++;
            sqDst = *lpucsqDst;
            lpucsqPin++;
        }
        break;

    // 2. 走了车，判断是否捉有根的马炮兵(卒)
    case ROOK_FROM:
    case ROOK_TO:
        x = FILE_X (sqSrc);
        y = RANK_Y (sqSrc);
        if (((SRC (mv) ^ sqSrc) & 0xf) == 0)
        {
            // 如果车纵向移动了，则判断车横向吃到的子
            lpsmv = Position_RankMovePtr (pos, x, y);
            for (i = 0; i < 2; i++)
            {
                sqDst = lpsmv->ucRookCap[i] + RANK_DISP (y);
                __ASSERT_SQUARE (sqDst);
                if (sqDst != sqSrc)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nSideTag) != 0)
                    {
                        pcCaptured -= nSideTag;
                        __ASSERT_BOUND (0, pcCaptured, 15);
                        // 技巧：优化兵种判断的分枝
                        if (pcCaptured <= ROOK_TO)
                        {
                            // 车捉仕(士)、相(象)的情况不予考虑
                            if (pcCaptured >= KNIGHT_FROM)
                            {
                                if (pcCaptured <= KNIGHT_TO)
                                {
                                    // 车捉到了马，要判断马是否受保护
                                    if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                    {
                                        return pcCaptured;
                                    }
                                }
                                // 车捉车的情况不予考虑
                            }
                        } else {
                            if (pcCaptured <= CANNON_TO)
                            {
                                // 车捉到了炮，要判断炮是否受保护
                                if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            } else {
                                // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                if (AWAY_HALF (sqDst, pos->sdPlayer) && !Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // 如果车横向移动了，则判断车纵向吃到的子
            lpsmv = Position_FileMovePtr (pos, x, y);
            for (i = 0; i < 2; i++)
            {
                sqDst = lpsmv->ucRookCap[i] + FILE_DISP (x);
                __ASSERT_SQUARE (sqDst);
                if (sqDst != sqSrc)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nSideTag) != 0)
                    {
                        pcCaptured -= nSideTag;
                        __ASSERT_BOUND (0, pcCaptured, 15);
                        // 技巧：优化兵种判断的分枝
                        if (pcCaptured <= ROOK_TO)
                        {
                            // 车捉仕(士)、相(象)的情况不予考虑
                            if (pcCaptured >= KNIGHT_FROM)
                            {
                                if (pcCaptured <= KNIGHT_TO)
                                {
                                    // 车捉到了马，要判断马是否受保护
                                    if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                    {
                                        return pcCaptured;
                                    }
                                }
                                // 车捉车的情况不予考虑
                            }
                        } else {
                            if (pcCaptured <= CANNON_TO)
                            {
                                // 车捉到了炮，要判断炮是否受保护
                                if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            } else {
                                // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                if (AWAY_HALF (sqDst, pos->sdPlayer) && !Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    // 3. 走了炮，判断是否捉车或捉有根的马兵(卒)
    case CANNON_FROM:
    case CANNON_TO:
        x = FILE_X (sqSrc);
        y = RANK_Y (sqSrc);
        if (((SRC (mv) ^ sqSrc) & 0xf) == 0)
        {
            // 如果炮纵向移动了，则判断炮横向吃到的子
            lpsmv = Position_RankMovePtr (pos, x, y);
            for (i = 0; i < 2; i++)
            {
                sqDst = lpsmv->ucCannonCap[i] + RANK_DISP (y);
                __ASSERT_SQUARE (sqDst);
                if (sqDst != sqSrc)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nSideTag) != 0)
                    {
                        pcCaptured -= nSideTag;
                        __ASSERT_BOUND (0, pcCaptured, 15);
                        // 技巧：优化兵种判断的分枝
                        if (pcCaptured <= ROOK_TO)
                        {
                            // 炮捉仕(士)、相(象)的情况不予考虑
                            if (pcCaptured >= KNIGHT_FROM)
                            {
                                if (pcCaptured <= KNIGHT_TO)
                                {
                                    // 炮捉到了马，要判断马是否受保护
                                    if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                    {
                                        return pcCaptured;
                                    }
                                } else {
                                    // 炮捉到了车
                                    return pcCaptured;
                                }
                            }
                        } else {
                            // 炮捉炮的情况不予考虑
                            if (pcCaptured >= PAWN_FROM)
                            {
                                // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                if (AWAY_HALF (sqDst, pos->sdPlayer) && !Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // 如果炮横向移动了，则判断炮纵向吃到的子
            lpsmv = Position_FileMovePtr (pos, x, y);
            for (i = 0; i < 2; i++)
            {
                sqDst = lpsmv->ucCannonCap[i] + FILE_DISP (x);
                __ASSERT_SQUARE (sqDst);
                if (sqDst != sqSrc)
                {
                    pcCaptured = pos->ucpcSquares[sqDst];
                    if ((pcCaptured & nSideTag) != 0)
                    {
                        pcCaptured -= nSideTag;
                        __ASSERT_BOUND (0, pcCaptured, 15);
                        // 技巧：优化兵种判断的分枝
                        if (pcCaptured <= ROOK_TO)
                        {
                            // 炮捉仕(士)、相(象)的情况不予考虑
                            if (pcCaptured >= KNIGHT_FROM)
                            {
                                if (pcCaptured <= KNIGHT_TO)
                                {
                                    // 炮捉到了马，要判断马是否受保护
                                    if (!Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                    {
                                        return pcCaptured;
                                    }
                                } else {
                                    // 炮捉到了车
                                    return pcCaptured;
                                }
                            }
                        } else {
                            // 炮捉炮的情况不予考虑
                            if (pcCaptured >= PAWN_FROM)
                            {
                                // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                if (AWAY_HALF (sqDst, pos->sdPlayer) && !Position_Protected (pos, pos->sdPlayer, sqDst, 0))
                                {
                                    return pcCaptured;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
      break;
    }

    return 0;
}
