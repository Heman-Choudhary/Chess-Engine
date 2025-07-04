#include "game.hpp"

Game::Game() { new_game(); }

bool Game::make_move(string m) { return make_move(m); }

bool Game::make_move(Move m) {
  if (end > 500) return false;
  if (m.from == m.to) return false;

  if (ply != end) {
    int oldply = ply;
    while (ply < end) next();
    while (ply > oldply) prev(), movelist.pop_back();
    end = ply;
  }
  board.make_move(m);
  movelist.push_back(m);
  if (m.captured != Empty) {
    auto& alive = (hostile(m.captured, wK) ? black_alive : white_alive);
    auto it = alive.find(m.captured);
    if (it != alive.end()) alive.erase(it);
  }
  if (m.captured != Empty || board.board[m.to] == bP || board.board[m.to] == wP)
    transpositions.clear();

  ply++, end++;
  transpositions.insert(board.pos_hash());
  result = get_result();
  return true;
}

void Game::prev() {
  if (ply > 0) board.unmake_move(movelist[--ply]);
}
void Game::next() {
  if (ply < end) board.make_move(movelist[ply++]);
}

void Game::print_movelist() {
  for (size_t i = 0; i < movelist.size(); i++) {
    cerr << i + 1 << " " << board.to_san(movelist[i]) << "    ";
    movelist[i].print();
  }
}

void Game::print_pgn() {
  cerr << "PGN" << endl;
  seek(0);
  for (int i = 0; i < end; i++) {
    if (ply % 2 == 0) cerr << (ply / 2 + 1) << ".";
    cerr << board.to_san(movelist[ply]) << " ";
    next();
  }
}
void Game::seek(int n) {
  while (ply < n && ply < end) next();
  while (ply > n && ply > 0) prev();
}

Move Game::random_move() {
  vector<Move> legal = board.generate_legal_moves();
  Move bestmove;
  if (legal.size() > 0) {
    bestmove = legal[rand() % legal.size()];
  }
  return bestmove;
}

pair<Move, int> Game::ai_move() {
  Board temp = board;
  AI ai(temp);
  // ai.set_clock(10 * 60 * 1000, 10 * 60 * 1000, 0, 0);
  ai.search_type = Time_per_move;
  ai.mtime = 3000;
  return ai.search(transpositions);
}

Status Game::get_result() {
  if (result != Undecided) return result;
  bool can_move = board.generate_legal_moves().size();
  if (!can_move) {
    if (board.is_in_check(board.turn))
      return board.turn == White ? Black_wins : White_wins;
    else
      return Draw;
  }

  // obvious draws
  if (board.fifty >= 100) return Draw;

  size_t w_size = white_alive.size();
  size_t b_size = black_alive.size();
  // KvK
  if (w_size == 1 && b_size == 1) return Draw;
  // KvKB or KvKN
  if (w_size == 1 && b_size == 2)
    if (black_alive.count(bB) || black_alive.count(bN)) return Draw;
  // KBvK or KNvK
  if (b_size == 1 && w_size == 2)
    if (white_alive.count(wB) || white_alive.count(wN)) return Draw;
  // // KBvKB, bishops being the same color
  // if (b_size == 2 && w_size == 2 &&
  //     board.sq_color(board.board.find('B')) ==
  //         board.sq_color(board.board.find('b')))
  //   return Draw;

  // repetition
  if (transpositions.count(board.pos_hash()) == 3) return Draw;

  return Undecided;
}

void Game::new_game() {
  ply = end = 0;
  movelist.clear();
  board.load_startpos();
  update_alive();
  result = Undecided;
  transpositions.clear();
}

bool Game::load_fen(string fen) {
  new_game();
  auto temp = board;
  bool valid = temp.load_fen(fen);
  if (valid) {
    board = temp;
    update_alive();
    result = get_result();
  }
  return valid;
}

void Game::update_alive() {
  white_alive.clear();
  black_alive.clear();
  for (auto& x : board.board)
    if (x != Empty) (x < 0 ? black_alive : white_alive).insert(x);
}
