/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define CHINESE_TYPE_CHESS              (cchess_get_type ())

G_DECLARE_FINAL_TYPE (CChess, cchess, CHINESE, CHESS, GObject)

CChess*       cchess_new                (void);
void          cchess_start_board        (CChess *chess);
void          cchess_fen_to_board       (CChess *chess, const gchar* fen);
gchar*        cchess_board_to_fen       (CChess *chess);
void          cchess_clear_board        (CChess *chess);
gchar*        cchess_board_text         (CChess *chess);

void          cchess_board_mirror       (CChess *chess);
void          cchess_exchange_side      (CChess *chess);
void          cchess_flip_board         (CChess *chess);

void          cchess_set_irrev          (CChess *chess);

void          cchess_promotion          (CChess *chess, gboolean promotion);
gboolean      cchess_can_promote        (CChess *chess, long sq);

void          cchess_add_piece          (CChess *chess, long sq, long pc, gboolean delete);

// 生成全部合理着法
glong         cchess_gen_moves          (CChess *chess, glong* lpmv);
gboolean      cchess_try_move           (CChess *chess, int* status, int mv);
void          cchess_undo_move          (CChess *chess);

gchar*        cchess_fen_mirror         (const gchar* fen);
guint32       cchess_file_mirror        (guint32 fstr);
int           cchess_get_last_move      (CChess *chess);

int           cchess_action_to_move     (CChess *chess, guint32 action);
guint32       cchess_move_to_action     (CChess *chess, int mv);

gchar*        cchess_action_to_wxf      (guint32 action);
guint32       cchess_chinese_to_action  (const gchar* utf8str);
gchar*        cchess_action_to_chinese  (guint32 action, int player);
int           cchess_ucci_to_move       (const gchar *str);
gchar*        cchess_move_to_ucci       (gint mv);

//TODO:
//1. 选中一个子的可用走法
//2. 上一步的源和目标

// 执行“空着”，该功能目前仅用在“搜索树分析器”中
gboolean      cchess_try_null           (CChess *chess);
// 撤消“空着”，该功能目前仅用在“搜索树分析器”中
void          cchess_undo_null          (CChess *chess);
#if 0
const gchar*  cchess_version         (void);
#endif

G_END_DECLS
