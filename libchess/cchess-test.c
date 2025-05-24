// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#include <stdio.h>
#include <locale.h>
#include <glib.h>
#include "chinese-chess.h"

static gboolean do_step(ChineseChess *chess, int *status, const gchar* pgn)
{
  gint mv;
  guint32 action;

  printf("Move: %s\n", pgn);
  action = chinese_chess_chinese_to_action (pgn);
  mv = chinese_chess_action_to_move (chess, action);
  return chinese_chess_try_move (chess, status, mv);
}

int main2 (void)
{
  ChineseChess* chess;
  gchar* fen = NULL;
  gchar *str = NULL;
  guint32 x, y;
  guint32 action;
  gint i, j, mv;
  glong l;

  int status;
  gboolean result;

  chess = chinese_chess_new ();
  chinese_chess_start_board (chess);
  //chinese_chess_exchange_side(chess);
//  chinese_chess_board_mirror(chess);
  //chinese_chess_flip_board(chess);

  str = chinese_chess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = chinese_chess_board_to_fen (chess);
  printf("fen: %s\n", fen); g_free(fen);

  result = do_step(chess, &status, "炮二平五");
  str = chinese_chess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = chinese_chess_board_to_fen (chess);
  printf("fen: %s\n", fen);

  chinese_chess_get_last_move(chess);
  //chinese_chess_flip_board(chess);

  str = chinese_chess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = chinese_chess_board_to_fen (chess);
  printf("fen: %s\n", fen);

  result = do_step(chess, &status, "马8进7");

  chinese_chess_get_last_move(chess);
  str = chinese_chess_board_text(chess);
  printf("%s\n", str); g_free(str);
  fen = chinese_chess_board_to_fen (chess);
  printf("fen: %s\n", fen);

 // fen = chinese_chess_board_text(chess);
 // printf("%s\n", fen); g_free(fen);

 // chinese_chess_flip_board (chess);

 // fen = chinese_chess_board_text(chess);
  //printf("%s\n", fen); g_free(fen);


  //fen = chinese_chess_fen_mirror ("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
  //printf("%s\n", fen); g_free(fen);

  //gchar *utf8_string = "炮二平五";
  //x = chinese_chess_chinese_to_action (utf8_string);
  //printf("%d\n", x);
#if 0
  while (true){
    gchar buf[128];
    printf("%s", "请走棋>");
    scanf("%s",buf);
    printf("[%s]\n", buf);
//    action = chinese_chess_chinese_to_action (buf);
//    mv = chinese_chess_action_to_move (chess, action);
    mv = chinese_chess_ucci_to_move (buf);
    if (chinese_chess_try_move (chess, &i, mv)) {
      str = chinese_chess_action_to_wxf (action);
      printf("走了:%s, status:%d\n", str, i);

      str = chinese_chess_board_text(chess);
      printf("%s\n", str); g_free(str);
    }
  }

//  action = chinese_chess_chinese_to_action ("马8进7");
//  mv = chinese_chess_action_to_move (chess, action);
//  if (chinese_chess_try_move (chess, &i, mv)) {
//    str = chinese_chess_action_to_wxf (action);
//    printf("走了:%s, status:%d\n", str, i);
//
//    str = chinese_chess_board_text(chess);
//    printf("%s\n", str); g_free(str);
//  }
//
//  glong lpmv[8];
//l = chinese_chess_gen_moves (chess, lpmv);
//  printf("生成了 %ld 步\n", l);

  action = chinese_chess_chinese_to_action ("炮二平五");
  mv = chinese_chess_action_to_move (chess, action);
  printf("%d, %d\n", action, mv);
  str = chinese_chess_move_to_ucci (mv);
  printf("%d, %d, %s\n", action, mv, str);
//  action = chinese_chess_chinese_to_action ("士四变兵");
//  printf("%d\n", action);
//  action = chinese_chess_chinese_to_action("仕四变兵");
//  printf("%d\n", action);
//  str = chinese_chess_action_to_chinese(892219971, 0);
//  printf("%s\n", str);
//  str = chinese_chess_action_to_chinese(1346188353, 1);
//  printf("%s\n", str);
//  str = chinese_chess_action_to_wxf (892219971);
//  printf("%s\n", str);
//  str = chinese_chess_action_to_wxf (1346188353);
//  printf("%s\n", str);

#endif
}

static void test_board_fen(void)
{
  ChineseChess* chess;
  gchar* fen = NULL;
  int status;

  gchar *str = NULL;
  guint32 x, y;
  guint32 action;
  gint i, j, mv;
  glong l;

  chess = chinese_chess_new ();
  chinese_chess_start_board (chess);
  fen = chinese_chess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
  g_free(fen);

  do_step(chess, &status, "炮二平五");
  fen = chinese_chess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b");
  g_free(fen);

  chinese_chess_flip_board(chess);

  fen = chinese_chess_board_to_fen (chess);
  g_assert_cmpstr (fen, ==, "RNBAKABNR/9/1C5C1/P1P1P1P1P/9/9/p1p1p1p1p/1c5c1/9/rnbakabnr w");
  g_free(fen);

  //str = chinese_chess_board_text(chess);
  //printf("%s\n", str); g_free(str);
  g_object_unref(chess);
}

//static void test_chinese_action(void)
//{
//  guint32 chinese_chess_chinese_to_action (const gchar* utf8str);
//gchar*        chinese_chess_action_to_chinese (guint32 action, int player);
//}

//  action = chinese_chess_chinese_to_action ("马8进7");
//  mv = chinese_chess_action_to_move (chess, action);
//  if (chinese_chess_try_move (chess, &i, mv)) {
//    str = chinese_chess_action_to_wxf (action);
//    printf("走了:%s, status:%d\n", str, i);
//
//    str = chinese_chess_board_text(chess);
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
