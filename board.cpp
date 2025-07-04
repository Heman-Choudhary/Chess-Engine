#include "board.hpp"

int sq2idx(char file, char rank) {
  return (file - 'a') + (7 - (rank - '1')) * 8;  // matrix magic
}

string idx2sq(int idx) {
  string sq;
  sq.append(1, idx % 8 + 'a');
  return sq.append(1, (7 - idx / 8) + '1');
}

Piece char2piece(char p) {
  size_t pos = string("kqrbnp.PNBRQK").find(p);
  if (pos != string::npos) return static_cast<Piece>(pos - 6);
  return Empty;
}

char piece2char(Piece p) { return "kqrbnp.PNBRQK"[p + 6]; }

bool Board::make_move(string move) {
  Move temp;
  int l = move.length();
  if (l == 4 || l == 5) {
    temp.from = sq2idx(move[0], move[1]);
    temp.to = sq2idx(move[2], move[3]);
    if (l == 5)
      temp.promotion =
          char2piece(turn == White ? toupper(move[4]) : tolower(move[4]));
    temp.castling = (temp.from == Kpos && (move == "e1g1" || move == "e1c1")) ||
                    (temp.from == kpos && (move == "e8g8" || move == "e8c8"));
    temp.enpassant = abs(board[temp.from]) == wP && enpassant_sq_idx == temp.to;
    make_move(temp);
    return true;
  }
  return false;
}

void Move::print() {
  cerr << "move: " << idx2sq(from) << idx2sq(to) << " " << captured << promotion
       << (castling ? " castling" : "") << (enpassant ? " enpassant" : "")
       << endl;
}

Board::Board() { load_startpos(); }

constexpr Piece Board::operator[](int i) { return board[i]; }

int Board::piece_color(int sq_idx) { return isupper(board[sq_idx]) ? 1 : -1; }
int Board::sq_color(int sq_idx) {
  return (sq_idx % 2 && (sq_idx / 8) % 2) ||
         (sq_idx % 2 == 0 && (sq_idx / 8) % 2 == 0);
}

void Board::print(string sq, bool flipped) {
  if (sq.length() == 4)
    sq[0] = sq[2], sq[1] = sq[3];  // extract destination square from move
  int sq_idx = sq != "" ? sq2idx(sq[0], sq[1]) : -1;
  char rank = '8', file = 'a';
  if (flipped) {
    if (~sq_idx) sq_idx = 63 - sq_idx;
    reverse(board, board + 64);
    rank = '1', file = 'h';
  }
  cout << endl << " ";
  for (int i = 0; i < 8; i++) cout << " " << (flipped ? file-- : file++);
  cout << endl << (flipped ? rank++ : rank--) << "|";
  for (int i = 0; i < 64; i++) {
    cout << (isupper(board[i]) ? "\e[33m" : "\e[36m");
    if (board[i] == Empty) {
      if ((i % 2 && (i / 8) % 2) || (i % 2 == 0 && (i / 8) % 2 == 0))
        // cout << "\e[47m";
        cout << ".\e[0m|";
      else
        cout << " \e[0m|";
    } else
      cout << piece2char(board[i]) << "\e[0m|";
    if (i % 8 == 7) {
      if (~sq_idx && sq_idx / 8 == i / 8) cout << "<";
      cout << endl;
      if (i != 63) cout << (flipped ? rank++ : rank--) << "|";
    }
  }
  if (~sq_idx) {
    cout << " ";
    for (int i = 0; i < 8; i++) cout << (sq_idx % 8 == i ? " ^" : "  ");
  }
  if (flipped) reverse(board, board + 64);
  cout << endl;
}

void Board::change_turn() { turn = turn == White ? Black : White; }

void Board::make_move(Move& move) {
  // save current aspects
  copy_n(castling_rights, 4, move.castling_rights);
  move.enpassant_sq_idx = enpassant_sq_idx;
  move.fifty = fifty;
  // move.moves = moves;

  // update half-move clock
  if (board[move.to] != Empty || abs(board[move.from]) == wP)
    fifty = 0;
  else
    fifty++;

  // update castling rights and king pos
  if (board[move.from] == wK) {
    castling_rights[0] = castling_rights[1] = false;
    Kpos = move.to;
  } else if (board[move.from] == bK) {
    castling_rights[2] = castling_rights[3] = false;
    kpos = move.to;
  }
  if (board[move.from] == wR || board[move.to] == wR) {
    if (move.from == 63 || move.to == 63)
      castling_rights[0] = false;
    else if (move.from == 56 || move.to == 56)
      castling_rights[1] = false;
  }
  if (board[move.from] == bR || board[move.to] == bR) {
    if (move.from == 7 || move.to == 7)
      castling_rights[2] = false;
    else if (move.from == 0 || move.to == 0)
      castling_rights[3] = false;
  }

  // update enpassant square
  if (board[move.from] == wP && move.to - move.from == N + N)
    enpassant_sq_idx = move.from + N;
  else if (board[move.from] == bP && move.to - move.from == S + S)
    enpassant_sq_idx = move.from + S;
  else
    enpassant_sq_idx = -1;

  // update board
  Piece captured = board[move.to];
  board[move.to] = move.promotion == Empty ? board[move.from] : move.promotion;
  board[move.from] = move.captured;
  move.captured = captured;

  if (move.castling) {  // move rook when castling
    if (move.from == 4 && move.to == 6)
      board[7] = Empty, board[5] = bR;
    else if (move.from == 4 && move.to == 2)
      board[0] = Empty, board[3] = bR;
    else if (move.from == 60 && move.to == 62)
      board[63] = Empty, board[61] = wR;
    else if (move.from == 60 && move.to == 58)
      board[56] = Empty, board[59] = wR;
  } else if (move.enpassant) {  // remove pawn when enpassant
    int rel_S = turn * S;       //(turn == White ? S : N);
    board[move.to + rel_S] = Empty;
  }
  change_turn();

  moves++;
}
void Board::unmake_move(Move& move) {
  // restore current aspects
  copy_n(move.castling_rights, 4, castling_rights);
  enpassant_sq_idx = move.enpassant_sq_idx;
  fifty = move.fifty;
  // moves = move.moves;

  // restore king pos
  if (board[move.to] == wK) Kpos = move.from;
  if (board[move.to] == bK) kpos = move.from;

  // update board
  Piece captured = board[move.from];
  board[move.from] =
      move.promotion == Empty ? board[move.to] : (turn == Black ? wP : bP);
  board[move.to] = move.captured;
  move.captured = captured;

  if (move.castling) {  // move rook when castling
    if (move.from == 4 && move.to == 6)
      board[7] = bR, board[5] = Empty;
    else if (move.from == 4 && move.to == 2)
      board[0] = bR, board[3] = Empty;
    else if (move.from == 60 && move.to == 62)
      board[63] = wR, board[61] = Empty;
    else if (move.from == 60 && move.to == 58)
      board[56] = wR, board[59] = Empty;
  } else if (move.enpassant) {  // add pawn when enpassant
    int rel_N = turn * N;       // (turn == White ? N : S);
    board[move.to + rel_N] = (turn == White ? wP : bP);  // TODO:verify
  }
  change_turn();

  moves--;
}

bool Board::load_fen(string fen) {
  fill_n(board, 64, Empty);
  int part = 0, p = 0;
  char enpassant_sq[2];
  enpassant_sq_idx = fifty = moves = 0;
  fill_n(castling_rights, 4, false);

  for (auto& x : fen) {
    if (x == ' ') {
      part++, p = 0;
    } else if (part == 0) {
      if (p > 63) return false;
      if (isdigit(x))
        for (int dots = x - '0'; dots--;) board[p++] = Empty;
      else if (x != '/')
        board[p++] = char2piece(x);
    } else if (part == 1) {
      turn = x == 'w' ? White : Black;
    } else if (part == 2) {
      if (x != '-') {
        if (x == 'K') castling_rights[0] = true;
        if (x == 'Q') castling_rights[1] = true;
        if (x == 'k') castling_rights[2] = true;
        if (x == 'q') castling_rights[3] = true;
      }
    } else if (part == 3) {
      if (x == '-')
        enpassant_sq_idx = -1;
      else
        enpassant_sq[p++] = x;
    } else if (part == 4) {
      fifty *= 10;
      fifty += x - '0';
    } else if (part == 5) {
      moves *= 10;
      moves += x - '0';
    }
    // cout << part << "," << p << ")" << x << ":" << moves << endl;
  }
  if (~enpassant_sq_idx)
    enpassant_sq_idx = sq2idx(enpassant_sq[0], enpassant_sq[1]);
  for (int i = 0; i < 64; i++)
    if (board[i] == wK)
      Kpos = i;
    else if (board[i] == bK)
      kpos = i;
  if (Kpos == 64 || kpos == 64) return false;
  return part > 1;
}

string Board::to_fen() {
  string fen = "";
  int blanks = 0;
  for (int i = 0; i < 64; i++) {
    if (board[i] == Empty) blanks++;
    if (blanks > 0 && (board[i] != Empty || !isnt_H(i))) {
      fen += '0' + blanks;
      blanks = 0;
    }
    if (board[i] != Empty) fen += piece2char(board[i]);
    if (!isnt_H(i) && i != 63) fen += '/';
  }
  fen += " ";
  fen += (turn == White ? "w" : "b");
  string castling = "";
  if (castling_rights[0]) castling += "K";
  if (castling_rights[1]) castling += "Q";
  if (castling_rights[2]) castling += "k";
  if (castling_rights[3]) castling += "q";
  if (castling != "")
    fen += " " + castling;
  else
    fen += " -";
  if (~enpassant_sq_idx)
    fen += " " + idx2sq(enpassant_sq_idx);
  else
    fen += " -";
  fen += " " + to_string(fifty) + " " + to_string(moves);
  return fen;
}

string Board::to_uci(Move move) {
  string uci = idx2sq(move.from) + idx2sq(move.to);
  if (move.promotion != Empty)
    return uci + piece2char(move.promotion);
  else
    return uci;
}

string Board::to_san(Move move) {
  Piece piece = static_cast<Piece>(abs(board[move.from]));
  string san;
  if (move.castling) {
    if ((move.from == 4 && move.to == 6) || (move.from == 60 && move.to == 62))
      san = "O-O";
    else if ((move.from == 4 && move.to == 2) ||
             (move.from == 60 && move.to == 58))
      san = "O-O-O";
  } else {
    if (piece != wP && piece != Empty) san = piece2char(piece);
    san += idx2sq(move.from);
    if (!empty(move.to) || move.enpassant) san += 'x';
    san += idx2sq(move.to);
    if (move.promotion != Empty) {
      san += '=';
      san += toupper(move.promotion);
    }
  }

  return san;
}

void Board::load_startpos() {
  load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Board::empty(int idx) { return board[idx] == Empty; }

void Board::slide(vector<Move>& movelist, int sq, vector<Direction> dirs) {
  for (auto& dir : dirs) {
    if (westwards(dir) && !isnt_A(sq)) continue;
    if (eastwards(dir) && !isnt_H(sq)) continue;
    for (int dest = sq + dir;
         in_board(dest) && !friendly(board[sq], board[dest]); dest += dir) {
      movelist.push_back(Move(sq, dest));
      if (hostile(board[sq], board[dest])) break;
      if (westwards(dir) && !isnt_A(dest)) break;
      if (eastwards(dir) && !isnt_H(dest)) break;
    }
  }
}

// Move Board::match_san(vector<Move> movelist, string san) {
//   // TODO
//   // int count = 0;
//   // for (char c : {'+', '#', '-', '+'})
//   //   san.erase(std::remove(san.begin(), san.end(), c), san.end());

//   // for (auto& move : movelist) {
//   //   if (move.to = 0)
//   //     ;
//   // }
// }

void Board::move_or_capture(vector<Move>& movelist, int sq, int dir) {
  if (!friendly(board[sq], board[sq + dir]))
    movelist.push_back(Move(sq, sq + dir));
}

string Board::pos_hash() {
  string fen = "";
  for (auto& s : board) fen += piece2char(s);
  fen += "|";
  fen += (turn == White ? "w" : "b");
  fen += "|";
  if (castling_rights[0]) fen += "K";
  if (castling_rights[1]) fen += "Q";
  if (castling_rights[2]) fen += "k";
  if (castling_rights[3]) fen += "q";
  fen += "|";
  fen += idx2sq(enpassant_sq_idx);
  return fen;
}