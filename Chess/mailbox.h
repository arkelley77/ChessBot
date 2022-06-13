#ifndef MAILBOX_H
#define MAILBOX_H

// defines the mailbox representation of the board, which is
// a different way of saying this is a wrapper class around
// an array of 64 Piece::Names
//
// this kind of representation makes it easier to look up
// a piece by square but makes most other operations inefficient

#include <string>
#include <sstream>

#include "piece.h"
#include "myModules/binary/binary.h"
#include "bitboard.h" // for BitBoard::bb

class MailBox {
public:
  inline MailBox() noexcept { clear(); }
  inline MailBox(const Piece::Name board[]) noexcept;
  inline MailBox(const MailBox& to_copy) noexcept;

  constexpr inline void clear() noexcept;

  inline void setUp();
  void setUp(const char* fen);
  void setUp(const std::string& fen) { setUp(fen.c_str()); }

  // drops a piece on the board at position idx
  // replaces anything there (which is lost forever)
  inline void pushPiece(Piece::Name piece, int idx) noexcept { board[idx] = piece; }
  // peeks at the piece on the board at idx
  constexpr inline Piece::Name getPiece(int idx) const noexcept { return board[idx]; }
  // removes and returns the piece at idx before placing new_piece there
  inline Piece::Name replace(int idx, Piece::Name new_piece) noexcept {
    Piece::Name old_piece = getPiece(idx);
    pushPiece(new_piece, idx);
    return old_piece;
  }
  // gets the piece at the MS1b of square
  constexpr inline Piece::Name getPiece(const BitBoard::bb& square) const noexcept {
    return getPiece(Binary::indexOfMS1B(square));
  }

  std::string toString() const noexcept;

  friend std::ostream& operator<<(std::ostream& os, MailBox& board) {
    return os << board.toString();
  }

private:
  Piece::Name board[64];
};

#endif
