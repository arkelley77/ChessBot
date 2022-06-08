#include "bitboard.h"

BitBoard::BitBoard() noexcept {
  clear();
}

BitBoard::BitBoard(BitBoard& to_copy) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = to_copy.boards[i];
  }
}

void BitBoard::clear() noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = 0ULL;
  }
}

inline void BitBoard::pushPiece(Piece::Name p, bb square) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] &= ~square; // empty the square in all bitboards
  }
  if (Piece::isSquare(p)) return; // if empty
  
  if (Piece::isBlack(p)) {
    boards[black] |= square;
  }
  else (Piece::isWhite(p)) {
    boards[white] |= square;
  }
  
  switch (Piece::getType(p)) {
  case Piece::pawn:
    boards[pawns] |= square;
    break;
  case Piece::rook:
    boards[rooks] |= square;
    break;
  case Piece::knight:
    boards[knights] |= square;
    break;
  case Piece::bishop:
    boards[bishops] |= square;
    break;
  case Piece::queen:
    boards[queens] |= square;
    break;
  case Piece::king:
    boards[kings] |= square;
    break;
  }
}
inline void BitBoard::pushPiece(Piece::Name p, int idx) noexcept {
  pushPiece(p, idxToBoard(idx));
}

inline void rmPiece(bb square) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] &= ~square;
  }
}
inline void rmPiece(int idx) noexcept {
  rmPiece(idxToBoard(idx));
}

inline void replacePiece(bb square, Piece::Name p) noexcept {
  rmPiece(square);
  pushPiece(p, square);
}

void BitBoard::setUp() noexcept {
  boards[white] = r1 | r2; // bottom 2 rows
  boards[black] = r7 | r8; // top 2 rows

  boards[pawns] = r2 | r7;
  boards[kings] = e1 | e8;
  boards[queens] = d1 | d8;
  boards[rooks] = a1 | h1 | a8 | h8;
  boards[knights] = b1 | g1 | b8 | g8;
  boards[bishops] = c1 | f1 | c8 | f8;
}

void BitBoard::setUp(const char* fen) {
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  clear();

#define THROW_INVALID_FEN throw std::invalid_argument("Invalid FEN")
  BitBoard();
  if (fen == nullptr) THROW_INVALID_FEN;
  size_t i = 0;

  // parse the piece placement data ("rnbqkbnr/pppppppp/...")
  bb col = a, row = r8;
  for (; fen[i] != ' '; ++i) {
    if (fen[i] == '\0') THROW_INVALID_FEN; // fen ended early
    else if ('0' < fen[i] && fen[i] <= '8') {
      // skip columns according to the specified number
      col = col << ((fen[i] - '0') * east);
    }
    else if (fen[i] == '/') {
      // skip to the next row
      row = row >> north;
      col = a;
    }
    else if (Piece::isValidName(fen[i])) {
      if (!(col & row)) THROW_INVALID_FEN; // too many board squares
      pushPiece(static_cast<Piece::Name>(fen[i]), col & row);
      col = col << east;  // move to the next square
    }
    else THROW_INVALID_FEN; // not sure why this would be triggered
  }
}

std::string& BitBoard::toString(bb board) {
  std::string buf(71, ' '); // 64 squares + 7 newlines = 71 chars
  for (size_t i = 8; i < 71; i += 9) buf[i] = '\n';

  while (board) {
    size_t row = static_cast<size_t>(8) - getRow(board) - 1;
    size_t col = getCol(board);
    buf[row * 9 + col] = '1'; // has to be (row * 9) because of the newlines
    board ^= Binary::isolateMS1B(board);
  }

  return buf;
}

std::string BitBoard::toString() const noexcept {
  std::string buf(71, ' '); // 64 pieces + 7 newlines = 71 chars
  const char piece_names[2][6] = { { 'p', 'n', 'b', 'r', 'q', 'k' },
                                   { 'P', 'N', 'B', 'R', 'Q', 'K' } };
  for (size_t i = 8; i < 71; i += 9) buf[i] = '\n';

  for (size_t i = pawns; i < num_boards; ++i) {
    bb board = boards[i];
    while (board) {
      size_t row = static_cast<size_t>(8) - getRow(board) - 1;
      size_t col = getCol(board);
      if (Binary::isolateMS1B(board) & boards[white]) {
        // has to be (row * 9) because of the newlines
        buf[row * 9 + col] = piece_names[white][i - pawns];
      }
      else {
        buf[row * 9 + col] = piece_names[black][i - pawns];
      }
      board ^= Binary::isolateMS1B(board);
    }
  }

  return buf;
}
