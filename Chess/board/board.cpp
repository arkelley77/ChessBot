#include "board.h"

using namespace Binary;
using namespace Bitboards;
using namespace Indexing;

void Board::setUp(const char* fen) {
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  clear();

#define THROW_INVALID_FEN throw std::invalid_argument("Invalid FEN")
  if (fen == nullptr) THROW_INVALID_FEN;
  size_t i = 0;

  // parse the piece placement data ("rnbqkbnr/pppppppp/...")
  int col = 0, row = 7;
  for (; fen[i] != ' '; ++i) {
    if (fen[i] == '\0') THROW_INVALID_FEN; // fen ended early
    else if ('0' < fen[i] && fen[i] <= '8') {
      // skip columns according to the specified number
      col += fen[i] - '0';
    }
    else if (fen[i] == '/') {
      // skip to the next row
      --row;
      col = 0;
    }
    else if (Piece::isValidName(fen[i])) {
      if (col < 0 || col >= 8 || row < 0 || row >= 8) THROW_INVALID_FEN; // too many board squares
      pushPiece(static_cast<Piece::Name>(fen[i]), getIDX(row, col));
      ++col;  // move to the next square
    }
    else THROW_INVALID_FEN; // not sure why this would be triggered
  }

  // parse the side to move
  ++i;
  switch (fen[i]) {
  case 'w':
    makeWhitesMove();
    break;
  case 'b':
    makeBlacksMove();
    break;
  default: THROW_INVALID_FEN;
  }
  ++i;
  if (fen[i] == '\0') THROW_INVALID_FEN;
  ++i;
  for (; fen[i] != ' '; ++i) {
    switch (fen[i]) {
    case 'K': flags |= w_castle_kingside;
      break;
    case 'Q': flags |= w_castle_queenside;
      break;
    case 'k': flags |= b_castle_kingside;
      break;
    case 'q': flags |= b_castle_queenside;
      break;
    default: THROW_INVALID_FEN;
    }
  }
#undef THROW_INVALID_FEN
}

Piece::Name Board::rmPiece(int idx) noexcept {
  bb square = idxToBoard(idx);
  Piece::Name old_piece = getPiece(idx);

  if (Piece::isWhite(old_piece)) {
    bitboards[white] &= ~square;
  }
  else if (Piece::isBlack(old_piece)) {
    bitboards[black] &= ~square;
  }
  else return Piece::square;

  switch (Piece::getType(old_piece)) {
  case Piece::pawn:
    bitboards[pawns] &= ~square;
    break;
  case Piece::rook:
    bitboards[rooks] &= ~square;
    break;
  case Piece::knight:
    bitboards[knights] &= ~square;
    break;
  case Piece::bishop:
    bitboards[bishops] &= ~square;
    break;
  case Piece::queen:
    bitboards[queens] &= ~square;
    break;
  case Piece::king:
    bitboards[kings] &= ~square;
    break;
  }

  mailbox[idx] = Piece::square;

  return old_piece;
}

void Board::dropPiece(Piece::Name p, int idx) noexcept {
  bb square = idxToBoard(idx);

  if (Piece::isWhite(p)) {
    bitboards[white] |= square;
  }
  else if (Piece::isBlack(p)) {
    bitboards[black] |= square;
  }
  else return;

  switch (Piece::getType(p)) {
  case Piece::pawn:
    bitboards[pawns] |= square;
    break;
  case Piece::rook:
    bitboards[rooks] |= square;
    break;
  case Piece::knight:
    bitboards[knights] |= square;
    break;
  case Piece::bishop:
    bitboards[bishops] |= square;
    break;
  case Piece::queen:
    bitboards[queens] |= square;
    break;
  case Piece::king:
    bitboards[kings] |= square;
    break;
  }

  mailbox[idx] = p;
}

vector<Move> Board::getAllMoves() const noexcept {
  using namespace Movegen;
  std::vector<Move> moves;
  bb my_pieces = bitboards[isWhitesMove()];
  bb enemy_pieces = bitboards[!isWhitesMove()];
  bb empty_squares = ~my_pieces & ~enemy_pieces;

  bb bishoplike = bitboards[bishops] | bitboards[queens];
  bb rooklike = bitboards[rooks] | bitboards[queens];

  bb my_pawns = bitboards[pawns] & my_pieces;
  bb my_knights = bitboards[knights] & my_pieces;
  bb my_bishops = bitboards[bishops] & my_pieces;
  bb my_rooks = bitboards[rooks] & my_pieces;
  bb my_queens = bitboards[queens] & my_pieces;
  bb my_king = bitboards[kings] & my_pieces;

  bb enemy_pawns = bitboards[pawns] & enemy_pieces;
  bb enemy_knights = bitboards[knights] & enemy_pieces;
  bb enemy_bishoplike = bishoplike & enemy_pieces;
  bb enemy_rooklike = rooklike & enemy_pieces;
  bb enemy_king = bitboards[kings] & enemy_pieces;

  bb not_pinned = ~findPinnedTo(my_king, empty_squares, my_pieces
    , enemy_bishoplike, enemy_rooklike);
  bb valid_move_targets = (isWhitesMove())
    ? legalMoveTargetsWhite(my_king, empty_squares, enemy_pawns
      , enemy_knights, enemy_bishoplike, enemy_rooklike)
    : legalMoveTargetsBlack(my_king, empty_squares, enemy_pawns
      , enemy_knights, enemy_bishoplike, enemy_rooklike);
  bb under_threat = (isWhitesMove())
    ? genAllThreatsBlack(enemy_pawns, enemy_knights, enemy_bishoplike
      , enemy_rooklike, enemy_king, empty_squares)
    : genAllThreatsWhite(enemy_pawns, enemy_knights, enemy_bishoplike
      , enemy_rooklike, enemy_king, empty_squares);

  my_pawns &= not_pinned;
  my_knights &= not_pinned;
  my_bishops &= not_pinned;
  my_rooks &= not_pinned;
  my_queens &= not_pinned;

  bb valid_cap_targets = valid_move_targets & enemy_pieces;
  bb valid_quiet_targets = valid_move_targets & empty_squares;

  size_t i = 0;

  // capture moves
  if (isWhitesMove()) genPawnCapsN(my_pawns, valid_cap_targets, moves);
  else genPawnCapsS(my_pawns, valid_cap_targets, moves);
  genKnightCaps(my_knights, valid_cap_targets, moves);
  genBishopCaps(my_bishops, empty_squares, valid_cap_targets, moves);
  genRookCaps(my_rooks, empty_squares, valid_cap_targets, moves);
  genQueenCaps(my_queens, empty_squares, valid_cap_targets, moves);

  genKnightMoves(my_knights, valid_quiet_targets, moves);
  genBishopMoves(my_bishops, valid_quiet_targets, moves);
  genRookMoves(my_rooks, valid_quiet_targets, moves);
  genQueenMoves(my_queens, valid_quiet_targets, moves);

  if (isWhitesMove()) {
    genPawnPushesN(my_pawns, valid_quiet_targets, enemy_pawns, moves);
    genKingMoves(my_king, empty_squares, ~under_threat, enemy_pieces
      , flags & w_castle_queenside, flags & w_castle_kingside, moves);
  }
  else {
    genPawnPushesS(my_pawns, valid_quiet_targets, enemy_pawns, moves);
    genKingMoves(my_king, empty_squares, ~under_threat, enemy_pieces
      , flags & b_castle_queenside, flags & b_castle_kingside, moves);
  }
  return moves;
}

Piece::Name Board::executeMove(Move move) noexcept {
  using Indexing::north, Indexing::south;
  int from = move.getFromSquare(), to = move.getToSquare();
  Move::Special special = move.getSpecial();

  Piece::Name piece = rmPiece(from);
  en_passant_square = -1;

  Piece::Name on_dest = rmPiece(to);

  switch (special) {
  case Move::promo: piece = Piece::makePiece(move.getPromoPieceType(), isWhitesMove());
    break;
  case Move::en_passant: en_passant_square = from + (isWhitesMove()) ? north : south;
    break;
  case Move::castling:
    Indexing::RankIDX from_rank = (isWhitesMove()) ? Indexing::RankIDX::r1 : Indexing::RankIDX::r8;
    int rook_from, rook_to;
    if (to > from) {
      rook_from = from_rank + Indexing::FileIDX::h;
      rook_to = from_rank + Indexing::FileIDX::f;
    }
    else {
      rook_from = from_rank + Indexing::FileIDX::a;
      rook_to = from_rank + Indexing::FileIDX::d;
    }
    pushPiece(rmPiece(rook_from), rook_to);
  }
  
  pushPiece(piece, to);
  switchMoveSide();
  return on_dest;
}

std::string Board::getBuffer() const noexcept {
  std::string buf(64, ' ');
  const char pieces[2][6] = { { 'p', 'n', 'b', 'r', 'q', 'k' },
                              { 'P', 'N', 'B', 'R', 'Q', 'K'} };
  for (size_t i = pawns; i < num_bitboards; ++i) {
    Bitboards::buffer(bitboards[i] & bitboards[white], buf, pieces[white][i - pawns]);
    Bitboards::buffer(bitboards[i] & bitboards[black], buf, pieces[black][i - pawns]);
  }
  return buf;
}

std::string Board::getBuffer(std::vector<Move>& moves) const noexcept {
  std::string buf = getBuffer();
  for (Move& move : moves) {
    Bitboards::buffer(Bitboards::idxToBoard(move.getToSquare()), buf);
  }
  return buf;
}

std::string Board::getBuffer(int selected_piece) const noexcept {
  std::string buf = getBuffer();
  for (Move& move : getAllMoves()) {
    if (move.getFromSquare() == selected_piece) {
      Bitboards::buffer(Bitboards::idxToBoard(move.getToSquare()), buf);
    }
  }
  return buf;
}
