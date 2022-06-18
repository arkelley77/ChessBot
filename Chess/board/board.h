#ifndef BOARD_H
#define BOARD_H

// defines a complete board representation comprised of
// both a BitBoard and a Mailbox

#include <string>
#include <sstream>

#include "bitboard/bitboard.h"
#include "mailbox/mailbox.h"

class Board {
public:
  inline Board() noexcept : bit(), box(), flags(0)
    , en_passant_square(-1), halfmove_clock(0), fullmove_number(0) {}
  inline Board(Board& to_copy) noexcept : bit(to_copy.bit), box(to_copy.box)
    , flags(to_copy.flags), en_passant_square(to_copy.en_passant_square)
    , halfmove_clock(to_copy.halfmove_clock), fullmove_number(to_copy.fullmove_number) {}

  inline void setUp() noexcept {
    bit.setUp();
    box.setUp();
  }
  void setUp(const char* fen) {
    bit.setUp(fen);
    box.setUp(fen);
  }
  void setUp(std::string& fen) { setUp(fen.c_str()); }

  std::string& toString() const noexcept { return box.toString(); }

  friend std::ostream& operator<<(std::ostream& os, Board& board) {
    return os << board.box;
  }

private:
  BitBoard bit;
  MailBox box;
  uint8_t flags;
  enum Flag : uint8_t {
    w_castle_queenside = 0x01, w_castle_kingside = 0x02,
    b_castle_queenside = 0x04, b_castle_kingside = 0x08,

    white_to_move = 0x10,
  };
  int en_passant_square;
  int halfmove_clock;
  int fullmove_number;
};

#endif
