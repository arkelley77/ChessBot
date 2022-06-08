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
#include "binary.h"
#include "bitboard.h" // for BitBoard::bb

class MailBox {
public:
  constexpr inline MailBox() noexcept { clear(); }
  constexpr MailBox(const Piece::Name[] board) noexcept;
  constexpr inline MailBox(const MailBox& to_copy) noexcept {
    this->MailBox(to_copy.board);
  }
  
  constexpr inline void clear() noexcept;
  
  inline void setUp();
  void setUp(const char* fen);
  void setUp(const std::string& fen) { setUp(fen.c_str()); }
  
  inline void pushPiece(Piece::Name piece, int idx) noexcept { board[idx] = piece; }
  constexpr inline Piece::Name getPiece(int idx) noexcept { return board[idx]; }
  // gets the piece at the MS1b of square
  constexpr inline void getPiece(BitBoard::bb square) noexcept { 
    return getPiece(Binary::getIndexOfMS1B(square));
  }
  
  std::string toString();
  
  friend std::ostream& operator<<(std::ostream& os, MailBox& board) noexcept {
    return os << board.toString();
  }
  
private:
  Piece::Name board[64];
}

#endif
