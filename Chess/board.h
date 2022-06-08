#ifndef BOARD_H
#define BOARD_H

// defines a complete board representation comprised of
// both a BitBoard and a Mailbox

#include <string>
#include <sstream>

#include "bitboard.h"
#include "mailbox.h"

class Board {
public:
  constexpr inline Board() noexcept : bit(), box() {}
  constexpr inline Board(board& to_copy) noexcept 
    : bit(to_copy.bit), box(to_copy.box) {}
  
  constexpr inline void setUp() noexcept;
  void setUp(char* fen);
  void setUp(std::string& fen);
  
  std::string& toString() const noexcept;
  
  friend std::ostream& operator<<(std::ostream& os, Board& board) {
    return os << board.box;
  }
  
private:
  BitBoard bit;
  MailBox box;
}

#endif
