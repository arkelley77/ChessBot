#include "movegen.h"

using namespace Binary;
using namespace Bitboards;

using std::vector;

void Movegen::genPawnPushesN(bb pawns, bb empty_squares, bb enemy_pawns, vector<Move>& out_to) noexcept {
  using namespace Piece;
  if (!pawns) return;
  bb singles = shiftN(pawns) & empty_squares; // single-square pawn moves
  bb doubles = shiftN(singles) & r4 & empty_squares;
  bb en_passant_avail = (shiftW(enemy_pawns) | shiftE(enemy_pawns)) & doubles;
  bb dest_square;
  for (; singles; singles &= ~dest_square) {
    int to = indexOfMS1B(singles);
    dest_square = idxToBoard(to);
    int from = to + Indexing::south;
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
  for (; doubles; doubles &= ~dest_square) {
    int to = indexOfMS1B(doubles);
    dest_square = idxToBoard(to);
    int from = to + 2 * Indexing::south;
    if (dest_square & en_passant_avail) {
      // pawn could be captured en passant
      out_to.push_back(Move(from, to, Move::en_passant));
    }
    else out_to.push_back(Move(from, to));
  }
}
void Movegen::genPawnPushesS(bb pawns, bb empty_squares, bb enemy_pawns, vector<Move>& out_to) noexcept {
  using namespace Piece;
  if (!pawns) return;
  bb singles = shiftS(pawns) & empty_squares; // single-square pawn moves
  bb doubles = shiftS(singles) & r5 & empty_squares;
  bb en_passant_avail = (shiftW(enemy_pawns) | shiftE(enemy_pawns)) & doubles;
  bb dest_square;
  for (; singles; singles &= ~dest_square) {
    int to = indexOfMS1B(singles);
    dest_square = idxToBoard(to);
    int from = to + Indexing::north;
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
  for (; doubles; doubles &= ~dest_square) {
    int to = indexOfMS1B(doubles);
    dest_square = idxToBoard(to);
    int from = to + 2 * Indexing::north;
    if (dest_square & en_passant_avail) {
      // pawn could be captured en passant
      out_to.push_back(Move(from, to, Move::en_passant));
    }
    else out_to.push_back(Move(from, to));
  }
}

void Movegen::genPawnCapsN(bb from_pawns, bb targets, vector<Move>& out_to) noexcept {
  using Indexing::n_west, Indexing::n_east, Indexing::s_west, Indexing::s_east;
  using namespace Piece;
  
  // since a pawn will never be on r1 or r8 (always promoted),
  // the north/south shift will never overflow, so no masking is needed
  // therefore, we use the shift() function from Binary to avoid it
  bb caps = shiftNW(from_pawns) & targets;
  for (bb dest_square; caps; caps &= ~dest_square) {
    int to = indexOfMS1B(caps);
    int from = to + s_east;
    dest_square = idxToBoard(to);
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
  caps = shiftNE(from_pawns) & targets;
  for (bb dest_square; caps; caps &= ~dest_square) {
    int to = indexOfMS1B(caps);
    int from = to + s_west;
    dest_square = idxToBoard(to);
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
}
void Movegen::genPawnCapsS(bb from_pawns, bb targets, vector<Move>& out_to) noexcept {
  using Indexing::n_west, Indexing::n_east, Indexing::s_west, Indexing::s_east;
  using namespace Piece;

  // since a pawn will never be on r1 or r8 (always promoted),
  // the north/south shift will never overflow, so no masking is needed
  // therefore, we use the shift() function from Binary to avoid it
  bb caps = shiftSW(from_pawns) & targets;
  for (bb dest_square; caps; caps &= ~dest_square) {
    int to = indexOfMS1B(caps);
    int from = to + s_east;
    dest_square = idxToBoard(to);
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
  caps = shiftSE(from_pawns) & targets;
  for (bb dest_square; caps; caps &= ~dest_square) {
    int to = indexOfMS1B(caps);
    int from = to + s_west;
    dest_square = idxToBoard(to);
    if (dest_square & r8) {
      // potential pawn promotion
      out_to.push_back(Move(from, to, Move::queen, Move::promo));
      out_to.push_back(Move(from, to, Move::rook, Move::promo));
      out_to.push_back(Move(from, to, Move::bishop, Move::promo));
      out_to.push_back(Move(from, to, Move::knight, Move::promo));
    }
    else out_to.push_back(Move(from, to));
  }
}

void Movegen::genKnightMoves(bb knights, bb empty_squares, vector<Move>& out_to) noexcept {
  for (bb from_square; knights; knights &= ~from_square) {
    int from = indexOfMS1B(knights), to;
    from_square = idxToBoard(from);
    
    for (bb slides = genKnightThreats(from_square) & empty_squares
      ; slides; slides &= ~idxToBoard(to)) {
      to = indexOfMS1B(slides);
      out_to.push_back(Move(from, to));
      bool spec = out_to.back().getSpecial();
      continue;
    }
  }
}
void Movegen::genKnightCaps(bb knights, bb enemy_pieces, vector<Move>& out_to) noexcept {
  for (bb from_square; knights; knights &= ~from_square) {
    int from = indexOfMS1B(knights), to;
    from_square = idxToBoard(from);
    
    for (bb caps = genKnightThreats(from_square) & enemy_pieces
      ; caps; caps &= ~idxToBoard(to)) {
      to = indexOfMS1B(caps);
      out_to.push_back(Move(from, to));
      bool spec = out_to.back().getSpecial();
      continue;
    }
  }
}

void Movegen::genBishopMoves(bb bishops, bb empty_squares, vector<Move>& out_to) noexcept {
  for (bb from_square; bishops; bishops &= ~from_square) {
    int from = indexOfMS1B(bishops), to;
    from_square = idxToBoard(from);
    
    for (bb slides = genBishopThreats(from_square, empty_squares) & empty_squares
      ; slides; slides &= ~idxToBoard(to)) {
      to = indexOfMS1B(slides);
      out_to.push_back(Move(from, to));
    }
  }
}
void Movegen::genBishopCaps(bb bishops, bb empty_squares, bb enemy_pieces, vector<Move>& out_to) noexcept {
  for (bb from_square; bishops; bishops &= ~from_square) {
    int from = indexOfMS1B(bishops), to;
    from_square = idxToBoard(from);
    
    for (bb caps = genBishopThreats(from_square, empty_squares) & enemy_pieces
      ; caps; caps &= ~idxToBoard(to)) {
      to = indexOfMS1B(caps);
      out_to.push_back(Move(from, to));
    }
  }
}

void Movegen::genRookMoves(bb rooks, bb empty_squares, vector<Move>& out_to) noexcept {
  for (bb from_square; rooks; rooks &= ~from_square) {
    int from = indexOfMS1B(rooks), to;
    from_square = idxToBoard(from);
    
    for (bb slides = genRookThreats(from_square, empty_squares) & empty_squares
      ; slides; slides &= ~idxToBoard(to)) {
      to = indexOfMS1B(slides);
      out_to.push_back(Move(from, to));
    }
  }
}
void Movegen::genRookCaps(bb rooks, bb empty_squares, bb enemy_pieces, vector<Move>& out_to) noexcept {
  for (bb from_square; rooks; rooks &= ~from_square) {
    int from = indexOfMS1B(rooks), to;
    from_square = idxToBoard(from);

    for (bb caps = genRookThreats(from_square, empty_squares) & enemy_pieces
      ; caps; caps &= ~idxToBoard(to)) {
      to = indexOfMS1B(caps);
      out_to.push_back(Move(from, to));
    }
  }
}

void Movegen::genKingMoves(Bitboards::bb king, Bitboards::bb empty_squares
  , Bitboards::bb not_threatened, Bitboards::bb enemy_pieces, bool castle_queenside
  , bool castle_kingside, vector<Move>& out_to) noexcept {
  int from = indexOfMS1B(king), to;
  bb threats = genKingThreats(king);
  bb moves_board = threats & not_threatened;
  bb empty_not_threatened = empty_squares & not_threatened;
  king &= not_threatened;
  bb castle_board = empty_not_threatened & (
    ((castle_queenside) ? shiftW(shiftW(king) & empty_not_threatened) : 0)
    | ((castle_kingside) ? shiftE(shiftE(king) & empty_not_threatened) : 0));
  
  for (bb caps = moves_board & enemy_pieces
    ; caps; caps &= ~(idxToBoard(to))) {
    to = indexOfMS1B(moves_board);
    out_to.push_back(Move(from, to));
  }
  
  for (bb slides = moves_board & ~enemy_pieces
    ; slides; slides &= ~(idxToBoard(to))) {
    to = indexOfMS1B(slides);
    out_to.push_back(Move(from, to));
  }

  for (; castle_board; castle_board &= ~idxToBoard(to)) {
    to = indexOfMS1B(castle_board);
    out_to.push_back(Move(from, to, Move::castling));
  }
}

bb Movegen::findPinnedTo(bb king, bb empty_squares, bb my_pieces
  , bb enemy_bishoplike, bb enemy_rooklike) noexcept {
  bb not_king = ~king;
  bb not_bishoplike = ~enemy_bishoplike;
  bb not_rooklike = ~enemy_rooklike;
  return my_pieces & (
    shiftNW((obstructedFillNW(king, not_bishoplike)) & obstructedFillSE(enemy_bishoplike, not_king))
    | (obstructedFillNE(king, not_bishoplike) & obstructedFillSW(enemy_bishoplike, not_king))
    | (obstructedFillSW(king, not_bishoplike) & obstructedFillNE(enemy_bishoplike, not_king))
    | (obstructedFillSE(king, not_bishoplike) & obstructedFillNW(enemy_bishoplike, not_king))
    | (obstructedFillN(king, not_rooklike) & obstructedFillS(enemy_rooklike, not_king))
    | (obstructedFillS(king, not_rooklike) & obstructedFillN(enemy_rooklike, not_king))
    | (obstructedFillE(king, not_rooklike) & obstructedFillW(enemy_rooklike, not_king))
    | (obstructedFillW(king, not_rooklike) & obstructedFillE(enemy_rooklike, not_king))
  );
}

bb Movegen::findCheckingPieces(bb king, bb empty_squares, bb enemy_knights
  , bb enemy_bishoplike, bb enemy_rooklike) noexcept {
  return (
    genKnightThreats(king) & enemy_knights
    | (genBishopThreats(king, empty_squares) & enemy_bishoplike)
    | (genRookThreats(king, empty_squares) & enemy_rooklike)
  );
}

bb Movegen::legalMoveTargetsWhite(bb king, bb empty_squares, bb enemy_pawns
  , bb enemy_knights, bb enemy_bishoplike, bb enemy_rooklike) noexcept {
  bb nw = shiftNW(obstructedFillNW(king, empty_squares));
  bb ne = shiftNE(obstructedFillNE(king, empty_squares));
  bb sw = shiftSW(obstructedFillSW(king, empty_squares));
  bb se = shiftSE(obstructedFillSE(king, empty_squares));
  bb n = shiftN(obstructedFillN(king, empty_squares));
  bb s = shiftS(obstructedFillS(king, empty_squares));
  bb e = shiftE(obstructedFillE(king, empty_squares));
  bb w = shiftW(obstructedFillW(king, empty_squares));
  bb knight = genKnightThreats(king);
  bb pawn = genPawnThreatsN(enemy_pawns);
  bb checking_pieces = ((nw | ne | sw | se) & enemy_bishoplike)
    | ((n | s | e | w) & enemy_rooklike)
    | (knight & enemy_knights) | (pawn & enemy_pawns);
  switch (countSetBits(checking_pieces)) {
  case 1:
    if (nw & enemy_bishoplike) return nw;
    if (ne & enemy_bishoplike) return ne;
    if (sw & enemy_bishoplike) return sw;
    if (se & enemy_bishoplike) return se;
    if (n & enemy_rooklike) return n;
    if (s & enemy_rooklike) return s;
    if (e & enemy_rooklike) return e;
    if (w & enemy_rooklike) return w;
    if (knight & enemy_knights) return knight & enemy_knights;
    if (pawn & enemy_pawns) return pawn & enemy_pawns;
    [[fallthrough]];
  case 0: return -1;
  default: return 0;
  }
}

bb Movegen::legalMoveTargetsBlack(bb king, bb empty_squares, bb enemy_pawns
  , bb enemy_knights, bb enemy_bishoplike, bb enemy_rooklike) noexcept {
  bb nw = shiftNW(obstructedFillNW(king, empty_squares));
  bb ne = shiftNE(obstructedFillNE(king, empty_squares));
  bb sw = shiftSW(obstructedFillSW(king, empty_squares));
  bb se = shiftSE(obstructedFillSE(king, empty_squares));
  bb n = shiftN(obstructedFillN(king, empty_squares));
  bb s = shiftS(obstructedFillS(king, empty_squares));
  bb e = shiftE(obstructedFillE(king, empty_squares));
  bb w = shiftW(obstructedFillW(king, empty_squares));
  bb knight = genKnightThreats(king);
  bb pawn = shiftS(shiftW(king) | shiftE(king));
  bb checking_pieces = ((nw | ne | sw | se) & enemy_bishoplike)
    | ((n | s | e | w) & enemy_rooklike)
    | (knight & enemy_knights) | (pawn & enemy_pawns);
  switch (countSetBits(checking_pieces)) {
  case 1:
    if (nw & enemy_bishoplike) return nw;
    if (ne & enemy_bishoplike) return ne;
    if (sw & enemy_bishoplike) return sw;
    if (se & enemy_bishoplike) return se;
    if (n & enemy_rooklike) return n;
    if (s & enemy_rooklike) return s;
    if (e & enemy_rooklike) return e;
    if (w & enemy_rooklike) return w;
    if (knight & enemy_knights) return knight & enemy_knights;
    if (pawn & enemy_pawns) return pawn & enemy_pawns;
    [[fallthrough]];
  case 0: return -1;
  default: return 0;
  }
}
