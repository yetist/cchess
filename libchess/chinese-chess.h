/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define CHINESE_TYPE_CHESS              (chinese_chess_get_type ())
G_DECLARE_FINAL_TYPE (ChineseChess, chinese_chess, CHINESE, CHESS, GObject)

ChineseChess* chinese_chess_new             (void);
void          chinese_chess_start_board     (ChineseChess *chess);
void          chinese_chess_fen_to_board    (ChineseChess *chess, const gchar* fen);
gchar*        chinese_chess_board_to_fen    (ChineseChess *chess);
void          chinese_chess_clear_board     (ChineseChess *chess);
gchar*        chinese_chess_board_text      (ChineseChess *chess);

void          chinese_chess_board_mirror    (ChineseChess *chess);
void          chinese_chess_exchange_side   (ChineseChess *chess);
void          chinese_chess_flip_board      (ChineseChess *chess);

void          chinese_chess_set_irrev       (ChineseChess *chess);

void          chinese_chess_promotion       (ChineseChess *chess, gboolean promotion);
gboolean      chinese_chess_can_promote     (ChineseChess *chess, long sq);

void          chinese_chess_add_piece       (ChineseChess *chess, long sq, long pc, gboolean delete);

// 生成全部合理着法
glong         chinese_chess_gen_moves       (ChineseChess *chess, glong* lpmv);
gboolean      chinese_chess_try_move        (ChineseChess *chess, int* lpStatus, int mv);
void          chinese_chess_undo_move       (ChineseChess *chess);

gchar*        chinese_chess_fen_mirror      (const gchar* fen);
guint32       chinese_chess_file_mirror     (guint32 fstr);
int           chinese_chess_get_last_move   (ChineseChess *chess);

int           chinese_chess_action_to_move  (ChineseChess *chess, guint32 action);
guint32       chinese_chess_move_to_action  (ChineseChess *chess, int mv);

gchar*        chinese_chess_action_to_wxf     (guint32 action);
guint32       chinese_chess_chinese_to_action (const gchar* utf8str);
gchar*        chinese_chess_action_to_chinese (guint32 action, int player);
int           chinese_chess_ucci_to_move      (const gchar *str);
gchar*        chinese_chess_move_to_ucci      (gint mv);

//TODO:
//1. 选中一个子的可用走法
//2. 上一步的源和目标

// 执行“空着”，该功能目前仅用在“搜索树分析器”中
gboolean      chinese_chess_try_null        (ChineseChess *chess);
// 撤消“空着”，该功能目前仅用在“搜索树分析器”中
void          chinese_chess_undo_null       (ChineseChess *chess);
#if 0
const gchar*  chinese_chess_version         (void);
#endif

G_END_DECLS
