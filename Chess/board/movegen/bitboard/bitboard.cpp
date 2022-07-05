#include "bitboard.h"

using std::vector;
using namespace Binary;
using namespace Indexing;

vector<BitBoard::bb> BitBoard::getEachPiece(BitBoard::bb board) noexcept {
  // the max # of pieces of a given type is 10
  // because theoretically you could keep both rooks
  // and promote all pawns to rooks
  vector<bb> pieces;
  while (board) {
    bb piece = isolateMS1B(board);
    pieces.push_back(piece);
    board &= ~piece;
  }
  return pieces;
}

void BitBoard::setUp() noexcept {
  clear();

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
      shift(col, (fen[i] - '0') * east);
    }
    else if (fen[i] == '/') {
      // skip to the next row
      shift(row, south);
      col = a;
    }
    else if (Piece::isValidName(fen[i])) {
      if (!(col & row)) THROW_INVALID_FEN; // too many board squares
      pushPiece(static_cast<Piece::Name>(fen[i]), col & row);
      shift(col, east);  // move to the next square
    }
    else THROW_INVALID_FEN; // not sure why this would be triggered
  }
}

void BitBoard::pushPiece(Piece::Name p, const bb& square) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] &= ~square; // empty the square in all bitboards
  }
  if (Piece::isSquare(p)) return; // if empty
  else if (Piece::isWhite(p)) {
    boards[white] |= square;
  }
  else if (Piece::isBlack(p)) {
    boards[black] |= square;
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
  default:
    return;
  }
}

void BitBoard::rmPiece(const bb& square) noexcept {
  for (size_t i = 0; i < num_boards; ++i) {
    boards[i] &= ~square;
  }
}

void BitBoard::replacePiece(Piece::Name p, const bb& square) noexcept {
  rmPiece(square);
  pushPiece(p, square);
}

BitBoard::bb BitBoard::genPawnSingles(bool white_to_move) const noexcept {
  bb moves = boards[pawns] & boards[white_to_move];
  if (!moves) return moves;

  constexpr bb n_mask = ~r1, s_mask = ~r8;
  bb valid_targets = ~boards[white] & ~boards[black];

  if (white_to_move) moves = shifted(moves, north) & valid_targets & n_mask;
  else moves = shifted(moves, south) & valid_targets & s_mask;
  return moves;
}
BitBoard::bb BitBoard::genPawnDoubles(bool white_to_move) const noexcept {
  bb moves = genPawnSingles(white_to_move);
  if (!moves) return moves;

  bb valid_targets = ~boards[white] & ~boards[black];

  if (white_to_move) moves = shifted(moves & r3, north) & valid_targets;
  else moves = shifted(moves & r6, south) & valid_targets;
  return moves;
}

BitBoard::bb BitBoard::genPawnPushes(bool white_to_move) const noexcept {
  bb moves = boards[pawns] & boards[white_to_move];
  bb double_moves;
  if (!moves) return moves;

  constexpr bb n_mask = ~r1, s_mask = ~r8;
  bb valid_targets = ~boards[white] & ~boards[black];
  direction dir = (white_to_move) ? north : south;
  bb dbl_row = (white_to_move) ? r3 : r6;

  // generate single moves
  moves = shifted(moves, dir) & valid_targets;

  // generate double moves
  double_moves = shifted(moves & dbl_row, north); // don't have to mask r1 bc of the "& r3"
  
  moves |= double_moves & valid_targets;
  return moves;
}

vector<Move> BitBoard::genPawnPushMoves(bool white_to_move) const noexcept {
  bb singles = genPawnSingles(white_to_move);
  bb doubles = genPawnDoubles(white_to_move);
  vector<Move> moves;
  bb enemy_pawns = boards[pawns] & boards[!white_to_move];
  bb en_passant = (shifted(doubles, west) | shifted(doubles, east)) & enemy_pawns;

  direction backwards = (white_to_move) ? south : north;

  for (auto file : { a, b, c, d, e, f, g, h }) {
    bb single_target = singles & file;
    bb double_target = doubles & file;
    if (!single_target) continue;

    int to_one = indexOfMS1B(single_target);
    int from = to_one + backwards;
    if (single_target & r8) {
      moves.push_back(Move(from, to_one, Piece::queen, Move::promo));
      moves.push_back(Move(from, to_one, Piece::rook, Move::promo));
      moves.push_back(Move(from, to_one, Piece::bishop, Move::promo));
      moves.push_back(Move(from, to_one, Piece::knight, Move::promo));
      continue;
    }
    moves.push_back(Move(from, to_one));
    if (double_target) {
      if (en_passant & file) {
        // set the en passant flag if the pawn can be captured en passant
        moves.push_back(Move(from, indexOfMS1B(double_target), Move::en_passant));
        continue;
      }
      moves.push_back(Move(from, indexOfMS1B(double_target)));
    }
  }
  return moves;
}

vector<BitBoard::bb> BitBoard::genPawnCaptures(bool white_to_move, const bb& en_passant_square) const noexcept {
  vector<bb> moves(2);
  moves[0] = moves[1] = boards[pawns] & boards[white_to_move];
  if (!moves[0]) return moves;


  constexpr bb n_mask = ~r1, s_mask = ~r8;
  bb valid_targets = (boards[!white_to_move]) | en_passant_square;

  // we don't have to mask ~r1 & ~r8 because it's illegal
  // for a pawn to be on its destination square

  if (white_to_move) {
    moves[0] = (shifted(moves[0], n_east)) & valid_targets & n_mask;
    moves[1] = (shifted(moves[1], n_west)) & valid_targets & n_mask;
  }
  else {
    moves[0] = (shifted(moves[0], s_east)) & valid_targets & s_mask;
    moves[1] = (shifted(moves[1], s_west)) & valid_targets & s_mask;
  }
  return moves;
}

vector<Move> BitBoard::genPawnCapMoves(bool white_to_move, int en_passant_target) const noexcept {
  vector<bb> caps = genPawnCaptures(white_to_move, 1ULL << en_passant_target);
  vector<Move> moves;

  if (white_to_move) {
    // genPawnCaptures() goes east first
    for (auto file : { b, c, d, e, f, g, h }) {
      bb target = caps[0] & file;
      if (!target) continue;

      int from_square = indexOfMS1B(shifted(target, s_west));
      int to_square = indexOfMS1B(target);

      moves.push_back(Move(from_square, to_square, Move::capture));
    }
    for (auto file : { a, b, c, d, e, f, g }) {
      bb target = caps[1] & file;
      if (!target) continue;

      int from_square = indexOfMS1B(shifted(target, s_east));
      int to_square = indexOfMS1B(target);

      moves.push_back(Move(from_square, to_square, Move::capture));
    }
  }
  else {
    // genPawnCaptures() goes east first
    for (auto file : { b, c, d, e, f, g, h }) {
      bb target = caps[0] & file;
      if (!target) continue;

      int from_square = indexOfMS1B(shifted(target, n_west));
      int to_square = indexOfMS1B(target);

      moves.push_back(Move(from_square, to_square, Move::capture));
    }
    for (auto file : { a, b, c, d, e, f, g }) {
      bb target = caps[1] & file;
      if (!target) continue;

      int from_square = indexOfMS1B(shifted(target, n_east));
      int to_square = indexOfMS1B(target);

      moves.push_back(Move(from_square, to_square, Move::capture));
    }
  }
  return moves;
}

vector<BitBoard::bb> BitBoard::genPawnMoves(bool white_to_move, const bb& en_passant_square) const noexcept {
  vector<bb> moves = genPawnCaptures(white_to_move, en_passant_square);
  moves.push_back(genPawnPushes(white_to_move));
  return moves;
}
BitBoard::bb BitBoard::genPawnThreats(const bb& pawn, bool white_to_move) const noexcept {
  return (white_to_move) ? (shifted(pawn, n_east) | shifted(pawn, n_west))
    : (shifted(pawn, s_east) | shifted(pawn, s_west));
}

vector<BitBoard::bb> BitBoard::genKnightMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[knights] & boards[white_to_move]);
  bb valid_targets = ~boards[white_to_move];
  for (bb& knight : moves) knight = genKnightThreats(knight) & valid_targets;
  
  return moves;
}
BitBoard::bb BitBoard::genKnightThreats(const bb& knight) const noexcept {
  constexpr bb n_mask = ~r1, n_n_mask = n_mask & ~r2, s_mask = ~r8, s_s_mask = s_mask & ~r7;
  bb move_to = (shifted(knight, n_n_east) | shifted(knight, n_n_west)) & n_n_mask; // 2 steps north
  move_to |= (shifted(knight, s_s_east) | shifted(knight, s_s_west)) & s_s_mask; // 2 steps south
  move_to |= (shifted(knight, e_n_east) | shifted(knight, w_n_west)) & n_mask; // 1 step north
  move_to |= (shifted(knight, e_s_east) | shifted(knight, w_s_west)) & s_mask; // 1 step south
  return move_to;
}

vector<BitBoard::bb> BitBoard::genBishopMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[bishops] & boards[white_to_move]);
  bb valid_targets = ~boards[white_to_move];
  for (bb& bishop : moves) bishop = genBishopThreats(bishop) & valid_targets;
  
  return moves;
}
BitBoard::bb BitBoard::genBishopThreats(const bb& bishop) const noexcept {
  bb empty_squares = ~boards[white] & ~boards[black];
  constexpr bb n_mask = ~r1, s_mask = ~r8;

  bb n_e = obstructedFill(bishop, empty_squares & n_mask, n_east);
  n_e = shifted(n_e, n_east) & n_mask;

  bb n_w = obstructedFill(bishop, empty_squares & n_mask, n_west);
  n_w = shifted(n_w, n_west) & n_mask;

  bb s_e = obstructedFill(bishop, empty_squares & s_mask, s_east);
  s_e = shifted(s_e, s_east) & s_mask;

  bb s_w = obstructedFill(bishop, empty_squares & s_mask, s_west);
  s_w = shifted(s_w, s_west) & s_mask;

  return n_e | n_w | s_e | s_w;
}

vector<BitBoard::bb> BitBoard::genRookMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[rooks] & boards[white_to_move]);
  bb valid_targets = ~boards[white_to_move];
  for (bb& rook : moves) rook = genRookThreats(rook) & valid_targets;
  
  return moves;
}
BitBoard::bb BitBoard::genRookThreats(const bb& rook) const noexcept {
  bb empty_squares = ~boards[white] & ~boards[black];
  constexpr bb n_mask = ~r1, s_mask = ~r8;

  bb n = obstructedFill(rook, empty_squares & n_mask, north);
  n = shifted(n, north) & n_mask;

  bb s = obstructedFill(rook, empty_squares & s_mask, south);
  s = shifted(s, north) & s_mask;

  bb e = obstructedFill(rook, empty_squares, east);
  shift(e, east);

  bb w = obstructedFill(rook, empty_squares, west);
  shift(w, west);

  return n | s | e | w;
}

vector<BitBoard::bb> BitBoard::genQueenMoves(bool white_to_move) const noexcept {
  vector<bb> moves = getEachPiece(boards[queens] & boards[white_to_move]);
  bb valid_targets = ~boards[white];
  for (bb& queen : moves) queen = genQueenThreats(queen) & valid_targets;
  
  return moves;
}
BitBoard::bb BitBoard::genQueenThreats(const bb& queen) const noexcept {
  return genBishopThreats(queen) | genRookThreats(queen);
}

BitBoard::bb BitBoard::genKingMoves(bool white_to_move) const noexcept {
  bb king = boards[kings] & boards[white_to_move];
  bb valid_targets = ~boards[white_to_move];
  return genKingThreats(king) & valid_targets;
}
BitBoard::bb BitBoard::genKingThreats(const bb& king) const noexcept {
  bb moves = king;

  moves |= shifted(moves, north) & ~r1;
  moves |= shifted(moves, south) & ~r8;
  moves |= shifted(moves, east);
  moves |= shifted(moves, west);

  return moves & ~king;
}
BitBoard::bb BitBoard::genLegalKingMoves(bool white_to_move, bool castle_queenside, bool castle_kingside) const noexcept {
  bb king = boards[kings] & boards[white_to_move];
  bb valid_squares = ~boards[white_to_move] & ~genThreatsFrom(!white_to_move);
  bb moves = genKingMoves(white_to_move) & valid_squares;
  bb castle_moves = shifted(moves, east) | shifted(moves, west);
  if (white_to_move) {
    castle_moves &= r1;
  }
  else {
    castle_moves &= r8;
  }
  castle_moves &= valid_squares;
  return moves | castle_moves;
}

BitBoard::bb BitBoard::genThreatsFrom(bool white_to_move) const noexcept {
  bb threats = genPawnThreats(boards[pawns] & boards[white_to_move], white_to_move);
  threats |= genKnightThreats(boards[knights] & boards[white_to_move]);
  threats |= genBishopThreats(boards[bishops] & boards[white_to_move]);
  threats |= genRookThreats(boards[rooks] & boards[white_to_move]);
  threats |= genQueenThreats(boards[queens] & boards[white_to_move]);
  threats |= genKingThreats(boards[kings] & boards[white_to_move]);
  return threats;
}

std::string BitBoard::toString(bb board, char one) noexcept {
  std::string buf(71, ' '); // 64 squares + 7 newlines = 71 chars
  for (size_t i = 8; i < 71; i += 9) buf[i] = '\n';

  while (board) {
    size_t row = static_cast<size_t>(8) - getRow(board) - 1;
    size_t col = getCol(board);
    buf[row * 9 + col] = one; // has to be (row * 9) because of the newlines
    board &= ~isolateMS1B(board);
  }

  return buf;
}

std::string BitBoard::toString() const noexcept {
  std::string buf(71, ' '); // 64 pieces + 7 newlines = 71 chars
  const char piece_names[2][6] = { { 'p', 'n', 'b', 'r', 'q', 'k' },
                                      { 'P', 'N', 'B', 'R', 'Q', 'K'}};
  for (size_t i = 8; i < 71; i += 9) buf[i] = '\n';

  for (size_t i = pawns; i < num_boards; ++i) {
    bb board = boards[i];
    while (board) {
      size_t row = static_cast<size_t>(8) - getRow(board) - 1;
      size_t col = getCol(board);
      if (isolateMS1B(board) & boards[white]) {
        // has to be (row * 9) because of the newlines
        buf[row * 9 + col] = piece_names[white][i - pawns];
      }
      else {
        buf[row * 9 + col] = piece_names[black][i - pawns];
      }
      board &= ~isolateMS1B(board);
    }
  }

  return buf;
}
