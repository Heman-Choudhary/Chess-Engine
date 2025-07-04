#include <sstream>
#include <thread>

#include "ai.hpp"
#include "board.hpp"
#include "game.hpp"

bool debug_mode = false;

void test_navigation() {
  Game g;
  // g.board.load_fen("8/8/3k4/8/8/3KP1r1/8/8 w - - 0 1");
  g.board.print();

  string cmd;
  while (cin >> cmd) {
    if (cmd == "list" || cmd == "l")
      g.print_movelist();
    else if (cmd == "perft" || cmd == "divide") {
      int depth;
      cin >> depth;
      g.board.divide(depth);
    } else if (cmd == "rand" || cmd == "r") {
      g.random_move();
    } else if (cmd == "random") {
      int times;
      cin >> times;
      for (int i = 0; i < times; i++) g.random_move();
    } else if (cmd == "prev" || cmd == "p")
      g.prev();
    else if (cmd == "next" || cmd == "n")
      g.next();
    else if (cmd == "start" || cmd == "s")
      g.seek(0);
    else if (cmd == "end" || cmd == "e")
      g.seek(g.end);
    else if (cmd == "seek") {
      int pos;
      cin >> pos;
      g.seek(pos);
    } else if (cmd == "pseudo") {
      Game temp = g;
      temp.movelist = g.board.generate_pseudo_moves();
      temp.print_movelist();
    } else if (cmd == "turn") {
      g.board.change_turn();
    } else if (cmd == "legal") {
      Game temp = g;
      temp.movelist = g.board.generate_legal_moves();
      temp.print_movelist();
    } else if (cmd == "threats") {
      g.board.mark_threats().print();
    } else if (cmd == "move" || cmd == "m") {
      cin >> cmd;
      bool valid = false;
      for (auto& move : g.board.generate_pseudo_moves()) {
        if (g.board.to_san(move) == cmd || g.board.to_uci(move) == cmd) {
          cout << "Valid move" << endl;
          g.make_move(move);
          valid = true;
          break;
        }
      }
      if (!valid) cout << "Invalid move" << endl;
    } else if (cmd == "fen") {
      getline(cin, cmd);
      g.board.load_fen(cmd.substr(1));
    } else if (cmd == "pgn") {
      g.print_pgn();
    } else if (cmd == "lichess") {
      string fen = g.board.to_fen();
      replace(fen.begin(), fen.end(), ' ', '_');
      cout << "https://lichess.org/analysis/" << fen << endl;
    } else if (cmd == "display" || cmd == "d") {
      g.board.print();
      cout << "ply " << g.ply << " end " << g.end << endl
           << "fen " << g.board.to_fen() << endl;
    } else if (cmd == "play") {
      vector<Move> movelist;
      Move move;
      bool white = true;
      // g.board.load_startpos();
      cout << "Play as (1) White (2) Black" << endl;
      cin >> cmd;
      if (cmd == "2") {
        move = g.random_move();
        white = false;
      }
      while (cmd != "q") {
        g.board.print(g.board.to_uci(move), !white);
        movelist = g.board.generate_legal_moves();
        cout << "(q) Quit (n) New game" << endl;
        if (movelist.size() > 0) {
          for (size_t i = 0; i < movelist.size(); i++) {
            cout << "(";
            cout.width(2);
            cout << left << i + 1 << flush;
            cout << ") ";
            cout.width(7);
            cout << g.board.to_san(movelist[i]) << flush;
            if (i % 3 == 2) cout << endl;
          }
          cout << endl;
          cout << "Choose move: ";
          cin >> cmd;
          if (cmd == "n") {
            g.new_game();
            continue;
          }
          size_t i = 0;
          if (all_of(cmd.begin(), cmd.end(), ::isdigit)) i = stoi(cmd);
          if (i > 0 && i <= movelist.size())
            g.make_move(movelist[i - 1]);
          else
            cout << "Invalid move.";
          move = g.random_move();
          if (move.from == 0 && move.to == 0) {
            if (g.board.is_in_check(white ? Black : White))
              cout << "You win!" << endl;
            else
              cout << "Stalemate!" << endl;
            break;
          }
        } else {
          if (g.board.is_in_check(white ? White : Black))
            cout << "I win!" << endl;
          else
            cout << "Stalemate!" << endl;
          break;
        }
      }
    } else if (cmd == "quit" || cmd == "q")
      break;
    g.board.print();
    cout << "ply " << g.ply << " end " << g.end << endl
         << "fen " << g.board.to_fen() << endl;
  }
}

void uci() {
  multiset<string> transpositions;
  Board board1;
  AI ai(board1);
  auto& board = ai.board;
  string line, token;

  while (getline(cin, line)) {
    istringstream iss(line);
    iss >> token;
    if (token == "uci") {
      cout << "id name Chess by Soham" << endl;
      cout << "id author Soham Korade" << endl;
      cout << "uciok" << endl;
    } else if (token == "ucinewgame") {
    } else if (token == "position") {
      iss >> token;
      transpositions.clear();
      if (token == "fen") {
        string fen;
        for (int i = 0; i < 6; i++) {
          if (iss >> token && token != "moves")
            fen += token + " ";
          else
            break;
        }
        // cout << "--\n{" << fen << "}\n--\n";
        board.load_fen(fen);
      } else if (token == "startpos") {
        transpositions.clear();
        board.load_startpos();
        iss >> token;
      }
      if (token == "moves") {
        while (iss >> token) {
          if (board.make_move(token))
            transpositions.insert(board.pos_hash());
          else
            break;
        }
      }
    } else if (token == "go") {
      // example: go wtime 56329 btime 86370 winc 1000 binc 1000
      ai.search_type = Time_per_game;
      ai.set_clock(30000, 30000, 0, 0);
      ai.max_depth = 100;
      while (iss >> token) {
        if (token == "searchmoves") {
        } else if (token == "ponder") {
          ai.search_type = Ponder;
        } else if (token == "wtime") {
          iss >> ai.wtime;
        } else if (token == "btime") {
          iss >> ai.btime;
        } else if (token == "winc") {
          iss >> ai.winc;
        } else if (token == "binc") {
          iss >> ai.binc;
        } else if (token == "movestogo") {
        } else if (token == "depth") {
          iss >> ai.max_depth;
          ai.search_type = Fixed_depth;
        } else if (token == "nodes") {
        } else if (token == "mate") {
          ai.search_type = Mate;
        } else if (token == "movetime") {
          iss >> ai.mtime;
          ai.search_type = Time_per_move;
        } else if (token == "infinite") {
          ai.search_type = Infinite;
        } else if (token == "startpos") {
          transpositions.clear();
          board.load_startpos();
          iss >> token;
          if (token == "moves") {
            while (iss >> token) {
              if (board.make_move(token))
                transpositions.insert(board.pos_hash());
              else
                break;
            }
          }
        }
      }
      cout << "timings: " << ai.wtime << " " << ai.btime << " " << ai.winc
           << " " << ai.binc << endl;
      auto [bestmove, bestscore] = ai.search(transpositions);
      cout << "bestmove " << board.to_uci(bestmove) << endl;
    } else if (token == "stop") {
    } else if (token == "ponderhit") {
    } else if (token == "debug") {
      iss >> token;
      debug_mode = token == "on";
    } else if (token == "isready") {
      cout << "readyok" << endl;
    } else if (token == "setoption") {
    } else if (token == "register") {
    } else if (token == "d") {
      board.print();
      cout << "fen: " << board.to_fen() << endl;
    } else if (token == "quit") {
      break;

      // additional commands
    } else if (token == "pseudo") {
      Game temp;
      temp.board = board;
      temp.movelist = board.generate_pseudo_moves();
      temp.print_movelist();
    } else if (token == "legal") {
      Game temp;
      temp.board = board;
      temp.movelist = board.generate_legal_moves();
      temp.print_movelist();
    } else if (token == "lichess") {
      string fen = board.to_fen();
      replace(fen.begin(), fen.end(), ' ', '_');
      cout << "https://lichess.org/analysis/" << fen << endl;
    } else if (token == "perft" || token == "divide") {
      int depth;
      iss >> depth;
      board.divide(depth);
    } else if (token == "debugmoves") {
      while (iss >> token) {
        if (board.make_move(token)) {
          board.print();
          cout << "fen: " << board.to_fen() << endl;
        } else {
          break;
        }
      }
      board.load_startpos();
    } else if (token == "eval") {
      ai.print_eval();
    } else {
      cout << "Invalid command: " << line << endl;
    }
  }
}

int main() {
  srand(time(0));
  // test_navigation();
  uci();
}