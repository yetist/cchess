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

int main2 (void)
{
  CChess* chess;
  gchar* fen = NULL;
  gchar *str = NULL;
  guint32 x, y;
  guint32 action;
  gint i, j, mv;
  glong l;

  int status;
  gboolean result;

  chess = cchess_new ();
  cchess_start_board (chess);
  //cchess_exchange_side(chess);
//  cchess_board_mirror(chess);
  //cchess_flip_board(chess);

  str = cchess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = cchess_board_to_fen (chess);
  printf("fen: %s\n", fen); g_free(fen);

  result = do_step(chess, &status, "炮二平五");
  str = cchess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = cchess_board_to_fen (chess);
  printf("fen: %s\n", fen);

  cchess_get_last_move(chess);
  //cchess_flip_board(chess);

  str = cchess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = cchess_board_to_fen (chess);
  printf("fen: %s\n", fen);

  result = do_step(chess, &status, "马8进7");

  cchess_get_last_move(chess);
  str = cchess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = cchess_board_to_fen (chess);
  printf("fen: %s\n", fen);

 // fen = cchess_board_text(chess);
 // printf("%s\n", fen); g_free(fen);

 // cchess_flip_board (chess);

 // fen = cchess_board_text(chess);
  //printf("%s\n", fen); g_free(fen);


  //fen = cchess_fen_mirror ("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
  //printf("%s\n", fen); g_free(fen);

  //gchar *utf8_string = "炮二平五";
  //x = cchess_chinese_to_action (utf8_string);
  //printf("%d\n", x);
#if 0
  while (true){
    gchar buf[128];
    printf("%s", "请走棋>");
    scanf("%s",buf);
    printf("[%s]\n", buf);
//    action = cchess_chinese_to_action (buf);
//    mv = cchess_action_to_move (chess, action);
    mv = cchess_ucci_to_move (buf);
    if (cchess_try_move (chess, &i, mv)) {
      str = cchess_action_to_wxf (action);
      printf("走了:%s, status:%d\n", str, i);

      str = cchess_board_text(chess);
      printf("%s\n", str); g_free(str);
    }
  }

//  action = cchess_chinese_to_action ("马8进7");
//  mv = cchess_action_to_move (chess, action);
//  if (cchess_try_move (chess, &i, mv)) {
//    str = cchess_action_to_wxf (action);
//    printf("走了:%s, status:%d\n", str, i);
//
//    str = cchess_board_text(chess);
//    printf("%s\n", str); g_free(str);
//  }
//
//  glong lpmv[8];
//l = cchess_gen_moves (chess, lpmv);
//  printf("生成了 %ld 步\n", l);

  action = cchess_chinese_to_action ("炮二平五");
  mv = cchess_action_to_move (chess, action);
  printf("%d, %d\n", action, mv);
  str = cchess_move_to_ucci (mv);
  printf("%d, %d, %s\n", action, mv, str);
//  action = cchess_chinese_to_action ("士四变兵");
//  printf("%d\n", action);
//  action = cchess_chinese_to_action("仕四变兵");
//  printf("%d\n", action);
//  str = cchess_action_to_chinese(892219971, 0);
//  printf("%s\n", str);
//  str = cchess_action_to_chinese(1346188353, 1);
//  printf("%s\n", str);
//  str = cchess_action_to_wxf (892219971);
//  printf("%s\n", str);
//  str = cchess_action_to_wxf (1346188353);
//  printf("%s\n", str);

#endif
}

static void test_board_fen(void)
{
  CChess* chess;
  gchar* fen = NULL;
  int status;

  gchar *str = NULL;
  guint32 x, y;
  guint32 action;
  gint i, j, mv;
  glong l;

  chess = cchess_new ();
  cchess_start_board (chess);
  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
  g_free(fen);

  do_step(chess, &status, "炮二平五");
  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b");
  g_free(fen);

  cchess_flip_board(chess);

  fen = cchess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "RNBAKABNR/9/1C5C1/P1P1P1P1P/9/9/p1p1p1p1p/1c5c1/9/rnbakabnr w");
  g_free(fen);

  //str = cchess_board_text(chess);
  //printf("%s\n", str); g_free(str);
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
  main2();
  //  g_test_init (&argc, &argv, NULL);
  //  g_test_add_func ("/board/fen", test_board_fen);
  //  return g_test_run ();
  return 0;
}
