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
    MailBox(to_copy.board);
  }
  
  constexpr inline void clear() noexcept;
  
  constexpr inline void setUp() noexcept;
  void setUp(char* fen);
  void setUp(std::string fen);
  
  inline void pushPiece(Piece::Name piece, int idx) noexcept;
  constexpr inline Piece::Name getPiece(int idx) noexcept;
  // gets the piece at the MS1b of square
  constexpr inline void getPiece(BitBoard::bb square) noexcept { 
    return getPiece(Binary::getIndexOfMS1B(square));
  }
  
  std::string& toString();
  
  friend std::ostream& operator<<(std::ostream& os, MailBox& board) noexcept {
    return os << board.toString();
  }
  
private:
  Piece::Name board[64];
}

#endif
