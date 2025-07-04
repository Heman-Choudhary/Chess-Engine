#include <chrono>
#include <fstream>

#include "board.hpp"

int main(int argc, char* argv[]) {
  // cout.setstate(ios_base::failbit);
  string filename = "perftsuite.epd";
  if (argc > 1) filename = argv[1];
  ifstream epd(filename);
  Board b;
  string line;
  int k = 1000;
  int count = 1;
  double total = 0;
  cerr.setstate(ios_base::failbit);
  while (getline(epd, line) && k--) {
    cout << "Case #" << count++ << ":" << endl;
    vector<string> parts;
    int start = 0;
    int end = line.find(" ;");
    do {
      parts.push_back(line.substr(start, end - start));
      start = end + 2;
      end = line.find(" ;", start);
    } while (~end);
    parts.push_back(line.substr(start, end - start));
    b.load_fen(parts[0]);
    cout << "fen: " << parts[0] << endl;
    for (size_t i = 1; i < parts.size(); i++) {
      cout << parts[i] << endl;
      int expected = stoi(parts[i].substr(parts[i].find(" ") + 1));
      cout << "               ";
      if (expected > 1e6) {
        cout << "skipped" << endl;
        continue;
      }
      auto begin = chrono::high_resolution_clock::now();
      int found = b.divide(i);
      auto end = chrono::high_resolution_clock::now();

      auto elapsed =
          chrono::duration_cast<chrono::milliseconds>(end - begin).count() *
          1e-3;
      total += elapsed;

      cout << (expected == found ? "\e[32mPASS\e[0m" : "\e[31mFAIL\e[0m")
           << " [" << elapsed << "s]" << endl;

      if (expected != found) {
        cout << "expected '" << expected << "' but found '" << found << "'"
             << endl;
        return -1;
      }
    }
  }
  cerr.clear();
  cout << "Total time taken: " << total << endl;
  return 0;
}