#include "bitboard.h"

#include <iostream>

BitBoard::BitBoard() noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = 0ULL;
  }
}

BitBoard::BitBoard(BitBoard& to_copy) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = to_copy.boards[i];
  }
}

void BitBoard::setUp() noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = 0ULL;
  }

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
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] = 0ULL;
  }

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

inline void BitBoard::pushPiece(Piece::Name p, bb square) noexcept {
  if (islower(p)) {
    boards[black] |= square;
  }
  else if (isupper(p)) {
    boards[white] |= square;
  }
  else {
    for (size_t i = 0; i < num_boards; ++i) {
      boards[i] &= ~square;
    }
    return;
  }
  switch (tolower(p)) {
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

vector<BitBoard::bb>
  BitBoard::getEachPiece(BitBoard::bb board) noexcept {
  // the max # of pieces of a given type is 10
  // because theoretically you could keep both rooks
  // and promote all pawns to rooks
  vector<bb> pieces;
  while (board) {
    bb piece = Binary::isolateMS1B(board);
    pieces.push_back(piece);
    board ^= piece;
  }
  return pieces;
}

BitBoard::bb BitBoard::genPawnPushes(bool white_to_move) const noexcept {
  bb moves = boards[pawns] & boards[white_to_move];
  bb double_moves;
  if (!moves) return moves;

  constexpr bb n_mask = ~r1, s_mask = ~r8;
  bb valid_targets = ~boards[white] & ~boards[black];

  if (white_to_move) {
    // generate single moves
    moves = (moves << north) & valid_targets & n_mask;

    // generate double moves
    double_moves = ((moves & r3) << north); // don't have to mask r1 bc of the "& r3"
  }
  else {
    moves = (moves >> north) & s_mask;
    moves = moves & valid_targets;

    double_moves = ((moves & r6) >> north);
  }
  double_moves &= valid_targets;
  
  moves |= double_moves;
  return moves;
}

vector<BitBoard::bb>
  BitBoard::genPawnCaptures(bool white_to_move, BitBoard::bb en_passant_square) const noexcept {
  vector<bb> moves(2);
  moves[0] = moves[1] = boards[pawns] & boards[white_to_move];
  if (!moves[0]) return moves;


  constexpr bb n_mask = ~r1, s_mask = ~r8;
  bb valid_targets = (boards[!white_to_move]) | en_passant_square;

  // we don't have to mask ~r1 & ~r8 because it's illegal
  // for a pawn to be on its destination square

  if (white_to_move) {
    moves[0] = (moves[0] << n_east) & valid_targets & n_mask;
    moves[1] = (moves[1] >> s_east) & valid_targets & n_mask;
  }
  else {
    moves[0] = (moves[0] << s_east) & valid_targets & s_mask;
    moves[1] = (moves[1] >> n_east) & valid_targets & s_mask;
  }
  return moves;
}

vector<BitBoard::bb>
  BitBoard::genKnightMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[knights] & boards[white_to_move]);
  bb valid_targets = ~boards[white_to_move];
  constexpr bb n_mask = ~r1, n_n_mask = n_mask & ~r2, s_mask = ~r8, s_s_mask = s_mask & ~r7;

  for (bb& knight : moves) {
    bb move_to = ((knight << n_n_east) | (knight >> s_s_east)) & n_n_mask; // 2 steps north
    move_to |= ((knight << s_s_east) | (knight >> n_n_east)) & s_s_mask; // 2 steps south
    move_to |= ((knight << e_n_east) | (knight >> e_s_east)) & n_mask; // 1 step north
    move_to |= ((knight << e_s_east) | (knight >> e_n_east)) & s_mask; // 1 step south
    move_to &= valid_targets;
    knight = move_to;
  }
  return moves;
}

vector<BitBoard::bb>
  BitBoard::genBishopMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[bishops] & boards[white_to_move]);
  for (bb& bishop : moves) {
    bishop = genSlidingDiagonals(white_to_move, bishop);
  }
  return moves;
}
BitBoard::bb BitBoard::genSlidingDiagonals(bool white_to_move, BitBoard::bb piece) const noexcept {
  bb empty_squares = ~boards[white] & ~boards[black];
  constexpr bb n_mask = ~r1, s_mask = ~r8;

  bb n_e = piece;
  bb smeared_empty = empty_squares & n_mask;
  n_e |= smeared_empty & (n_e << n_east);
  smeared_empty &= (smeared_empty << n_east);
  n_e |= smeared_empty & (n_e << (2 * n_east));
  smeared_empty &= (smeared_empty << (2 * n_east));
  n_e |= smeared_empty & (n_e << (4 * n_east));
  n_e = (n_e << n_east) & n_mask & ~boards[white_to_move];

  bb n_w = piece;
  smeared_empty = empty_squares & n_mask;
  n_w |= smeared_empty & (n_w >> s_east);
  smeared_empty &= (smeared_empty >> s_east);
  n_w |= smeared_empty & (n_w >> (2 * s_east));
  smeared_empty &= (smeared_empty >> (2 * s_east));
  n_w |= smeared_empty & (n_w >> (4 * s_east));
  n_w = (n_w >> s_east) & n_mask & ~boards[white_to_move];

  bb s_e = piece;
  smeared_empty = empty_squares & s_mask;
  s_e |= smeared_empty & (s_e << s_east);
  smeared_empty &= (smeared_empty << s_east);
  s_e |= smeared_empty & (s_e << (2 * s_east));
  smeared_empty &= (smeared_empty << (2 * s_east));
  s_e |= smeared_empty & (s_e << (4 * s_east));
  s_e = (s_e << s_east) & s_mask & ~boards[white_to_move];

  bb s_w = piece;
  smeared_empty = empty_squares & s_mask;
  s_w |= smeared_empty & (s_w >> n_east);
  smeared_empty &= (smeared_empty >> n_east);
  s_w |= smeared_empty & (s_w >> (2 * n_east));
  smeared_empty &= (smeared_empty >> (2 * n_east));
  s_w |= smeared_empty & (s_w >> (4 * n_east));
  s_w = (s_w >> n_east) & s_mask & ~boards[white_to_move];

  return n_e | n_w | s_e | s_w;
}

vector<BitBoard::bb>
  BitBoard::genRookMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[rooks] & boards[white_to_move]);
  for (bb& rook : moves) {
    rook = genSlidingRookMoves(white_to_move, rook);
  }
  return moves;
}
BitBoard::bb BitBoard::genSlidingRookMoves(bool white_to_move, BitBoard::bb piece) const noexcept {
  bb empty_squares = ~boards[white] & ~boards[black];
  constexpr bb n_mask = ~r1, s_mask = ~r8;

  bb n = piece;
  bb smeared_empty = empty_squares & n_mask;
  n |= smeared_empty & (n << north);
  smeared_empty &= (smeared_empty << north);
  n |= smeared_empty & (n << (2 * north));
  smeared_empty &= (smeared_empty << (2 * north));
  n |= smeared_empty & (n << (4 * north));
  n = (n << north) & n_mask & ~boards[white_to_move];

  bb s = piece;
  smeared_empty = empty_squares & n_mask;
  s |= smeared_empty & (s >> north);
  smeared_empty &= (smeared_empty >> north);
  s |= smeared_empty & (s >> (2 * north));
  smeared_empty &= (smeared_empty >> (2 * north));
  s |= smeared_empty & (s >> (4 * north));
  s = (s >> north) & n_mask & ~boards[white_to_move];

  bb e = piece;
  smeared_empty = empty_squares;
  e |= smeared_empty & (e << east);
  smeared_empty &= (smeared_empty << east);
  e |= smeared_empty & (e << (2 * east));
  smeared_empty &= (smeared_empty << (2 * east));
  e |= smeared_empty & (e << (4 * east));
  e = (e << east) & ~boards[white_to_move];

  bb w = piece;
  smeared_empty = empty_squares & s_mask;
  w |= smeared_empty & (w >> east);
  smeared_empty &= (smeared_empty >> east);
  w |= smeared_empty & (w >> (2 * east));
  smeared_empty &= (smeared_empty >> (2 * east));
  w |= smeared_empty & (w >> (4 * east));
  w = (w >> east) & ~boards[white_to_move];

  return n | s | e | w;
}

vector<BitBoard::bb>
  BitBoard::genQueenMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[queens] & boards[white_to_move]);
  for (bb& queen : moves) {
    queen = genSlidingRookMoves(white_to_move, queen)
      | genSlidingDiagonals(white_to_move, queen);
  }
  return moves;
}

BitBoard::bb BitBoard::genKingMoves(bool white_to_move) const noexcept {
  bb moves = boards[kings] & boards[white_to_move];
  bb valid_targets = ~boards[white_to_move];

  moves |= (moves << north) & ~r1;
  moves |= (moves << east);
  moves |= (moves >> east);
  moves |= (moves >> north) & ~r8;

  moves &= valid_targets;
  return moves;
}

std::string BitBoard::toString(bb board, char one) noexcept {
  std::string buf(71, ' '); // 64 squares + 7 newlines = 71 chars
  for (size_t i = 8; i < 71; i += 9) buf[i] = '\n';

  while (board) {
    size_t row = static_cast<size_t>(8) - getRow(board) - 1;
    size_t col = getCol(board);
    buf[row * 9 + col] = one; // has to be (row * 9) because of the newlines
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
