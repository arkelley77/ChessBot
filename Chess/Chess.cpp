// Chess.cpp : Defines the entry point for the application.
//

#include "Chess.h"

using namespace std;

string printed;
bool running = true;

void print(string s, const char* end="\n") {
  cout << s << end << flush;
  printed += s + end;
}
void delete_printed() {
  for (auto it : printed) cout << '\b';
}

vector<string> split(string str, char c) {
  if (str.size() == 0) return {};
  size_t start = 0, end = 0;
  vector<string> out;
  for (; end < str.size(); ++end) {
    if (str[end] == c) {
      out.push_back(str.substr(start, end - start));
      start = end + 1;
    }
  }
  if (start < end) out.push_back(str.substr(start, end - start));
  return out;
}

string tolower(string str) {
  for (char& c : str) c = tolower(c);
  return str;
}

void getline(string& to) {
  print(">> ", "");
  getline(cin, to);
  printed += to + '\n';
  to = tolower(to);
  if (to == "q" || to == "quit" || cin.eof()) running = false;
}

int main()
{
  Board board;
  string input;
  board.setUp();

  print(board.toString() + "\nSelect a piece by typing 'sel' or 'select',\nthen the adress of the piece.\nFor example: sel e2\nTo quit, type 'q' or 'quit' at any time.\n");
  getline(input);
  while (running) {
    vector<Move> valid_moves = board.getAllMoves();
    vector<string> input_split = split(input, ' ');
    if (input_split.size() == 0) {
      print("input split size 0");
    }
    else if (input_split.size() == 1) {
      print("input split size 1");
    }
    else if (input_split.size() == 2) {
      if (input_split[0] == "sel" || input_split[0] == "select") {
        try {
          int from = Indexing::stringToIdx(input_split[1]);
          delete_printed();
          string board_out = Userspace::getPrettyPrint(board.getBuffer(from));
          cout << board_out;
          cout << printed.substr(board_out.size(), printed.size() - board_out.size());
          print("Where would you like to move to?\n(Drop the piece by typing an invalid square)");
          getline(input);
          int to = Indexing::stringToIdx(input);
          Move move(from, to);
          if ((board.getPiece(from) == Piece::white_pawn && Indexing::getRank(to) == Indexing::r8)
            || (board.getPiece(from) == Piece::black_pawn && Indexing::getRank(to) == Indexing::r1)) {
            move.setSpecial(Move::promo);
          retry_promotion:
            print("What would you like to promote to? ('knight', 'bishop', 'rook', or 'queen')");
            getline(input);
            if (input == "knight") move.setPromoType(Move::knight);
            else if (input == "bishop") move.setPromoType(Move::bishop);
            else if (input == "rook") move.setPromoType(Move::rook);
            else if (input == "queen") move.setPromoType(Move::queen);
            else if (running) goto retry_promotion;
          }
          else if ((board.getPiece(from) == Piece::white_pawn && Indexing::getRank(to) == Indexing::r4 && Indexing::getRank(from) == Indexing::r2)) {
            if (board.getPiece(to + Indexing::west) == Piece::black_pawn || board.getPiece(to + Indexing::east) == Piece::black_pawn) {
              move.setSpecial(Move::en_passant);
            }
          }
          else if ((board.getPiece(from) == Piece::black_pawn && Indexing::getRank(to) == Indexing::r5 && Indexing::getRank(from) == Indexing::r7)) {
            if (board.getPiece(to + Indexing::west) == Piece::white_pawn || board.getPiece(to + Indexing::east) == Piece::white_pawn) {
              move.setSpecial(Move::en_passant);
            }
          }
          else if (board.getPiece(from) == Piece::white_king || board.getPiece(from) == Piece::black_king) {
            if (to - from == 2 * Indexing::east || from - to == 2 * Indexing::west) {
              move.setSpecial(Move::castling);
            }
          }
          bool valid = false;
          for (Move& mov : valid_moves) {
            if (mov == move) {
              board.executeMove(move);
              break;
            }
          }
        }
        catch (std::exception& e) { print("bad idx"); }
      }
    }
    delete_printed();
    printed.clear();
    print(board.toString() + "\nSelect a piece by typing 'sel' or 'select',\nthen the adress of the piece.\nFor example: sel e2\nTo quit, type 'q' or 'quit' at any time.");
    getline(input);
  }

end:
  return 0;
}
