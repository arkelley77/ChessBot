#include "movegen.h"

#include <iostream>

using namespace Bitboards;
using namespace Movegen;
using std::vector, std::exception, std::cout, std::endl;

int main() {
  vector<Move> moves;
  cout << "Testing genPawnPushes...\n- Pawn on start row...";
  genPawnPushesN(c2, ~c2, 0, moves);
  if (moves.size() < 2)
    cout << "[FAIL] Not enough moves -> expected 2, got " << moves.size() << endl;
  else if (moves.size() > 2)
    cout << "[FAIL] Too many moves -> expected 2, got " << moves.size() << endl;
  else {
    if (idxToBoard(moves[0].getFromSquare()) != c2 || idxToBoard(moves[1].getFromSquare()) != c2)
      cout << "[FAIL] Move originates from wrong square" << endl;
    else if (idxToBoard(moves[0].getToSquare()) == idxToBoard(moves[1].getToSquare()))
      cout << "[FAIL] Moves target same square" << endl;
    else if (moves[0].getSpecial() || moves[1].getSpecial())
      cout << "[FAIL] Moves should not be special" << endl;
    else if (!((idxToBoard(moves[0].getToSquare()) == c3 && idxToBoard(moves[1].getToSquare()) == c4)
      || (idxToBoard(moves[0].getToSquare()) == c4 && idxToBoard(moves[1].getToSquare()) == c3)))
      cout << "[FAIL] Generated incorrect moves" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "- Pawn not on start row...";
  genPawnPushesS(c3, ~c3, 0, moves);
  if (moves.size() < 1)
    cout << "[FAIL] Not enough moves -> expected 1, got " << moves.size() << endl;
  else if (moves.size() > 1)
    cout << "[FAIL] Too many moves -> expected 1, got " << moves.size() << endl;
  else {
    if (idxToBoard(moves[0].getFromSquare()) != c3)
      cout << "[FAIL] Move originates from wrong square" << endl;
    else if (moves[0].getSpecial())
      cout << "[FAIL] Move should not be special" << endl;
    else if (idxToBoard(moves[0].getToSquare()) != c2)
      cout << "[FAIL] Generated incorrect move" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "- Testing en passant availability...";
  genPawnPushesN(c2, ~c2 & ~d4, d4, moves);
  if (!((moves[0].getSpecial() & Move::en_passant)
    || (moves[1].getSpecial() & Move::en_passant)))
    cout << "[FAIL] Move should allow en passant" << endl;
  else cout << "[PASS]" << endl;
  moves.clear();
  cout << "- Testing promotion...";
  genPawnPushesS(c2, ~c2, 0, moves);
  if (moves.size() < 4)
    cout << "[FAIL] Not enough moves -> expected 4, got " << moves.size() << endl;
  else if (moves.size() > 4)
    cout << "[FAIL] Too many moves -> expected 4, got " << moves.size() << endl;
  else {
    bool passing = true;
    bool found[4] = { false, false, false, false };  // n, b, r, q
    for (Move& move : moves) {
      if (idxToBoard(moves[0].getFromSquare()) != c2) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        passing = false;
        break;
      }
      else if (move.getSpecial() != Move::promo) {
        cout << "[FAIL] Move should be Move::promo" << endl;
        passing = false;
        break;
      }
      else if (idxToBoard(moves[0].getToSquare()) != c1) {
        cout << "[FAIL] Generated incorrect move" << endl;
        passing = false;
        break;
      }
      else if (move.getPromoType() == Piece::knight) found[0] = true;
      else if (move.getPromoType() == Piece::bishop) found[1] = true;
      else if (move.getPromoType() == Piece::rook) found[2] = true;
      else if (move.getPromoType() == Piece::queen) found[3] = true;
    }
    if (passing) {
      if (!(found[0] && found[1] && found[2] && found[3]))
        cout << "[FAIL] Incorrect piece promotion types" << endl;
      else cout << "[PASS]" << endl;
    }
  }
  moves.clear();
  cout << "Testing genPawnCaps...";
  genPawnCapsN(c2, d3, moves);
  if (moves.size() < 1)
    cout << "[FAIL] Not enough moves -> expected 1, got " << moves.size() << endl;
  else if (moves.size() > 1)
    cout << "[FAIL] Too many moves -> expected 1, got " << moves.size() << endl;
  else {
    if (idxToBoard(moves[0].getFromSquare()) != c2)
      cout << "[FAIL] Move originates from wrong square" << endl;
    else if (moves[0].getSpecial())
      cout << "[FAIL] Moves should not be special" << endl;
    else if (idxToBoard(moves[0].getToSquare()) != d3)
      cout << "[FAIL] Generated incorrect move" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "Testing genKnightMoves...";
  genKnightMoves(d4, ~d4, moves);
  if (moves.size() < 8)
    cout << "[FAIL] Not enough moves -> expected 8, got " << moves.size() << endl;
  else if (moves.size() > 8)
    cout << "[FAIL] Too many moves -> expected 8, got " << moves.size() << endl;
  else {
    bool failed = false;
    for (Move& move : moves) {
      if (idxToBoard(move.getFromSquare()) != d4) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        failed = true;
      }
      else if (move.getSpecial()) {
        cout << "[FAIL] Moves should not be special" << endl;
        failed = true;
      }
      else {
        using namespace Indexing;
        bool is_valid = false;
        for (auto dir : { n_n_west, n_n_east, w_n_west, e_n_east,
          w_s_west, e_s_east, s_s_west, s_s_east }) {
          if (move.getToSquare() - move.getFromSquare() == dir) is_valid = true;
        }
        if (!is_valid) {
          cout << "[FAIL] Generated incorrect move" << endl;
          failed = true;
        }
      }
    }
    if (!failed) cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "Testing genBishopMoves...";
  genBishopMoves(d4, ~d4 & ~f2, moves);
  if (moves.size() < 11)
    cout << "[FAIL] Not enough moves -> expected 11, got " << moves.size() << endl;
  else if (moves.size() > 11)
    cout << "[FAIL] Too many moves -> expected 11, got " << moves.size() << endl;
  else {
    bool failed = false;
    for (Move& move : moves) {
      if (idxToBoard(move.getFromSquare()) != d4) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        failed = true;
      }
      else if (move.getSpecial()) {
        cout << "[FAIL] Moves should not be special" << endl;
        failed = true;
      }
      else {
        bool is_valid = false;
        for (auto target : { c5, b6, a7, e5, f6, g7, h8, c3, b2, a1, e3 }) {
          if (idxToBoard(move.getToSquare()) == target) is_valid = true;
        }
        if (!is_valid) {
          cout << "[FAIL] Generated incorrect move";
          failed = true;
        }
      }
    }
    if (!failed) cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "Testing genRookMoves...";
  genRookMoves(d4, ~d4 & ~d2, moves);
  if (moves.size() < 12)
    cout << "[FAIL] Not enough moves -> expected 12, got " << moves.size() << endl;
  else if (moves.size() > 12)
    cout << "[FAIL] Too many moves -> expected 12, got " << moves.size() << endl;
  else {
    bool failed = false;
    for (Move& move : moves) {
      if (idxToBoard(move.getFromSquare()) != d4) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        failed = true;
      }
      else if (move.getSpecial()) {
        cout << "[FAIL] Move should not be special" << endl;
        failed = true;
      }
      else {
        using namespace Indexing;
        bool is_valid = false;
        for (auto target : { d1, d2, d3, d5, d6, d7, d8, a4, b4, c4, e4, f4, g4, h4 }) {
          if (idxToBoard(move.getToSquare()) == target) is_valid = true;
        }
        if (!is_valid) {
          cout << "[FAIL] Generated incorrect move" << endl;
          failed = true;
        }
      }
    }
    if (!failed) cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "Testing genQueenMoves...";
  genQueenMoves(d4, ~d4 & ~d2 & ~f2, moves);
  if (moves.size() < 23)
    cout << "[FAIL] Not enough moves -> expected 12, got " << moves.size() << endl;
  else if (moves.size() > 23)
    cout << "[FAIL] Too many moves -> expected 12, got " << moves.size() << endl;
  else {
    bool failed = false;
    for (Move& move : moves) {
      if (idxToBoard(move.getFromSquare()) != d4) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        failed = true;
      }
      else if (move.getSpecial()) {
        cout << "[FAIL] Move should not be special" << endl;
        failed = true;
      }
      else {
        using namespace Indexing;
        bool is_valid = false;
        for (auto target : { c5, b6, a7, e5, f6, g7, h8, c3, b2, a1, e3, d1, d2, d3, d5, d6, d7, d8, a4, b4, c4, e4, f4, g4, h4 }) {
          if (idxToBoard(move.getToSquare()) == target) is_valid = true;
        }
        if (!is_valid) {
          cout << "[FAIL] Generated incorrect move" << endl;
          failed = true;
        }
      }
    }
    if (!failed) cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "Testing genKingMoves...\n- Normal king moves...";
  genKingMoves(d4, ~d4, ~0ULL, 0, false, false, moves);
  if (moves.size() < 8)
    cout << "[FAIL] Not enough moves -> Expected 8, got " << moves.size() << endl;
  else if (moves.size() > 8)
    cout << "[FAIL] Too many moves -> Expected 8, got " << moves.size() << endl;
  else {
    bool failed = false;
    for (Move& move : moves) {
      if (idxToBoard(move.getFromSquare()) != d4) {
        cout << "[FAIL] Move originates from wrong square" << endl;
        failed = true;
      }
      else if (move.getSpecial()) {
        cout << "[FAIL] Moves should not be special" << endl;
        failed = true;
      }
      else {
        using namespace Indexing;
        bool is_valid = false;
        for (auto dir : { north, south, east, west,
          n_west, n_east, s_west, s_east }) {
          if (move.getToSquare() - move.getFromSquare() == dir) is_valid = true;
        }
        if (!is_valid) {
          cout << "[FAIL] Generated incorrect move" << endl;
          failed = true;
        }
      }
    }
    if (!failed) cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "- Testing queenside castle...";
  genKingMoves(e1, ~0ULL, ~0ULL, 0, true, false, moves);
  {
    bool castle_move_found = false;
    bool castle_move_correct = false;
    for (Move& move : moves) {
      if (move.getSpecial() & Move::castling) {
        castle_move_found = true;
        if (idxToBoard(move.getToSquare()) == c1)
          castle_move_correct = true;
        break;
      }
    }
    if (!castle_move_found) cout << "[FAIL] No castle move generated" << endl;
    else if (!castle_move_correct) cout << "[FAIL] Generated incorrect castle move" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "- Testing kingside castle...";
  genKingMoves(e1, ~0ULL, ~0ULL, 0, false, true, moves);
  {
    bool castle_move_found = false;
    bool castle_move_correct = false;
    for (Move& move : moves) {
      if (move.getSpecial() & Move::castling) {
        castle_move_found = true;
        if (idxToBoard(move.getToSquare()) == g1)
          castle_move_correct = true;
        break;
      }
    }
    if (!castle_move_found) cout << "[FAIL] No castle move generated" << endl;
    else if (!castle_move_correct) cout << "[FAIL] Generated incorrect castle move" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();
  cout << "- Testing castle through check...";
  genKingMoves(e1, ~e1, ~f1, 0, false, true, moves);
  {
    bool castle_move_found = false;
    for (Move& move : moves) {
      if (move.getSpecial() & Move::castling) {
        castle_move_found = true;
        break;
      }
    }
    if (castle_move_found) cout << "[FAIL] Castle move generated" << endl;
    else cout << "[PASS]" << endl;
  }
  moves.clear();

  cout << endl << "Testing findPinnedTo...\n- With 1 bishop and 1 rook pin...";
  if (findPinnedTo(e1, ~e1 & ~f1 & ~f2 & ~g1 & ~g3, f1 | f2, g3, g1) != (f1 | f2)) cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
  cout << "- With swapped rooks and bishops...";
  bb a = findPinnedTo(e1, ~e1 & ~f1 & ~f2 & ~g1 & ~g3, f1 | f2, g1, g3);
  if (a) cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;

  cout << "Testing findCheckingPieces...";
  if (findCheckingPieces(e1, ~e1 & ~e4 & ~f3 & ~g3, f3, g3, e4) != (e4 | f3 | g3))
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;

  cout << "Testing blockCheckTargets...\n- With 1 rook check...";
  if (legalMoveTargetsWhite(e1, ~e1 & ~e5, 0, 0, 0, e5) != (e2 | e3 | e4 | e5))
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
  cout << "- With 1 bishop check...";
  if (legalMoveTargetsWhite(e1, ~e1 & ~h4, 0, 0, h4, 0) != (f2 | g3 | h4))
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
  cout << "- With 1 knight check...";
  if (legalMoveTargetsWhite(e1, ~e1 & ~f3, 0, f3, 0, 0) != f3)
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
  cout << "- With no checks...";
  if (legalMoveTargetsWhite(e1, ~e1, 0, 0, 0, 0) != ~0ULL)
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
  cout << "- With double check...";
  if (legalMoveTargetsWhite(e1, ~e1 & ~e5 & ~h4, 0, 0, h4, e5) != 0)
    cout << "[FAIL]" << endl;
  else cout << "[PASS]" << endl;
}
