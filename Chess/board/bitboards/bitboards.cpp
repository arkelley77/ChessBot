#include "bitboards.h"

using std::string, std::to_string, std::vector;
using namespace Binary;
using namespace Bitboards;
using namespace Indexing;

vector<bb> Bitboards::getEachPiece(bb board) noexcept {
  vector<bb> pieces;
  while (board) {
    bb piece = isolateMS1B(board);
    pieces.push_back(piece);
    board ^= piece;
  }
  return pieces;
}

void Bitboards::buffer(bb board, string& buf, char one) {
  while (board) {
    size_t idx = indexOfMS1B(board);
    buf[idx] = one;
    board ^= isolateMS1B(board);
  }
}
