// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#include <stdio.h>
#include <locale.h>
#include <glib.h>
#include "cchess.h"

static gboolean do_step(CChess *chess, int *status, const gchar* pgn)
{
  gint mv;
  guint32 action;

  printf("Move: %s\n", pgn);
  action = cchess_chinese_to_action (pgn);
  mv = cchess_action_to_move (chess, action);
  return cchess_try_move (chess, status, mv);
}

static void test_board_fen(void)
{
  CChess* chess;
  gchar* fen = NULL;
  int status;

  chess = cchess_new ();
  cchess_start_board (chess);
  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
  g_free(fen);

  do_step(chess, &status, "炮二平五");
  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b");
  g_free(fen);

  do_step(chess, &status, "马8进7");

  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakab1r/9/1c4nc1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w");
  g_free(fen);

  g_object_unref(chess);
}

//static void test_chinese_action(void)
//{
//  guint32 cchess_chinese_to_action (const gchar* utf8str);
//gchar*        cchess_action_to_chinese (guint32 action, int player);
//}

//  action = cchess_chinese_to_action ("马8进7");
//  mv = cchess_action_to_move (chess, action);
//  if (cchess_try_move (chess, &i, mv)) {
//    str = cchess_action_to_wxf (action);
//    printf("走了:%s, status:%d\n", str, i);
//
//    str = cchess_board_text(chess);
//    printf("%s\n", str); g_free(str);
//  }

int main (int argc, char* argv[])
{
  g_setenv("LC_ALL", "zh_CN.utf8", TRUE);
  setlocale (LC_ALL, "");
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/board/fen", test_board_fen);
  return g_test_run ();
}
