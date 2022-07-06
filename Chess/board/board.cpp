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
  if (fen[i] != ' ') THROW_INVALID_FEN;
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
    case '-': break;
    default: THROW_INVALID_FEN;
    }
  }
  ++i;
  if (fen[i] != '-') {
    if ('a' > fen[i] || fen[i] > 'h') THROW_INVALID_FEN;
    ++i;
    if ('1' > fen[i] || fen[i] > '8') THROW_INVALID_FEN;
    en_passant_square = Indexing::getIDX(fen[i] - '1', fen[i - 1] - 'a');
  }
  ++i;
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

  bb en_passant_pawn = 0, en_passant_target = 0;
  if (en_passant_square != -1) {
    en_passant_target = idxToBoard(en_passant_square);
    en_passant_pawn = (isWhitesMove())
      ? idxToBoard(en_passant_square + Indexing::south)
      : idxToBoard(en_passant_square + Indexing::north);
  }

  bb valid_move_targets = (isWhitesMove())
    ? legalMoveTargetsWhite(my_king, empty_squares, enemy_pawns
      , enemy_knights, enemy_bishoplike, enemy_rooklike)
    : legalMoveTargetsBlack(my_king, empty_squares, enemy_pawns
      , enemy_knights, enemy_bishoplike, enemy_rooklike);
  bb under_threat = ((isWhitesMove())
    ? genAllThreatsBlack(enemy_pawns, enemy_knights, enemy_bishoplike
      , enemy_rooklike, enemy_king, empty_squares | my_king)
    : genAllThreatsWhite(enemy_pawns, enemy_knights, enemy_bishoplike
      , enemy_rooklike, enemy_king, empty_squares | my_king));

  bb valid_cap_targets = valid_move_targets & enemy_pieces;
  bb valid_quiet_targets = valid_move_targets & empty_squares;

  bb pinned_nw = findPinnedRailNW(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_ne = findPinnedRailNE(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_sw = findPinnedRailSW(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_se = findPinnedRailSE(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_n = findPinnedRailN(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_s = findPinnedRailS(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_e = findPinnedRailE(my_king, empty_squares, my_pieces, enemy_bishoplike);
  bb pinned_w = findPinnedRailW(my_king, empty_squares, my_pieces, enemy_bishoplike);

  pinned_nw = (countSetBits(pinned_nw & my_pieces) == 1) ? pinned_nw : 0;
  pinned_ne = (countSetBits(pinned_ne & my_pieces) == 1) ? pinned_ne : 0;
  pinned_sw = (countSetBits(pinned_sw & my_pieces) == 1) ? pinned_sw : 0;
  pinned_se = (countSetBits(pinned_se & my_pieces) == 1) ? pinned_se : 0;
  pinned_n = (countSetBits(pinned_n & my_pieces) == 1) ? pinned_n : 0;
  pinned_s = (countSetBits(pinned_s & my_pieces) == 1) ? pinned_s : 0;
  pinned_e = (countSetBits(pinned_e & my_pieces) == 1) ? pinned_e : 0;
  pinned_w = (countSetBits(pinned_w & my_pieces) == 1) ? pinned_w : 0;

  bb pinned_bishop_rails = pinned_nw | pinned_ne | pinned_sw | pinned_se;
  bb pinned_bishop_cap_targets = valid_cap_targets & pinned_bishop_rails;
  bb pinned_bishop_quiet_targets = valid_quiet_targets & pinned_bishop_rails;
  bb pinned_rook_rails = pinned_n | pinned_s | pinned_e | pinned_w;
  bb pinned_rook_cap_targets = valid_cap_targets & pinned_rook_rails;
  bb pinned_rook_quiet_targets = valid_quiet_targets & pinned_rook_rails;

  bb not_pinned = ~((pinned_bishop_rails | pinned_rook_rails) & my_pieces);

  // capture moves
  if (isWhitesMove()) {
    if (valid_cap_targets & en_passant_pawn) {
      genPawnCapsN(my_pawns & not_pinned, valid_cap_targets | en_passant_target, moves);
      genPawnCapsN(my_pawns & pinned_bishop_rails, pinned_bishop_cap_targets | en_passant_target, moves);
    }
    else {
      genPawnCapsN(my_pawns & not_pinned, valid_cap_targets, moves);
      genPawnCapsN(my_pawns & pinned_bishop_rails, pinned_bishop_cap_targets, moves);
    }
  }
  else {
    if (valid_cap_targets & en_passant_pawn) {
      genPawnCapsS(my_pawns & not_pinned, valid_cap_targets | en_passant_target, moves);
      genPawnCapsS(my_pawns & pinned_bishop_rails, pinned_bishop_cap_targets | en_passant_target, moves);
    }
    else {
      genPawnCapsS(my_pawns & not_pinned, valid_cap_targets, moves);
      genPawnCapsS(my_pawns & pinned_bishop_rails, pinned_bishop_cap_targets, moves);
    }
  }
  genKnightCaps(my_knights & not_pinned, valid_cap_targets, moves);
  genBishopCaps(my_bishops & not_pinned, empty_squares, valid_cap_targets, moves);
  genBishopCaps(my_bishops & pinned_bishop_rails, pinned_bishop_rails, pinned_bishop_cap_targets, moves);
  genRookCaps(my_rooks & not_pinned, empty_squares, valid_cap_targets, moves);
  genRookCaps(my_rooks & pinned_rook_rails, pinned_rook_rails, pinned_rook_cap_targets, moves);
  genQueenCaps(my_queens & not_pinned, empty_squares, valid_cap_targets, moves);
  genQueenCaps(my_queens & pinned_bishop_rails, pinned_bishop_rails, pinned_bishop_cap_targets, moves);
  genQueenCaps(my_queens & pinned_rook_rails, pinned_rook_rails, pinned_rook_cap_targets, moves);

  genKnightMoves(my_knights & not_pinned, valid_quiet_targets, moves);
  genBishopMoves(my_bishops & not_pinned, valid_quiet_targets, moves);
  genBishopMoves(my_bishops & pinned_bishop_rails, pinned_bishop_quiet_targets, moves);
  genRookMoves(my_rooks & not_pinned, valid_quiet_targets, moves);
  genRookMoves(my_rooks & pinned_rook_rails, pinned_rook_quiet_targets, moves);
  genQueenMoves(my_queens & not_pinned, valid_quiet_targets, moves);
  genQueenMoves(my_queens & pinned_bishop_rails, pinned_bishop_quiet_targets, moves);
  genQueenMoves(my_queens & pinned_rook_rails, pinned_rook_quiet_targets, moves);

  if (isWhitesMove()) {
    genPawnPushesN(my_pawns & not_pinned, valid_quiet_targets, enemy_pawns, moves);
    genPawnPushesN(my_pawns & pinned_rook_rails, pinned_rook_quiet_targets, enemy_pawns, moves);
    genKingMoves(my_king, empty_squares, ~under_threat, enemy_pieces
      , flags & w_castle_queenside, flags & w_castle_kingside, moves);
  }
  else {
    genPawnPushesS(my_pawns & not_pinned, valid_quiet_targets, enemy_pawns, moves);
    genPawnPushesS(my_pawns & pinned_bishop_rails, pinned_rook_quiet_targets, enemy_pawns, moves);
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

  Piece::Name on_dest = rmPiece(to);
  if (to == en_passant_square) {
    switch (piece) {
    case Piece::white_pawn: on_dest = rmPiece(to + south);
      break;
    case Piece::black_pawn: on_dest = rmPiece(to + north);
      break;
    default: break;
    }
  }
  en_passant_square = -1;

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
