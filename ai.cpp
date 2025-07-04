#include "ai.hpp"

#include "knowledge.hpp"

const int MateScore = 1000000;

int piece_val[13] = {0};

AI::AI(Board& _board) : board(_board) {
  piece_val[wK + 6] = 10000;
  piece_val[wQ + 6] = 900;
  piece_val[wR + 6] = 500;
  piece_val[wB + 6] = 300;
  piece_val[wN + 6] = 250;
  piece_val[wP + 6] = 100;
  piece_val[bK + 6] = -10000;
  piece_val[bQ + 6] = -900;
  piece_val[bR + 6] = -500;
  piece_val[bB + 6] = -300;
  piece_val[bN + 6] = -250;
  piece_val[bP + 6] = -100;
}

void AI::set_clock(int _wtime, int _btime, int _winc, int _binc) {
  wtime = _wtime;
  btime = _btime;
  winc = _winc;
  binc = _binc;
}

pair<Move, int> AI::search(multiset<string>& transpositions) {
  int bestscore = -1e8;
  Move bestmove;

  Board temp = board;
  auto movelist = iterative_search();
  for (int i = 1; i >= 0; i--) {
    for (auto& m : movelist) {
      temp.make_move(m.second);
      int tp = transpositions.count(temp.pos_hash());
      temp.unmake_move(m.second);
      if (tp > i) continue;
      if (m.first > bestscore) {
        bestscore = m.first;
        bestmove = m.second;
      }
    }
    if (bestmove.from != bestmove.to) break;
  }
  cout << "info bestmove: " << bestscore << " = " << board.to_san(bestmove)
       << " out of " << movelist.size() << endl;
  return {bestmove, bestscore};
}

int get_mate_score(int score) {
  if (score > MateScore / 2)
    return MateScore - score;
  else if (score < -MateScore / 2)
    return -MateScore - score;
  return 0;
}

void print_score(int score) {
  int mate = get_mate_score(score);
  if (mate == 0)
    cout << " score cp " << score;
  else
    cout << " score mate " << mate;
}

vector<pair<int, Move>> AI::iterative_search() {
  vector<pair<int, Move>> bestmoves, tempmoves;
  // Move bestmove;
  // int bestscore = -1e8;
  // int max_breadth = 5;
  // {
  //   ofstream log("log.txt");
  //   log << "";
  //   log.close();
  // }
  int time_taken = 0, time_taken_depth = 0;
  int max_search_time = (board.turn == White) ? (wtime + winc) : (btime + binc);
  if (search_type == Fixed_depth) {
    max_search_time = INT_MAX;
    cout << "info using maxdepth: " << max_depth << endl;
  } else if (search_type == Time_per_move) {
    max_search_time = mtime;
    cout << "info using movetime: " << max_search_time << endl;
  } else if (search_type == Time_per_game) {
    double percentage = 1;  // 0.88;
    max_search_time *= min((percentage + board.moves / 116.4) / 50, percentage);
    cout << "info using time: " << max_search_time << endl;
  } else {
    max_search_time = INT_MAX;
    cout << "info using infinite: " << max_search_time << endl;
  }

  // convert moves to score-move pairs
  auto legals = board.generate_legal_moves();
  vector<pair<int, Move>> legalmoves;
  for (auto& move : legals) legalmoves.push_back({0, move});

  // iterative deepening
  int depth = 1;
  for (; time_taken * 2 < max_search_time && depth <= max_depth; depth++) {
    tempmoves = bestmoves;
    bestmoves.clear();

    // no need to seach deeper if there's only one legal move
    if (search_type != Mate)
      if (legalmoves.size() == 1) {
        bestmoves.push_back(legalmoves.front());
        break;
      }

    // mate found so prune non-mating moves (TODO: verify)
    if (get_mate_score(legalmoves.front().first) > 0) {
      for (auto& move : legalmoves)
        if (get_mate_score((move.first)) > 0) bestmoves.push_back(move);
      break;
    }

    if (get_mate_score(legalmoves.back().first) < 0) {
      // there is atleast one non-losing move
      if (get_mate_score(legalmoves.front().first) == 0) {
        // prune losing moves
        for (auto& move : legalmoves)
          if (get_mate_score(move.first) == 0) bestmoves.push_back(move);
        if (bestmoves.size() != 0) legalmoves = bestmoves;
        // break;
      } else {
        // best and worst move is losing, so no point in searching deeper
        for (auto& move : legalmoves) bestmoves.push_back(move);
        break;
      }
    }

    int mate_score = -MateScore;

    for (auto& score_move : legalmoves) {
      // cout << "searching move: " << board.to_san(score_move.second)
      //      << " score: " << score_move.first << endl;

      board.make_move(score_move.second);
      auto t1 = chrono::high_resolution_clock::now();
      int score = -alphabeta(depth, mate_score, +MateScore);
      // int score2 = -negamax(depth);
      auto t2 = chrono::high_resolution_clock::now();
      auto diff = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
      board.unmake_move(score_move.second);

      time_taken += diff;

      // break if time is up
      if (time_taken >= max_search_time && bestmoves.size() > 1) {
        for (auto& x : tempmoves) bestmoves.push_back(x);
        break;
      }
      score_move.first = score;

      if (get_mate_score(score) > 0) mate_score = score;

      // ofstream log("log.txt", std::ios_base::app);
      // log << board.to_san(move) << " = " << score << "\n\n";
      // log.close();}
      // cout << "info " << board.to_san(move) << " = " << score2 << endl;
      cout << "info depth " << depth;
      print_score(score);
      // print_score(score2);
      cout << " nodes 1 time " << diff << " pv "
           << board.to_uci(score_move.second) << endl;

      // if (score > bestscore) {
      // bestscore = score;
      // bestmove = move;
      // }
      bestmoves.push_back({score, score_move.second});
    }

    // move-ordering
    sort(legalmoves.begin(), legalmoves.end(),
         [](auto& a, auto& b) { return a.first > b.first; });

    cout << "info searched depth " << depth;
    print_score(bestmoves.front().first);
    cout << " time " << time_taken - time_taken_depth << " best "
         << board.to_uci(bestmoves.front().second) << endl;
    time_taken_depth = time_taken;
  }

  // {
  //   ofstream log("log.txt", std::ios_base::app);
  //   log << "best: " << board.to_san(bestmove) << " = " << bestscore <<
  //   "\n\n"; log.close();
  // }
  cout << "info total time: " << time_taken << endl;

  if (search_type == Mate)
    debug = to_string(depth - 2 + get_mate_score(bestmoves.front().first));

  // TODO: choose random move out of same-scoring moves

  return bestmoves;
}

int AI::print_eval() {
  int material_score = 0;
  int pst_score = 0;
  int queens = 0;
  for (int i = 0; i < 64; i++) {
    Piece piece = board.board[i];
    if (abs(piece) == wQ) queens++;
    material_score += piece_val[piece + 6];
    if (piece)
      pst_score +=
          pst[abs(piece) - 1][piece > 0 ? i : 63 - i] * (piece > 0 ? 1 : -1);
  }

  if (queens == 0) {  // assume near endgame
    pst_score += pst_k_end[board.Kpos] - pst_k_end[63 - board.kpos];
    // https://www.chessprogramming.org/Mop-up_Evaluation
    int cmd =
        (material_score > 0 ? pst_cmd[board.Kpos] : pst_cmd[63 - board.kpos]);
    int file1 = board.Kpos % 8, rank1 = board.Kpos / 8;
    int file2 = board.kpos % 8, rank2 = board.kpos / 8;
    int md = abs(rank2 - rank1) + abs(file2 - file1);
    pst_score += 5 * cmd + 2 * (14 - md);
  }

  cout << "fen: " << board.to_fen() << endl;
  cout << "material: " << material_score << endl;
  cout << "position: " << pst_score << endl;

  cout << "is in check: " << board.is_in_check(board.turn) << endl;

  return material_score + pst_score;
}

inline int AI::eval() {
  int material_score = 0;
  int pst_score = 0;
  int mobility_score = 0;
  int queens = 0;
  for (int i = 0; i < 64; i++) {
    Piece piece = board.board[i];
    if (abs(piece) == wQ) queens++;
    material_score += piece_val[piece + 6];
    if (piece)
      pst_score +=
          pst[abs(piece) - 1][piece > 0 ? i : 63 - i] * (piece > 0 ? 1 : -1);
  }

  if (queens == 0) {  // assume near endgame
    pst_score += pst_k_end[board.Kpos] - pst_k_end[63 - board.kpos];
    // https://www.chessprogramming.org/Mop-up_Evaluation
    int cmd =
        (material_score > 0 ? pst_cmd[board.Kpos] : pst_cmd[63 - board.kpos]);
    int file1 = board.Kpos % 8, rank1 = board.Kpos / 8;
    int file2 = board.kpos % 8, rank2 = board.kpos / 8;
    int md = abs(rank2 - rank1) + abs(file2 - file1);
    pst_score += 5 * cmd + 2 * (14 - md);
  }

  // int rel_mobility = board.generate_legal_moves().size();
  // board.change_turn();
  // int opp_mobility = board.generate_legal_moves().size();
  // board.change_turn();
  // mobility_score = (rel_mobility - opp_mobility) * board.turn;

  return material_score + pst_score + mobility_score;
}

int AI::negamax(int depth) {
  // while (g_main_context_pending(0)) g_main_context_iteration(0, 0);

  if (depth == 0) return eval() * board.turn;
  int bestscore = -MateScore;
  auto legals = board.generate_legal_moves();
  for (auto& move : legals) {
    board.make_move(move);
    int score = -negamax(depth - 1);
    board.unmake_move(move);
    bestscore = max(bestscore, score);
  }
  if (legals.size() == 0)
    return board.is_in_check(board.turn) ? -MateScore + depth : 0;

  return bestscore;
}

int reduction(int depth, int moves) { return 1; }

int AI::alphabeta(int depth, int alpha, int beta) {
  // while (g_main_context_pending(0)) g_main_context_iteration(0, 0);
  int extra_depth = 0;
  bool is_in_check = board.is_in_check(board.turn);
  if (is_in_check) extra_depth++;

  // Mate distance pruning
  alpha = max(alpha, -MateScore + board.moves);
  beta = min(beta, MateScore - board.moves - 1);
  if (alpha >= beta) return alpha;

  if (depth <= 0 || depth > 50) return quiesce(0, alpha, beta);
  int bestscore = -MateScore;

  auto legals = board.generate_legal_moves();

  // if (legals.size() == 1) return alpha;

  int moves = 0, late_moves = legals.size() / 2 + 1;
  int score;

  // null move pruning
  if (!is_in_check) {
    board.change_turn();
    score = -alphabeta(depth + extra_depth - reduction(depth, moves), -beta,
                       -alpha);
    board.change_turn();
    if (score >= beta) return score;
  }

  for (auto& move : legals) {
    board.make_move(move);
    // late move reduction
    if (moves < late_moves) {
      score = -alphabeta(depth + extra_depth - 1, -beta, -alpha);
    } else {
      score = -alphabeta(depth + extra_depth - 1 - reduction(depth, moves),
                         -beta, -alpha);
      if (score >= alpha)
        score = -alphabeta(depth + extra_depth - 1, -beta, -alpha);
    }
    board.unmake_move(move);
    if (score >= beta) return score;
    alpha = max(alpha, score);
    bestscore = max(bestscore, score);
    moves++;
  }
  // TODO: check + or - depth
  if (legals.size() == 0) return is_in_check ? -MateScore + depth : 0;

  return bestscore;
}

int AI::quiesce(int depth, int alpha, int beta) {
  // while (g_main_context_pending(0)) g_main_context_iteration(0, 0);

  int pat = eval() * board.turn;
  if (pat >= beta) return beta;
  if (depth > 5) return pat;

  // // delta pruning, 900=queen value
  // if (alpha - pat > 900) return alpha;

  alpha = max(alpha, pat);

  auto legals = board.generate_legal_moves();

  // if (legals.size() == 1) return 0;

  for (auto& move : legals) {
    if (board.empty(move.to)) continue;  // consider only captures
    board.make_move(move);
    int score = -quiesce(depth + 1, -beta, -alpha);
    board.unmake_move(move);
    if (score >= beta) return score;
    alpha = max(alpha, score);
  }
  if (legals.size() == 0)
    return board.is_in_check(board.turn) ? -MateScore + depth : 0;

  return alpha;
}