#pragma once

#include <gtk/gtk.h>

#include "board.hpp"
#include "types.hpp"

class AI {
 public:
  Board board;
  int wtime = 30000, btime = 30000, winc = 0, binc = 0;
  int mtime = 1000;  // move time
  int max_depth = 100;
  SearchType search_type = Time_per_game;

  string debug = "";

  AI(Board &board);
  pair<Move, int> search(multiset<string> &transpositions);
  void set_clock(int _wtime, int _btime, int _winc, int _binc);
  int print_eval();

 protected:
  int eval();
  int negamax(int depth);
  int alphabeta(int depth, int alpha, int beta);
  int quiesce(int depth, int alpha, int beta);
  vector<pair<int, Move>> iterative_search();
};
