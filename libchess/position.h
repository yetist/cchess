/*
position.h - Source Code for ElephantEye, Part III

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com
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

#ifndef POSITION_H
#define POSITION_H

#define MAX_MOVE_NUM 1024  // 局面能容纳的回滚着法数
#define REP_HASH_MASK 4095 // 判断重复局面的迷你置换表长度，即4096个表项
#define MAX_GEN_MOVES 128  // 搜索的最大着法数，中国象棋的任何局面都不会超过120个着法
#define DRAW_MOVES   100      // 默认的和棋着法数，ElephantEye设定在50回合即100步，但将军和应将不计入其中

#define MATE_VALUE   10000            // 最高分值，即将死的分值
#define BAN_VALUE   MATE_VALUE - 100  // 长将判负的分值，低于该值将不写入置换表
#define WIN_VALUE   MATE_VALUE - 200  // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
#define NULLOKAY_MARGIN   200         // 空着裁剪可以不检验的子力价值边界
#define NULLSAFE_MARGIN   400         // 允许使用空着裁剪的条件的子力价值边界
#define DRAW_VALUE   20               // 和棋时返回的分数(取负值)

#define CHECK_LAZY   true    // 偷懒检测将军
#define CHECK_MULTI   48     // 被多个子将军

// 每种子力的类型编号
#define KING_TYPE   0
#define ADVISOR_TYPE   1
#define BISHOP_TYPE   2
#define KNIGHT_TYPE   3
#define ROOK_TYPE   4
#define CANNON_TYPE   5
#define PAWN_TYPE   6

// 每种子力的开始序号和结束序号
#define KING_FROM     0
#define ADVISOR_FROM 1
#define ADVISOR_TO   2
#define BISHOP_FROM   3
#define BISHOP_TO   4
#define KNIGHT_FROM   5
#define KNIGHT_TO   6
#define ROOK_FROM   7
#define ROOK_TO   8
#define CANNON_FROM   9
#define CANNON_TO   10
#define PAWN_FROM   11
#define PAWN_TO   15

// 各种子力的屏蔽位
#define KING_BITPIECE   (1 << KING_FROM)
#define ADVISOR_BITPIECE   ((1 << ADVISOR_FROM) | (1 << ADVISOR_TO))
#define BISHOP_BITPIECE   ((1 << BISHOP_FROM) | (1 << BISHOP_TO))
#define KNIGHT_BITPIECE   ((1 << KNIGHT_FROM) | (1 << KNIGHT_TO))
#define ROOK_BITPIECE   ((1 << ROOK_FROM) | (1 << ROOK_TO))
#define CANNON_BITPIECE   ((1 << CANNON_FROM) | (1 << CANNON_TO))
#define PAWN_BITPIECE   ((1 << PAWN_FROM) | (1 << (PAWN_FROM + 1)) | \
    (1 << (PAWN_FROM + 2)) | (1 << (PAWN_FROM + 3)) | (1 << PAWN_TO))
#define ATTACK_BITPIECE   (KNIGHT_BITPIECE | ROOK_BITPIECE | CANNON_BITPIECE | PAWN_BITPIECE)

// "RepStatus()"返回的重复标记位
#define REP_NONE   0
#define REP_DRAW   1
#define REP_LOSS   3
#define REP_WIN   5

#define DEL_PIECE  true // 函数"Position::AddPiece()"的选项

extern const int cnSimpleValues[48];      // 棋子的简单分值
extern const uint8_t cucsqMirrorTab[256]; // 坐标的镜像(左右对称)数组

// FEN串中棋子标识
int FenPiece(int Arg);
uint32_t MOVE_COORD(int mv);      // 把着法转换成字符串
int COORD_MOVE(uint32_t dwMoveStr); // 把字符串转换成着法
uint32_t WHITE_BITPIECE(int nBitPiece);
uint32_t BLACK_BITPIECE(int nBitPiece);

// 着法结构
int SRC(int mv); // 得到着法的起点
int DST(int mv); // 得到着法的终点
int MOVE(int sqSrc, int sqDst);  // 由起点和终点得到着法
char PIECE_BYTE(int pt);
int PIECE_TYPE(int pc);
//int SIDE_TAG(int sd);

/* ElephantEye的很多代码中都用到"SIDE_TAG()"这个量，红方设为16，黑方设为32。
 * 用"SIDE_TAG() + i"可以方便地选择棋子的类型，"i"从0到15依次是：
 * 帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 例如"i"取"KNIGHT_FROM"到"KNIGHT_TO"，则表示依次检查两个马的位置
 */
int SIDE_TAG(int sd);


// 复杂着法结构
typedef union _Move Move;

union _Move {
  uint32_t dwmv;           // 填满整个结构用
  struct {
    uint16_t wmv, wvl;     // 着法和分值
  };
  struct {
    uint8_t Src, Dst;      // 起始格和目标格
    int8_t CptDrw, ChkChs; // 被吃子(+)/和棋着法数(-)、将军子(+)/被捉子(-)
  };
}; // mvs

// 回滚结构
typedef struct _Rollback Rollback;

struct _Rollback {
  Zobrist zobr;   // Zobrist
  int vlWhite, vlBlack; // 红方和黑方的子力价值
  Move mvs;       // 着法
}; // rbs


// 局面结构
typedef struct _Position Position;

struct _Position {
  // 基本成员
  int sdPlayer;             // 轮到哪方走，0表示红方，1表示黑方
  uint8_t ucpcSquares[256]; // 每个格子放的棋子，0表示没有棋子
  uint8_t ucsqPieces[48];   // 每个棋子放的位置，0表示被吃
  Zobrist zobr;       // Zobrist

  // 位结构成员，用来增强棋盘的处理
  union {
    uint32_t dwBitPiece;    // 32位的棋子位，0到31位依次表示序号为16到47的棋子是否还在棋盘上
    uint16_t wBitPiece[2];  // 拆分成两个
  };
  uint16_t wBitRanks[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
  uint16_t wBitFiles[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"

  // 局面评价数据
  int vlWhite, vlBlack;   // 红方和黑方的子力价值

  // 回滚着法，用来检测循环局面
  int nMoveNum, nDistance;              // 回滚着法数和搜索深度
  Rollback rbsList[MAX_MOVE_NUM]; // 回滚列表
  uint8_t ucRepHash[REP_HASH_MASK + 1]; // 判断重复局面的迷你置换表
};

// 获得某个棋子对于本方视角的纵线优先坐标，棋子编号从0到15
//int FILESQ_SIDE_PIECE (Position *pos, int nPieceNum);

// 获取着法预生成信息
SlideMove *Position_RankMovePtr(Position* pos, int x, int y);
SlideMove *Position_FileMovePtr(Position* pos, int x, int y);
SlideMask *Position_RankMaskPtr(Position* pos, int x, int y);
SlideMask *Position_FileMaskPtr(Position* pos, int x, int y);

// 棋盘处理过程
void Position_ClearBoard(Position* pos);
 // 交换走棋方
void Position_ChangeSide(Position* pos);
void Position_SaveStatus(Position* pos);
void Position_Rollback(Position* pos);

//  棋盘上增加棋子
void Position_AddPiece (Position* pos, int sq, int pc, bool bDel /* ?=false */);
// 移动棋子
int Position_MovePiece (Position* pos, int mv);
// 撤消移动棋子
void Position_UndoMovePiece (Position* pos, int mv, int pcCaptured);
// 升变
int Position_Promote (Position* pos, int sq);
// 撤消升变
void Position_UndoPromote (Position* pos, int sq, int pcCaptured);

// 着法处理过程
// 执行一个着法
bool Position_MakeMove(Position *pos, int mv);
// 撤消一个着法
void Position_UndoMakeMove (Position *pos);
// 执行一个空着
void Position_NullMove (Position *pos);
// 撤消一个空着
void Position_UndoNullMove (Position* pos);

// 生成FEN串
void Position_ToFen (Position* pos, char *szFen);

// 着法生成过程
// 棋子保护判断
bool Position_Protected (Position* pos, int sd, int sqSrc, int sqExcept /* ?=0 */);

// 局面处理过程
// FEN串识别
void Position_FromFen (Position* pos, const char *szFen);
// 局面镜像
void Position_Mirror (Position* pos);
// 着法合理性检测，仅用在“杀手着法”的检测中
bool Position_LegalMove (Position* pos, int mv);
// 将军检测, 被哪个子将军
int Position_CheckedBy (Position* pos, bool bLazy /* ?=false */);
// 判断是否被将死 判断是已被将死
bool Position_IsMate (Position* pos);

// “捉”的检测 捉哪个子
int  Position_ChasedBy (Position* pos, int mv);
//  计算MVV(LVA)值
int Position_MvvLva (Position* pos, int sqDst, int pcCaptured, int nLva);
// 吃子着法生成器，按MVV(LVA)设定分值
int Position_GenCapMoves(Position* pos, Move *lpmvs);
// 不吃子着法生成器
int Position_GenNonCapMoves(Position* pos, Move *lpmvs);

// 重复局面检测
int Position_RepStatus (Position* pos, int nRecur /*?=1 */);

void Position_SetIrrev(Position* pos);
  // 着法检测过程
bool Position_GoodCap(Position* pos, int mv);
Move Position_LastMove(Position* pos);
bool Position_CanPromote(Position* pos);
bool Position_NullOkay(Position* pos);
bool Position_NullSafe(Position* pos);
bool Position_IsDraw(Position* pos);
int Position_DrawValue(Position* pos);
int Position_RepValue(Position* pos, int vlRep);
int Position_Material(Position* pos);
int Position_GenAllMoves(Position* pos, Move *lpmvs);

#endif
