#pragma once

#include "types.hpp"

class Move {
 public:
  int from = 0, to = 0;
  int enpassant_sq_idx = -1, fifty = 0, moves = 0;
  Piece promotion = Empty, captured = Empty;
  bool castling_rights[4] = {};
  bool enpassant = false, castling = false;
  Move(int _from = 0, int _to = 0, Piece _promotion = Empty,
       Piece _captured = Empty, bool _enpassant = false, bool _castling = false)
      : from(_from),
        to(_to),
        promotion(_promotion),
        captured(_captured),
        enpassant(_enpassant),
        castling(_castling) {}
  void print();
};

class Board {
 public:
  Piece board[64];
  int enpassant_sq_idx = -1, fifty = 0, moves = 1;
  int Kpos = -1, kpos = -1;
  bool castling_rights[4] = {};
  Player turn = White;

  Board();
  constexpr Piece operator[](int i);
  int piece_color(int sq_idx);
  int sq_color(int sq_idx);
  void print(string sq = "", bool flipped = false);
  void change_turn();
  bool make_move(string move);
  void make_move(Move& move);
  void unmake_move(Move& move);
  bool load_fen(string fen);
  string to_fen();
  string to_uci(Move move);
  string to_san(Move move);
  void load_startpos();
  bool empty(int idx);
  vector<Move> generate_pseudo_moves();
  vector<Move> generate_legal_moves();
  Board mark_threats();
  // Move match_san(vector<Move> movelist, string san);
  vector<string> list_san(vector<Move> movelist);
  int divide(int depth);
  int perft(int depth, int K_pos);

  bool is_in_threat(int sq);
  bool is_in_check(Player player);

  string pos_hash();

 protected:
  void slide(vector<Move>& movelist, int sq, vector<Direction> dirs);
  void move_or_capture(vector<Move>& movelist, int sq, int dir);
};

int sq2idx(char file, char rank);
string idx2sq(int idx);

inline bool friendly(Piece a, Piece b) { return a * b > 0; }
inline bool hostile(Piece a, Piece b) { return a * b < 0; }
inline bool in_board(int idx) { return idx >= 0 && idx < 64; }

inline bool isnt_H(int idx) { return idx % 8 != 7; }
inline bool isnt_A(int idx) { return idx % 8 != 0; }
inline bool isnt_8(int idx) { return idx / 8 != 0; }
inline bool isnt_1(int idx) { return idx / 8 != 7; }

inline bool westwards(Direction dir) {
  return dir == NW || dir == SW || dir == W;
}
inline bool eastwards(Direction dir) {
  return dir == NE || dir == SE || dir == E;
}

Piece char2piece(char p);
char piece2char(Piece p);