#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>

#include "../bitboards/bitboards.h"
#include "../indexing.h"
#include "move.h"

namespace Movegen {
  struct ChecksAndPins {
    Bitboards::bb checks;
    Bitboards::bb pins;
  };

  // generates quiet pawn moves (for white)
  // first one is the double-pushes, then single-pushes
  // ! adds at most 16 Moves to out_to
  void genPawnPushesN(Bitboards::bb pawns, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pawns, std::vector<Move>& out_to) noexcept;
  // generates quiet pawn moves (for black)
  // first one is the double-pushes, then single-pushes
  // ! adds at most 16 Moves to out_to
  void genPawnPushesS(Bitboards::bb pawns, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pawns, std::vector<Move>& out_to) noexcept;
  // generates pawn capture moves (for white)
  // ! adds at most 14 Moves to out_to
  void genPawnCapsN(Bitboards::bb from_pawns, Bitboards::bb targets
    , std::vector<Move>& out_to) noexcept;
  // generates pawn capture moves (for black)
  // ! adds at most 14 Moves to out_to
  void genPawnCapsS(Bitboards::bb from_pawns, Bitboards::bb targets
    , std::vector<Move>& out_to) noexcept;
  // generates quiet knight moves
  // ! normal game adds at most 16 Moves to out_to, but
  // ! worst-case endgame could add as many as 80
  void genKnightMoves(Bitboards::bb knights, Bitboards::bb valid_targets
    , std::vector<Move>& out_to) noexcept;
  // generates knight captures
  void genKnightCaps(Bitboards::bb knights, Bitboards::bb enemy_pieces
    , std::vector<Move>& out_to) noexcept;
  // generate quiet bishop moves
  // ! normal game adds at most 26 Moves to out_to, but
  // ! worst-case endgame could add as many as 130
  void genBishopMoves(Bitboards::bb bishops, Bitboards::bb empty_squares
    , std::vector<Move>& out_to) noexcept;
  // generate bishop captures
  void genBishopCaps(Bitboards::bb bishops, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pieces, std::vector<Move>& out_to) noexcept;
  // generate quiet rook moves
  // ! normal game adds at most 28 Moves to out_to, but
  // ! worst-case endgame could add as many as 140
  void genRookMoves(Bitboards::bb rooks, Bitboards::bb empty_squares
    , std::vector<Move>& out_to) noexcept;
  // generate rook captures
  void genRookCaps(Bitboards::bb rooks, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pieces, std::vector<Move>& out_to) noexcept;
  // generate quiet queen moves
  // ! normal game adds at most 27 Moves to out_to, but
  // ! worst-case endgame could add as many as 243
  inline void genQueenMoves(Bitboards::bb queens
    , Bitboards::bb empty_squares, std::vector<Move>& out_to) noexcept {
    genBishopMoves(queens, empty_squares, out_to);
    genRookMoves(queens, empty_squares, out_to);
  }
  // generate queen captures
  inline void genQueenCaps(Bitboards::bb queens
    , Bitboards::bb empty_squares, Bitboards::bb enemy_pieces
    , std::vector<Move>& out_to) noexcept {
    genBishopCaps(queens, empty_squares, enemy_pieces, out_to);
    genRookCaps(queens, empty_squares, enemy_pieces, out_to);
  }
  // generate all king moves (quiet, capture, and castle)
  // ! adds between 8 and 10 Moves to out_to, depending on castling rights
  void genKingMoves(Bitboards::bb king, Bitboards::bb empty_squares
    , Bitboards::bb not_threatened, Bitboards::bb enemy_pieces, bool castle_queenside
    , bool castle_kingside, std::vector<Move>& out_to) noexcept;

  // the following functions generate bitboards that help with check, pins, etc

  Bitboards::bb findPinnedTo(Bitboards::bb king, Bitboards::bb empty_squares
    , Bitboards::bb my_pieces, Bitboards::bb enemy_bishoplike
    , Bitboards::bb enemy_rooklike) noexcept;

  inline Bitboards::bb findPinnedRailNW(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_bishoplike) noexcept {
    using namespace Bitboards;
    return shiftNW((obstructedFillNW(king, ~enemy_pieces)))
      & obstructedFillSE(enemy_bishoplike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailNE(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_bishoplike) noexcept {
    using namespace Bitboards;
    return shiftNE((obstructedFillNE(king, ~enemy_pieces)))
      & obstructedFillSW(enemy_bishoplike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailSW(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_bishoplike) noexcept {
    using namespace Bitboards;
    return shiftSW((obstructedFillSW(king, ~enemy_pieces)))
      & obstructedFillNE(enemy_bishoplike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailSE(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_bishoplike) noexcept {
    using namespace Bitboards;
    return shiftSE((obstructedFillSE(king, ~enemy_pieces)))
      & obstructedFillNW(enemy_bishoplike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailN(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_rooklike) noexcept {
    using namespace Bitboards;
    return shiftN((obstructedFillN(king, ~enemy_pieces)))
      & obstructedFillS(enemy_rooklike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailS(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_rooklike) noexcept {
    using namespace Bitboards;
    return shiftS((obstructedFillS(king, ~enemy_pieces)))
      & obstructedFillN(enemy_rooklike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailE(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_rooklike) noexcept {
    using namespace Bitboards;
    return shiftE((obstructedFillE(king, ~enemy_pieces)))
      & obstructedFillW(enemy_rooklike, ~enemy_pieces);
  }
  inline Bitboards::bb findPinnedRailW(Bitboards::bb king, Bitboards::bb enemy_pieces, Bitboards::bb enemy_rooklike) noexcept {
    using namespace Bitboards;
    return shiftW((obstructedFillW(king, ~enemy_pieces)))
      & obstructedFillE(enemy_rooklike, ~enemy_pieces );
  }

  Bitboards::bb findCheckingPieces(Bitboards::bb king, Bitboards::bb empty_squares
    , Bitboards::bb enemy_knights, Bitboards::bb enemy_bishoplike
    , Bitboards::bb enemy_rooklike) noexcept;

  // generates all valid squares to move to which will remove check (for white)
  // if not in check, returns a full board
  // if in double check, returns an empty board
  // ! does not work for kings
  Bitboards::bb legalMoveTargetsWhite(Bitboards::bb king, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pawns, Bitboards::bb enemy_knights
    , Bitboards::bb enemy_bishoplike, Bitboards::bb enemy_rooklike) noexcept;
  // generates all valid squares to move to which will remove check (for black)
  // if not in check, returns a full board
  // if in double check, returns an empty board
  // ! does not work for kings
  Bitboards::bb legalMoveTargetsBlack(Bitboards::bb king, Bitboards::bb empty_squares
    , Bitboards::bb enemy_pawns, Bitboards::bb enemy_knights
    , Bitboards::bb enemy_bishoplike, Bitboards::bb enemy_rooklike) noexcept;

  // gen<X>Threats() functions are inlined because they are needed to determine
  // if the king is in check, which in turn determines how to generate moves

  inline Bitboards::bb genPawnThreatsN(Bitboards::bb from_pawns) noexcept {
    using namespace Bitboards;
    return shiftN(shiftW(from_pawns) | shiftE(from_pawns));
  }
  inline Bitboards::bb genPawnThreatsS(Bitboards::bb from_pawns) noexcept {
    using namespace Bitboards;
    return shiftS(shiftW(from_pawns) | shiftE(from_pawns));
  }
  inline Bitboards::bb genKnightThreats(Bitboards::bb from_knights) noexcept {
    using namespace Bitboards;
    // bitboards are already very efficient, but we can cut down on the number
    // of operations even more by limiting the number of times we shift north and south
    // as these shifts require a binary AND with a mask
    bb we_1 = shiftW(from_knights) | shiftE(from_knights);
    bb we_2 = shift2W(from_knights) | shift2E(from_knights);
    return shift2N(we_1) | shiftN(we_2) | shiftS(we_2) | shift2S(we_1);
  }
  inline Bitboards::bb genBishopThreats(Bitboards::bb from_bishops
    , Bitboards::bb empty_squares) noexcept {
    using namespace Bitboards;
    return shiftNW(obstructedFillNW(from_bishops, empty_squares))
      | shiftNE(obstructedFillNE(from_bishops, empty_squares))
      | shiftSW(obstructedFillSW(from_bishops, empty_squares))
      | shiftSE(obstructedFillSE(from_bishops, empty_squares));
  }
  inline Bitboards::bb genRookThreats(Bitboards::bb from_rooks
    , Bitboards::bb empty_squares) noexcept {
    using namespace Bitboards;
    return shiftN(obstructedFillN(from_rooks, empty_squares))
      | shiftE(obstructedFillE(from_rooks, empty_squares))
      | shiftS(obstructedFillS(from_rooks, empty_squares))
      | shiftW(obstructedFillW(from_rooks, empty_squares));
  }
  inline Bitboards::bb genQueenThreats(Bitboards::bb from_queens
    , Bitboards::bb empty_squares) noexcept {
    using namespace Bitboards;
    return genBishopThreats(from_queens, empty_squares)
      | genRookThreats(from_queens, empty_squares);
  }
  inline Bitboards::bb genKingThreats(Bitboards::bb from_king) noexcept {
    using namespace Bitboards;
    bb threats = from_king | shiftW(from_king) | shiftE(from_king);
    threats |= shiftN(threats);
    threats |= shiftS(threats);
    threats &= ~from_king;
    return threats;
  }
  inline Bitboards::bb genAllThreatsWhite(Bitboards::bb from_pawns
    , Bitboards::bb from_knights, Bitboards::bb from_bishoplike
    , Bitboards::bb from_rooklike, Bitboards::bb from_king
    , Bitboards::bb empty_squares) noexcept {
    return genPawnThreatsN(from_pawns)
      | genKnightThreats(from_knights)
      | genBishopThreats(from_bishoplike, empty_squares)
      | genRookThreats(from_rooklike, empty_squares)
      | genKingThreats(from_king);
  }
  inline Bitboards::bb genAllThreatsBlack(Bitboards::bb from_pawns
    , Bitboards::bb from_knights, Bitboards::bb from_bishoplike
    , Bitboards::bb from_rooklike, Bitboards::bb from_king
    , Bitboards::bb empty_squares) noexcept {
    return genPawnThreatsS(from_pawns)
      | genKnightThreats(from_knights)
      | genBishopThreats(from_bishoplike, empty_squares)
      | genRookThreats(from_rooklike, empty_squares)
      | genKingThreats(from_king);
  }
}

#endif // MOVEGEN_H
