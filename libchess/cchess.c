// SPDX-License-Identifier: LGPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "base.h"
#include "position.h"
#include "cchess.h"

// 以下常量规定了"TryMove()"的返回状态
#define MOVE_ILLEGAL   256        // 不合法的着法
#define MOVE_INCHECK   128        // 因将军而不合法的着法
#define MOVE_DRAW   64            // 和棋着法(仍被理解为合法的，下同)
#define MOVE_PERPETUAL_LOSS   32  // 长打的重复着法
#define MOVE_PERPETUAL_WIN   16   // 对方长打的重复着法
#define MOVE_PERPETUAL   8        // 重复三次的着法
#define MOVE_MATE   4             // 将死(包括困毙)
#define MOVE_CHECK   2            // 将军
#define MOVE_CAPTURE   1          // 吃子

/* 以下常量规定了着法表示使用的数字、棋子、方向(进平退)、位置(前后)等的最大个数。
 *
 * 表示位置的符号共有8个，除了“前中后”以外还有“一二三四五”，参考
 * 《中国象棋电脑应用规范(二)：着法表示》(简称《规范》)，即以下网页：
 * 　　http://www.elephantbase.net/protocol/cchess_move.htm
 * 由于“前中后”被安排在“一二三四五”以后，但又和“进平退”在符号上一致，因此要加减"DIRECT_TO_POS"作转换。
 * 另外，由于仕(士)相(象)的着法表示的纵线形式和坐标形式有一一对应的关系(固定纵线表示)，
 * 因此可以使用数组"cdwFixFile"和"cucFixMove"对两者进行转换，总共有28种对应关系。
 */
const int MAX_DIGIT     = 9;
const int MAX_PIECE     = 7;
const int MAX_DIRECT    = 3;
const int MAX_POS       = 8;  // 最大位置
const int DIRECT_TO_POS = 5;
const int MAX_FIX_FILE = 28;

const gchar* piece_word[2] = {"帅仕相马车炮兵", "将士象马车炮卒"};
//红方用汉字文字, 黑方用全角数字,也支持黑方用半角数字
gchar* digit_word[3] = {"一二三四五六七八九", "１２３４５６７８９", "123456789"};
gchar* direct_word = "进平退進";
gchar* posit_word = "一二三四五前中后後";

// 固定纵线表示的纵线数组
static const uint32_t cdwFixFile[28] = {
  /* 士的8种着法 */
  0x352d3441 /*A4-5*/, 0x352b3441 /*A4+5*/, 0x342d3541 /*A5-4*/, 0x342b3541 /*A5+4*/,
  0x362d3541 /*A5-6*/, 0x362b3541 /*A5+6*/, 0x352d3641 /*A6-5*/, 0x352b3641 /*A6+5*/,

  /* 象的16种着法 */
  0x332d3142 /*B1-3*/, 0x332b3142 /*B1+3*/, 0x312d3342 /*B3-1*/, 0x312b3342 /*B3+1*/,
  0x352d3342 /*B3-5*/, 0x352b3342 /*B3+5*/, 0x332d3542 /*B5-3*/, 0x332b3542 /*B5+3*/,
  0x372d3542 /*B5-7*/, 0x372b3542 /*B5+7*/, 0x352d3742 /*B7-5*/, 0x352b3742 /*B7+5*/,
  0x392d3742 /*B7-9*/, 0x392b3742 /*B7+9*/, 0x372d3942 /*B9-7*/, 0x372b3942 /*B9+7*/,

  /* 士、象升兵的4种着法 */
  0x503d3441 /*A4=P*/, 0x503d3641 /*A6=P*/, 0x503d3342 /*B3=P*/, 0x503d3742 /*B7=P*/
};

// 固定纵线表示的坐标数组
static const uint8_t cucFixMove[28][2] = {
  { 0xa8, 0xb7 }, { 0xc8, 0xb7 }, { 0xb7, 0xc8 }, { 0xb7, 0xa8 }, { 0xb7, 0xc6 }, { 0xb7, 0xa6 }, { 0xa6, 0xb7 }, { 0xc6, 0xb7 }, { 0xab, 0xc9 }, { 0xab, 0x89 }, { 0x89, 0xab }, { 0xc9, 0xab }, { 0x89, 0xa7 }, { 0xc9, 0xa7 }, { 0xa7, 0xc9 }, { 0xa7, 0x89 }, { 0xa7, 0xc5 }, { 0xa7, 0x85 }, { 0x85, 0xa7 }, { 0xc5, 0xa7 }, { 0x85, 0xa3 }, { 0xc5, 0xa3 }, { 0xa3, 0xc5 }, { 0xa3, 0x85 }, { 0xc8, 0xc8 }, { 0xc6, 0xc6 }, { 0xc9, 0xc9 }, { 0xc5, 0xc5 }
};

/* 以下两个数组实现了内部棋盘坐标(Square)和纵线优先坐标(FileSq)的转换。
 *
 * 内部棋盘坐标是有3层边界的16x16冗余数组(参阅"pregen.cpp")，为方便转换成纵线格式，
 * 要对它们重新编号，即按列优先从右到左，相同的列再从前到后的顺序(参阅《规范》)。
 * 转换后的坐标仍然是16x16的冗余数组，整除16后就是列号(右边线是0)，对16取余就是行号(上边线是0)。
 */

static const uint8_t cucSquare2FileSq[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00, 0, 0, 0, 0,
  0, 0, 0, 0x81, 0x71, 0x61, 0x51, 0x41, 0x31, 0x21, 0x11, 0x01, 0, 0, 0, 0,
  0, 0, 0, 0x82, 0x72, 0x62, 0x52, 0x42, 0x32, 0x22, 0x12, 0x02, 0, 0, 0, 0,
  0, 0, 0, 0x83, 0x73, 0x63, 0x53, 0x43, 0x33, 0x23, 0x13, 0x03, 0, 0, 0, 0,
  0, 0, 0, 0x84, 0x74, 0x64, 0x54, 0x44, 0x34, 0x24, 0x14, 0x04, 0, 0, 0, 0,
  0, 0, 0, 0x85, 0x75, 0x65, 0x55, 0x45, 0x35, 0x25, 0x15, 0x05, 0, 0, 0, 0,
  0, 0, 0, 0x86, 0x76, 0x66, 0x56, 0x46, 0x36, 0x26, 0x16, 0x06, 0, 0, 0, 0,
  0, 0, 0, 0x87, 0x77, 0x67, 0x57, 0x47, 0x37, 0x27, 0x17, 0x07, 0, 0, 0, 0,
  0, 0, 0, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38, 0x28, 0x18, 0x08, 0, 0, 0, 0,
  0, 0, 0, 0x89, 0x79, 0x69, 0x59, 0x49, 0x39, 0x29, 0x19, 0x09, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t cucFileSq2Square[256] = {
  0x3b, 0x4b, 0x5b, 0x6b, 0x7b, 0x8b, 0x9b, 0xab, 0xbb, 0xcb, 0, 0, 0, 0, 0, 0,
  0x3a, 0x4a, 0x5a, 0x6a, 0x7a, 0x8a, 0x9a, 0xaa, 0xba, 0xca, 0, 0, 0, 0, 0, 0,
  0x39, 0x49, 0x59, 0x69, 0x79, 0x89, 0x99, 0xa9, 0xb9, 0xc9, 0, 0, 0, 0, 0, 0,
  0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xa8, 0xb8, 0xc8, 0, 0, 0, 0, 0, 0,
  0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xa7, 0xb7, 0xc7, 0, 0, 0, 0, 0, 0,
  0x36, 0x46, 0x56, 0x66, 0x76, 0x86, 0x96, 0xa6, 0xb6, 0xc6, 0, 0, 0, 0, 0, 0,
  0x35, 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xa5, 0xb5, 0xc5, 0, 0, 0, 0, 0, 0,
  0x34, 0x44, 0x54, 0x64, 0x74, 0x84, 0x94, 0xa4, 0xb4, 0xc4, 0, 0, 0, 0, 0, 0,
  0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xa3, 0xb3, 0xc3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

enum {
  LAST_SIGNAL
};

enum {
  PROP_0,
  NUM_PROPERTIES
};

extern PreEval preEval;

struct _CChess
{
  GObject   object;
  Position  pos;
  gboolean  board_flipped;
};

// 着法符号
typedef union _Action Action;
union _Action {
  gchar c[4];
  guint32 code;
};

G_DEFINE_TYPE (CChess, cchess, G_TYPE_OBJECT);

static void cchess_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  CChess *chess;

  chess = CHINESE_CHESS (object);

  switch (prop_id)
  {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void cchess_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  CChess *chess;

  chess = CHINESE_CHESS (object);

  switch (prop_id)
  {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void cchess_class_init (CChessClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = cchess_set_property;
  object_class->get_property = cchess_get_property;
}

static void cchess_init (CChess *chess)
{
  PreGenInit ();
  chess->board_flipped = FALSE;
}

CChess* cchess_new (void)
{
  return g_object_new (CHINESE_TYPE_CHESS, NULL);
}

void cchess_start_board (CChess *chess)
{
  Position_FromFen (&chess->pos, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
}

void cchess_fen_to_board (CChess *chess, const gchar* fen)
{
  Position_FromFen (&chess->pos, fen);
}

gchar* cchess_board_to_fen (CChess *chess)
{
  gchar *fen = NULL;

  fen = g_new0 (char, 128);
  Position_ToFen (&chess->pos, fen);   // 生成FEN串
  return fen;
}

void cchess_clear_board (CChess *chess)
{
  Position_ClearBoard (&chess->pos);
}

// 局面镜像
void cchess_board_mirror (CChess *chess)
{
  Position_Mirror (&chess->pos);
}

// 红黑互换, 交换走棋方, 红先变成黑先
void cchess_exchange_side (CChess *chess)
{
  Position *pos = &chess->pos;
  int i, sq;
  uint8_t ucsqList[32];
  for (i = 16; i < 48; i++)
  {
    sq = pos->ucsqPieces[i];
    ucsqList[i - 16] = sq;
    if (sq != 0)
    {
      Position_AddPiece (pos, sq, i, DEL_PIECE);
    }
  }
  for (i = 16; i < 48; i++)
  {
    sq = ucsqList[i < 32 ? i : i - 32]; // 这行不同于FlipBoard
    if (sq != 0)
    {
      Position_AddPiece (pos, SQUARE_FLIP (sq), i, false);
    }
  }
  Position_ChangeSide (pos); // 这行不同于FlipBoard
}

// 翻转棋盘
void cchess_flip_board (CChess *chess)
{
  Position *pos = &chess->pos;
  int i, sq;
  uint8_t ucsqList[32];
  for (i = 16; i < 48; i++)
  {
    sq = pos->ucsqPieces[i];
    ucsqList[i - 16] = sq;
    if (sq != 0)
    {
      Position_AddPiece (pos, sq, i, DEL_PIECE);
    }
  }
  for (i = 16; i < 48; i++)
  {
    sq = ucsqList[i - 16]; // 这行不同于ExchangeSide
    if (sq != 0)
    {
      Position_AddPiece (pos, SQUARE_FLIP (sq), i, false);
    }
  }
  chess->board_flipped = TRUE;
}

// 文本棋盘的棋盘字符
static const char *board_string[10] = {
"＋ㅜㅜㅜㅜㅜㅜㅜ＋",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"ㅏㅗㅗㅗㅗㅗㅗㅗㅓ",
"ㅏㅜㅜㅜㅜㅜㅜㅜㅓ",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"ㅏ＋＋＋＋＋＋＋ㅓ",
"＋ㅗㅗㅗㅗㅗㅗㅗ＋"
};

gunichar g_utf8_get_nth_unichar(const gchar* p, gint n)
{
  gchar *pos;
  gunichar c;

  pos = g_utf8_offset_to_pointer (p, n);
  c = g_utf8_get_char (pos);
  return c;
}

static gchar* g_utf8_get_nth_char(const gchar* p, gint n)
{
  gunichar c;
  GString* string = NULL;

  c = g_utf8_get_nth_unichar(p, n);

  string = g_string_new (NULL);
  string = g_string_append_unichar(string, c);
  return g_string_free(string, FALSE);
}

// 生成文本棋盘
gchar* cchess_board_text (CChess *chess)
{
  gint i, j, pc;
  GString* string = NULL;

  string = g_string_new ("１２３４５６７８９\r\n");
  for (i = 0; i < 10; i++)
  {
    for (j = FILE_LEFT; j <= FILE_RIGHT; j++)
    {
      pc = chess->pos.ucpcSquares[COORD_XY (j, i + RANK_TOP)];
      if ((pc & SIDE_TAG (0)) != 0) {
        gchar* c = g_utf8_get_nth_char(piece_word[0], PIECE_TYPE (pc));
        g_string_append_printf(string, "\33[1;31m%s\33[0m", c);
        g_free(c);
      } else if ((pc & SIDE_TAG (1)) != 0) {
        gchar* c = g_utf8_get_nth_char(piece_word[1], PIECE_TYPE (pc));
        g_string_append_printf(string, "\33[1;35m%s\33[0m", c);
        g_free(c);
      } else {
        gchar* c = g_utf8_get_nth_char(board_string[i],(j - FILE_LEFT));
        string = g_string_append(string, c);
        g_free(c);
      }
    }
    string = g_string_append(string, "\r\n");
  }
  string = g_string_append(string, "九八七六五四三二一\r\n");
  return g_string_free(string, FALSE);
}

// 对FEN串作镜像(只要识别行分隔符"/"，行内字符串顺序颠倒即可)
gchar* cchess_fen_mirror (const gchar* fen)
{
  int i, j;
  const char *p;
  char *q, *limit;
  char buf[128];
  char new_fen[128];

  p = fen;
  q = new_fen;

  limit = q + 127;
  if (*p == '\0')
  {
    *q = '\0';
    return NULL;
  }
  while (*p == ' ')
  {
    p++;
    if (*p == '\0')
    {
      *q = '\0';
      return NULL;
    }
  }

  i = 0;
  while (q < limit && i < 127)
  {
    if (*p == '/' || *p == ' ' || *p == '\0')
    {
      for (j = 0; j < i; j++)
      {
        *q = buf[i - j - 1];
        q++;
        if (q == limit)
        {
          break;
        }
      }
      i = 0;
      if (*p == '/')
      {
        *q = '/';
        q++;
      } else {
        break;
      }
    } else {
      buf[i] = *p;
      i++;
    }
    p++;
  };
  while (*p != '\0' && q < limit)
  {
    *q = *p;
    p++;
    q++;
  }
  *q = '\0';
  return g_strdup(new_fen);
}

/* 以下函数实现了数字、棋子、方向和位置的编码和符号、编码和汉字之间的转换
 *
 * 部分符号编码转换的代码，利用了"position.cpp"中的"PIECE_BYTE"数组和"FenPiece()"函数。
 * 从汉字转换为编码是难点，无论处于简体状态还是繁体状态，转换时既考虑了简体、繁体和异体，也考虑了GBK码和BIG5码，
 * 因此除了依次比较汉字数组外，还增加了对GBK码繁体字和异体字的识别。
 */

static int Digit2Byte (int nArg)
{
  return nArg + '1';
}

/*
 * + => 0
 * . => 1
 * = => 1
 * - => 2
 * ? => 3
 */
static gint Byte2Direct (gchar nArg)
{
  return (nArg == '+' ? 0 :
  nArg == '.' || nArg == '=' ? 1 :
  nArg == '-' ? 2 :
  3);
}

static int Byte2Piece (int nArg)
{
  return (nArg >= '1' && nArg <= '7' ? nArg - '1' :
  nArg >= 'A' && nArg <= 'Z' ? FenPiece (nArg) :
  nArg >= 'a' && nArg <= 'z'   ? FenPiece (nArg - 'a' + 'A') :
  MAX_PIECE);
}

static int Byte2Digit (int nArg)
{
  return (nArg >= '1' && nArg <= '9' ? nArg - '1' : MAX_DIGIT);
}

/* 函数"FileMirror()"对着法的纵线表示作镜像。
 *
 * 纵线的符号表示基本类似于汉字表示，但当出现类似“前炮退二”这样的表示时，符号表示就会有不同的情况。
 * 按照《规范》的建议，表示成"C+-2"最容易被识别，但是也有表示成"+C-2"的，即符号和汉字完全对应，因此本函数也会考虑这种形式。
 * 对一般着法而言，纵线表示的镜像是唯一的，但是对于“两条的纵线上有多个兵(卒)”的罕见情况，
 * 本函数只能考虑最不罕见的一种特例，即两条纵线上各有两个兵(卒)，这样，"Paxx"和"Pbxx"分别跟"Pcxx"和"Pdxx"镜像，
 * 而对于其他情况则无法作出正确转换。
 * 注意：符号表示由4个字节构成，所以可以用一个"uint32_t"类型作快速传输(同理，汉字表示用"uint64_t")。
 */
guint32 cchess_file_mirror (guint32 filestr)
{
  int nPos, nFile, pt;
  Action Ret;
  Ret.code = filestr;

  nPos = Byte2Direct (Ret.c[0]);
  if (nPos == MAX_DIRECT)
  {
    pt = Byte2Piece (Ret.c[0]);
    nFile = Byte2Digit (Ret.c[1]);
    if (nFile == MAX_DIGIT)
    {
      switch (Ret.c[1])
      {
        case 'a':
          Ret.c[1] = 'c';
          break;
        case 'b':
          Ret.c[1] = 'd';
          break;
        case 'c':
          Ret.c[1] = 'a';
          break;
        case 'd':
          Ret.c[1] = 'b';
          break;
        default:
          break;
      }
    } else {
      Ret.c[1] = Digit2Byte (8 - nFile);
    }
  } else {
    pt = Byte2Piece (Ret.c[1]);
  }
  if ((pt >= ADVISOR_TYPE && pt <= KNIGHT_TYPE) || Byte2Direct (Ret.c[2]) == 1)
  {
    Ret.c[3] = Digit2Byte (8 - Byte2Digit (Ret.c[3]));
  }
  return Ret.code;
}

// 把局面设成“不可逆”
void cchess_set_irrev (CChess *chess)
{
  Position_SetIrrev (&chess->pos);
}

void cchess_promotion (CChess *chess, gboolean promotion)
{
  preEval.bPromotion = promotion != FALSE;
}

gboolean cchess_can_promote (CChess *chess, long sq)
{
  int pt;
  Position *pos = &chess->pos;

  if (preEval.bPromotion && Position_CanPromote (pos) && CAN_PROMOTE (sq))
  {
    pt = PIECE_TYPE (pos->ucpcSquares[sq]);
    return pt == ADVISOR_TYPE || pt == BISHOP_TYPE;
  }
  return FALSE;
}

void cchess_add_piece (CChess *chess, long sq, long pc, gboolean delete)
{
  Position_AddPiece (&chess->pos, sq, pc, !delete);
}

// 尝试某个着法，并返回着法状态，参阅"cchess.h"
gboolean cchess_try_move (CChess *chess, int* nStatus, int mv)
{
  Position *pos = &chess->pos;
  if (!Position_LegalMove (pos, mv))
  {
    *nStatus = MOVE_ILLEGAL;
    return false;
  }
  if (!Position_MakeMove (pos, mv))
  {
    *nStatus = MOVE_INCHECK;
    return false;
  }
  nStatus = 0;
  nStatus += (Position_LastMove (pos).CptDrw > 0 ? MOVE_CAPTURE : 0);
  nStatus += (Position_LastMove (pos).ChkChs > 0 ? MOVE_CHECK : 0);
  nStatus += (Position_IsMate (pos) ? MOVE_MATE : 0);
  nStatus += Position_RepStatus (pos, 3) * MOVE_PERPETUAL; // 提示：参阅"position.cpp"中的"IsRep()"函数
  nStatus += (Position_IsDraw (pos) ? MOVE_DRAW : 0);
  return true;
}

void cchess_undo_move (CChess *chess)
{
  Position_UndoMakeMove (&chess->pos);
}

// 生成全部合理着法
glong cchess_gen_moves (CChess *chess, glong* lpmv)
{
  int i, nTotal, nLegal;
  Move mvs[MAX_GEN_MOVES];
  Position *pos = &chess->pos;

  nTotal = Position_GenAllMoves(pos, mvs);
  nLegal = 0;
  for (i = 0; i < nTotal; i++)
  {
    if (Position_MakeMove (pos, mvs[i].wmv))
    {
      Position_UndoMakeMove (pos);
      lpmv[nLegal] = mvs[i].wmv;
      nLegal++;
    }
  }
  return nLegal;
}

static gint Word2Pos (gunichar ch)
{
  int i;

  if (ch == g_utf8_get_nth_unichar(posit_word, 7) || ch == g_utf8_get_nth_unichar(posit_word, 8))
  {
    return 2 + DIRECT_TO_POS;
  } else {
    for (i = 0; i < MAX_POS; i++)
    {
      if (ch == g_utf8_get_nth_unichar(posit_word, i))
      {
        break;
      }
    }
    return i;
  }
}

static int Word2Piece (gunichar ch)
{
  int i;

  gchar* text = "帥將馬傌車硨俥包砲";

  if (ch == g_utf8_get_nth_unichar(text, 0) || ch == g_utf8_get_nth_unichar(text, 1)) {
    return 0;
  } else if (ch == g_utf8_get_nth_unichar(text, 2) || ch == g_utf8_get_nth_unichar(text, 3)) {
    return 3;
  } else if (ch == g_utf8_get_nth_unichar(text, 4) || ch == g_utf8_get_nth_unichar(text, 5) ||
    ch == g_utf8_get_nth_unichar(text, 6)) {
    return 4;
  } else if (ch == g_utf8_get_nth_unichar(text, 7) || ch == g_utf8_get_nth_unichar(text, 8)) {
    return 5;
  } else {
    for (i = 0; i < MAX_PIECE; i++)
    {
      if (ch == g_utf8_get_nth_unichar(piece_word[0], i) || ch == g_utf8_get_nth_unichar(piece_word[1], i)) {
        break;
      }
    }
    return i;
  }
}

static gint Word2Digit (gunichar ch)
{
  gint i;

  for (i = 0; i < MAX_DIGIT; i++)
  {
    if (ch == g_utf8_get_nth_unichar(digit_word[0], i) || ch == g_utf8_get_nth_unichar(digit_word[1], i) || ch == g_utf8_get_nth_unichar(digit_word[2], i)) {
      break;
    }
  }
  return i;
}

static int Word2Direct (gunichar ch)
{
  int i;
  gchar* text="进平退進";
  if (ch == g_utf8_get_nth_unichar(text, 3))
  {
    return 0;
  } else {
    for (i = 0; i < 3; i++)
    {
      if (ch == g_utf8_get_nth_unichar(text, i))
      {
        break;
      }
    }
    return i;
  }
}

static const char ccPos2Byte[12] = {
  'a', 'b', 'c', 'd', 'e', '+', '.', '-', ' ', ' ', ' ', ' '
};

static const char ccDirect2Byte[4] = {
  '+', '.', '-', ' '
};

// 将汉字表示转换为着法符号表示
// 如: 炮二平五 => C2.5
// 士四变兵 => A4=P
guint32 cchess_chinese_to_action (const gchar* utf8str)
{
  int nPos;
  Action Ret;
  gchar* x_to_pown = "变跑變";

  gunichar ch0 = g_utf8_get_nth_unichar(utf8str, 0);
  gunichar ch1 = g_utf8_get_nth_unichar(utf8str, 1);
  gunichar ch2 = g_utf8_get_nth_unichar(utf8str, 2);
  gunichar ch3 = g_utf8_get_nth_unichar(utf8str, 3);

  nPos = Word2Pos (ch0);

  Ret.c[0] = PIECE_BYTE (Word2Piece (nPos == MAX_POS ? ch0 : ch1));
  Ret.c[1] = (nPos == MAX_POS ? Digit2Byte (Word2Digit (ch1)) : ccPos2Byte[nPos]);

  if ((ch2 == g_utf8_get_nth_unichar(x_to_pown, 0) || ch2 == g_utf8_get_nth_unichar(x_to_pown, 1) || ch2 == g_utf8_get_nth_unichar(x_to_pown, 2)) &&
    Word2Piece(ch3) == 6)
  {
    Ret.c[2] = '=';
    Ret.c[3] = 'P';
  } else {
    Ret.c[2] = ccDirect2Byte[Word2Direct (ch2)];
    Ret.c[3] = Digit2Byte (Word2Digit (ch3));
  }
  return Ret.code;
}

int Byte2Pos (gchar nArg)
{
  return (nArg >= 'a' && nArg <= 'e' ? nArg - 'a'
  : Byte2Direct (nArg) + DIRECT_TO_POS);
}

// 将着法符号表示转换为汉字表示
gchar* cchess_action_to_chinese (guint32 action, int player)
{
  int nPos;
  char *lpArg;
  gunichar w[4];
  GString* string = NULL;
  gchar* promote = "跑变";

  lpArg = (char *) &action;
  nPos = Byte2Direct (lpArg[0]);
  if (nPos == MAX_DIRECT)
  {
    nPos = Byte2Pos (lpArg[1]);
    w[0] = (nPos == MAX_POS ? g_utf8_get_nth_unichar(piece_word[player], Byte2Piece(lpArg[0])) : g_utf8_get_nth_unichar(posit_word, nPos));
    w[1] = (nPos == MAX_POS ? g_utf8_get_nth_unichar(digit_word[player], Byte2Digit(lpArg[1])) : g_utf8_get_nth_unichar(piece_word[player], Byte2Piece(lpArg[0])));
  } else {
    w[0] = g_utf8_get_nth_unichar(posit_word, nPos + DIRECT_TO_POS);
    w[1] = g_utf8_get_nth_unichar(piece_word[player], Byte2Piece (lpArg[1]));
  }
  if (lpArg[2] == '=' && Byte2Piece (lpArg[3]) == 6)
  {
    w[2] = g_utf8_get_nth_unichar(promote, player);
    w[3] = g_utf8_get_nth_unichar(piece_word[player], 6);
  } else {
    w[2] = g_utf8_get_nth_unichar(direct_word, Byte2Direct (lpArg[2]));
    w[3] = g_utf8_get_nth_unichar(digit_word[player],Byte2Digit(lpArg[3]));
  }

  string = g_string_new (NULL);
  string = g_string_append_unichar(string, w[0]);
  string = g_string_append_unichar(string, w[1]);
  string = g_string_append_unichar(string, w[2]);
  string = g_string_append_unichar(string, w[3]);
  return g_string_free(string, FALSE);
}

static void swap_int(int a, int b);
static void swap_int(int a, int b)
{
  int temp;
  temp = a;
  a = b;
  b = temp;
}

static uint8_t FILESQ_SQUARE (int sq)
{
  return cucFileSq2Square[sq];
}

static uint8_t SQUARE_FILESQ (int sq)
{
  return cucSquare2FileSq[sq];
}

// 获得某个棋子对于本方视角的纵线优先坐标，棋子编号从0到15
static int FILESQ_SIDE_PIECE (Position *pos, int nPieceNum)
{
  int sq;
  sq = pos->ucsqPieces[SIDE_TAG (pos->sdPlayer) + nPieceNum];
  return (sq == 0 ? -1 : pos->sdPlayer == 0 ? SQUARE_FILESQ (sq)
  : SQUARE_FILESQ (SQUARE_FLIP (sq)));
}

static int FILESQ_RANK_Y (int sq)
{
  return sq & 15;
}

static int FILESQ_FILE_X (int sq)
{
  return sq >> 4;
}

static int FILESQ_COORD_XY (int x, int y)
{
  return (x << 4) + y;
}

// 根据子力类型获得棋子的编号
static int FIRST_PIECE (int pt, int pc)
{
  return pt * 2 - 1 + pc;
}

/* "File2Move()"函数将纵线符号表示转换为内部着法表示。
 *
 * 这个函数以及后面的"Move2Action()"函数是本模块最难处理的两个函数，特别是在处理“两条的纵线上有多个兵(卒)”的问题上。
 * 在棋谱的快速时，允许只使用数字键盘，因此1到7依次代表帅(将)到兵(卒)这七种棋子，"File2Move()"函数也考虑到了这个问题。
 * C2.5 =>
 */
int cchess_action_to_move (CChess *chess, guint32 action)
{
  int i, j, nPos, pt, sq, nPieceNum;
  int xSrc, ySrc, xDst, yDst;
  Action FileStr;
  int nFileList[9], nPieceList[5];
  Position *pos = &chess->pos;
  // 纵线符号表示转换为内部着法表示，通常分为以下几个步骤：

  // 1. 检查纵线符号是否是仕(士)相(象)的28种固定纵线表示，在这之前首先必须把数字、小写等不统一的格式转换为统一格式；
  FileStr.code = action;
  switch (FileStr.c[0])
  {
    case '2':
    case 'a':
      FileStr.c[0] = 'A';
      break;
    case '3':
    case 'b':
    case 'E':
    case 'e':
      FileStr.c[0] = 'B';
      break;
    default:
      break;
  }
  if (FileStr.c[3] == 'p')
  {
    FileStr.c[3] = 'P';
  }
  for (i = 0; i < MAX_FIX_FILE; i++)
  {
    if (FileStr.code == cdwFixFile[i])
    {
      if (pos->sdPlayer == 0)
      {
        return MOVE (cucFixMove[i][0], cucFixMove[i][1]);
      } else {
        return MOVE (SQUARE_FLIP (cucFixMove[i][0]), SQUARE_FLIP (cucFixMove[i][1]));
      }
    }
  }

  // 2. 如果不是这28种固定纵线表示，那么把棋子、位置和纵线序号(列号)解析出来
  nPos = Byte2Direct (FileStr.c[0]);
  if (nPos == MAX_DIRECT)
  {
    pt = Byte2Piece (FileStr.c[0]);
    nPos = Byte2Pos (FileStr.c[1]);
  } else {
    pt = Byte2Piece (FileStr.c[1]);
    nPos += DIRECT_TO_POS;
  }
  if (nPos == MAX_POS)
  {

    // 3. 如果棋子是用列号表示的，那么可以直接根据纵线来找到棋子序号；
    xSrc = Byte2Digit (FileStr.c[1]);
    if (pt == KING_TYPE) {
      sq = FILESQ_SIDE_PIECE (pos, 0);
    } else if (pt >= KNIGHT_TYPE && pt <= PAWN_TYPE) {
      j = (pt == PAWN_TYPE ? 5 : 2);
      for (i = 0; i < j; i++)
      {
        sq = FILESQ_SIDE_PIECE (pos, FIRST_PIECE (pt, i));
        if (sq != -1)
        {
          if (FILESQ_FILE_X (sq) == xSrc)
          {
            break;
          }
        }
      }
      sq = (i == j ? -1 : sq);
    } else {
      sq = -1;
    }
  } else {

    // 4. 如果棋子是用位置表示的，那么必须挑选出含有多个该种棋子的所有纵线，这是本函数最难处理的地方；
    if (pt >= KNIGHT_TYPE && pt <= PAWN_TYPE)
    {
      for (i = 0; i < 9; i++)
      {
        nFileList[i] = 0;
      }
      j = (pt == PAWN_TYPE ? 5 : 2);
      for (i = 0; i < j; i++)
      {
        sq = FILESQ_SIDE_PIECE (pos, FIRST_PIECE (pt, i));
        if (sq != -1)
        {
          nFileList[FILESQ_FILE_X (sq)]++;
        }
      }
      nPieceNum = 0;
      for (i = 0; i < j; i++)
      {
        sq = FILESQ_SIDE_PIECE (pos, FIRST_PIECE (pt, i));
        if (sq != -1)
        {
          if (nFileList[FILESQ_FILE_X (sq)] > 1)
          {
            nPieceList[nPieceNum] = FIRST_PIECE (pt, i);
            nPieceNum++;
          }
        }
      }

      // 5. 找到这些纵线以后，对这些纵线上的棋子进行排序，然后根据位置来确定棋子序号；
      for (i = 0; i < nPieceNum - 1; i++)
      {
        for (j = nPieceNum - 1; j > i; j--)
        {
          if (FILESQ_SIDE_PIECE (pos, nPieceList[j - 1]) > FILESQ_SIDE_PIECE (pos, nPieceList[j]))
          {
            swap_int (nPieceList[j - 1], nPieceList[j]);
          }
        }
      }
      // 提示：如果只有两个棋子，那么“后”表示第二个棋子，如果有多个棋子，
      // 那么“一二三四五”依次代表第一个到第五个棋子，“前中后”依次代表第一个到第三个棋子。
      if (nPieceNum == 2 && nPos == 2 + DIRECT_TO_POS)
      {
        sq = FILESQ_SIDE_PIECE (pos, nPieceList[1]);
      } else {
        nPos -= (nPos >= DIRECT_TO_POS ? DIRECT_TO_POS : 0);
        sq = (nPos >= nPieceNum ? -1 : FILESQ_SIDE_PIECE (pos, nPieceList[nPos]));
      }
    } else {
      sq = -1;
    }
  }
  if (sq == -1)
  {
    return 0;
  }

  // 6. 现在已知了着法的起点，就可以根据纵线表示的后两个符号来确定着法的终点；
  xSrc = FILESQ_FILE_X (sq);
  ySrc = FILESQ_RANK_Y (sq);
  if (pt == KNIGHT_TYPE)
  {
    // 提示：马的进退处理比较特殊。
    xDst = Byte2Digit (FileStr.c[3]);
    if (FileStr.c[2] == '+')
    {
      yDst = ySrc - 3 + ABS (xDst - xSrc);
    } else {
      yDst = ySrc + 3 - ABS (xDst - xSrc);
    }
  } else {
    if (FileStr.c[2] == '+')
    {
      xDst = xSrc;
      yDst = ySrc - Byte2Digit (FileStr.c[3]) - 1;
    } else if (FileStr.c[2] == '-')
    {
      xDst = xSrc;
      yDst = ySrc + Byte2Digit (FileStr.c[3]) + 1;
    } else {
      xDst = Byte2Digit (FileStr.c[3]);
      yDst = ySrc;
    }
  }
  // 注意：yDst有可能超过范围！
  if (yDst < 0 || yDst > 9)
  {
    return 0;
  }

  // 7. 把相对走子方的坐标转换为固定坐标，得到着法的起点和终点。
  if (pos->sdPlayer == 0)
  {
    return MOVE (FILESQ_SQUARE (FILESQ_COORD_XY (xSrc, ySrc)), FILESQ_SQUARE (FILESQ_COORD_XY (xDst, yDst)));
  }
  return MOVE (SQUARE_FLIP (FILESQ_SQUARE (FILESQ_COORD_XY (xSrc, ySrc))),
               SQUARE_FLIP (FILESQ_SQUARE (FILESQ_COORD_XY (xDst, yDst))));
}

// 将内部着法表示转换为纵线符号
guint32 cchess_move_to_action (CChess *chess, int mv)
{
  Position *pos = &chess->pos;
  {
    int i, j, sq, pc, pt, nPieceNum;
    int xSrc, ySrc, xDst, yDst;
    int nFileList[9], nPieceList[5];
    Action Ret;

    if (SRC (mv) == 0 || DST (mv) == 0)
    {
      return 0x20202020;
    }
    pc = pos->ucpcSquares[SRC (mv)];
    if (pc == 0)
    {
      return 0x20202020;
    }
    pt = PIECE_TYPE (pc);
    Ret.c[0] = PIECE_BYTE (pt);
    if (pos->sdPlayer == 0)
    {
      xSrc = FILESQ_FILE_X (SQUARE_FILESQ (SRC (mv)));
      ySrc = FILESQ_RANK_Y (SQUARE_FILESQ (SRC (mv)));
      xDst = FILESQ_FILE_X (SQUARE_FILESQ (DST (mv)));
      yDst = FILESQ_RANK_Y (SQUARE_FILESQ (DST (mv)));
    } else {
      xSrc = FILESQ_FILE_X (SQUARE_FILESQ (SQUARE_FLIP (SRC (mv))));
      ySrc = FILESQ_RANK_Y (SQUARE_FILESQ (SQUARE_FLIP (SRC (mv))));
      xDst = FILESQ_FILE_X (SQUARE_FILESQ (SQUARE_FLIP (DST (mv))));
      yDst = FILESQ_RANK_Y (SQUARE_FILESQ (SQUARE_FLIP (DST (mv))));
    }
    if (pt >= KING_TYPE && pt <= BISHOP_TYPE)
    {
      Ret.c[1] = Digit2Byte (xSrc);
    } else {
      for (i = 0; i < 9; i++)
      {
        nFileList[i] = 0;
      }
      j = (pt == PAWN_TYPE ? 5 : 2);
      for (i = 0; i < j; i++)
      {
        sq = FILESQ_SIDE_PIECE (pos, FIRST_PIECE (pt, i));
        if (sq != -1)
        {
          nFileList[FILESQ_FILE_X (sq)]++;
        }
      }
      // 提示：处理“两条的纵线上有多个兵(卒)”的问题上，可参阅"File2Move()"函数。
      if (nFileList[xSrc] > 1)
      {
        nPieceNum = 0;
        for (i = 0; i < j; i++)
        {
          sq = FILESQ_SIDE_PIECE (pos, FIRST_PIECE (pt, i));
          if (sq != -1)
          {
            if (nFileList[FILESQ_FILE_X (sq)] > 1)
            {
              nPieceList[nPieceNum] = FIRST_PIECE (pt, i);
              nPieceNum++;
            }
          }
        }
        for (i = 0; i < nPieceNum - 1; i++)
        {
          for (j = nPieceNum - 1; j > i; j--)
          {
            if (FILESQ_SIDE_PIECE (pos, nPieceList[j - 1]) > FILESQ_SIDE_PIECE (pos, nPieceList[j]))
            {
              swap_int (nPieceList[j - 1], nPieceList[j]);
            }
          }
        }
        sq = FILESQ_COORD_XY (xSrc, ySrc);
        for (i = 0; i < nPieceNum; i++)
        {
          if (FILESQ_SIDE_PIECE (pos, nPieceList[i]) == sq)
          {
            break;
          }
        }
        Ret.c[1] = (nPieceNum == 2 && i == 1 ? ccPos2Byte[2 + DIRECT_TO_POS] : ccPos2Byte[nPieceNum > 3 ? i : i + DIRECT_TO_POS]);
      } else {
        Ret.c[1] = Digit2Byte (xSrc);
      }
    }
    if (pt >= ADVISOR_TYPE && pt <= KNIGHT_TYPE)
    {
      if (SRC (mv) == DST (mv))
      {
        Ret.c[2] = '=';
        Ret.c[3] = 'P';
      } else {
        Ret.c[2] = (yDst > ySrc ? '-' : '+');
        Ret.c[3] = Digit2Byte (xDst);
      }
    } else {
      Ret.c[2] = (yDst == ySrc ? '.' : yDst > ySrc ? '-'
        : '+');
      Ret.c[3] = (yDst == ySrc ? Digit2Byte (xDst) : Digit2Byte (ABS (ySrc - yDst) - 1));
    }
    return Ret.code;
  }
}

// 执行“空着”，该功能目前仅用在“搜索树分析器”中
gboolean cchess_try_null (CChess *chess)
{
  Position *pos = &chess->pos;
  if (Position_LastMove (pos).ChkChs > 0)
  {
    return FALSE;
  } else {
    Position_NullMove (pos);
    return TRUE;
  }
}

// 撤消“空着”，该功能目前仅用在“搜索树分析器”中
void cchess_undo_null (CChess *chess)
{
  Position *pos = &chess->pos;
  Position_UndoNullMove (pos);
}

gchar* cchess_action_to_wxf (guint32 action)
{
  Action act;
  gchar *str;

  act.code = action;

  str = g_strdup_printf("%c%c%c%c", act.c[0], act.c[1], act.c[2], act.c[3]);
  return str;
}

// 把字符串转换成着法
int cchess_ucci_to_move (const gchar *str)
{
  gint mv;
  guint32 ucci;

  ucci = *(guint32 *) str;
  mv = COORD_MOVE(ucci);
  return mv;
}

// 把着法转换成字符串
gchar* cchess_move_to_ucci (gint mv)
{
  guint32 ucci;

  ucci = MOVE_COORD (mv);

  return g_strdup_printf("%.4s", (const gchar*) &ucci);
}

/*
 * 返回上次移动的着法，可用于图形界面绘制提示
 *
 * 返回值的16 进制表示着法
 * hex(res) = 0xabcd, 低八位 0xcd 是起点坐标，高8位0xab 是终点坐标
 * 起点坐标：0xcd, 其中低4位 0xd 是x 坐标， 高4位0xc是 y 坐标
 * 终点坐标：0xab,其中低4位 0xb 是x 坐标， 高4位0xa是 y 坐标
 */
int cchess_get_last_move (CChess *chess)
{
  Position *pos = &chess->pos;
  int src, dst, src_mv, dst_mv, mv;
  int x1,y1, x2, y2;
  Move move;

  move = Position_LastMove (pos);

  src_mv = SRC(move.wmv);
  dst_mv = DST(move.wmv);

  x1 = FILE_X(src_mv) - FILE_LEFT;
  y1 = RANK_Y(src_mv) - RANK_TOP;

  x2 = FILE_X(dst_mv) - FILE_LEFT;
  y2 = RANK_Y(dst_mv) - RANK_TOP;

  src = COORD_XY(x1, y1);
  dst = COORD_XY(x2, y2);
  mv = MOVE(src, dst);
  return mv;
}
