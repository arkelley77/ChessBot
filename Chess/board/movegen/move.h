#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include <sstream>

#include "../indexing.h"
#include "../pieces.h"

class Move {
public:
  enum Special : uint16_t {
    not_special = 0x0,
    promo = 0x4000,
    en_passant = 0x8000, // as in, allows the opponent to capture en passant
    castling = 0xc000
  };
  enum Promo : uint16_t {
    knight = 0x0 << 12,
    bishop = 0x1 << 12,
    rook = 0x2 << 12,
    queen = 0x3 << 12,
  };

  Move(int from, int to) : move(from | (to << 6)) {}
  Move(int from, int to, Special special) : move(from | (to << 6) | special) {}
  Move(int from, int to, Promo promo, Special special)
    : move(from | (to << 6) | promo | special) {}

  inline int getFromSquare() const noexcept {
    return move & from_square;
  }
  inline void setFromSquare(int from) noexcept {
    move = (move & ~from_square) | (from & from_square);
  }
  inline int getToSquare() const noexcept {
    return (move & to_square) >> 6;
  }
  inline void setToSquare(int to) noexcept {
    move = (move & ~to_square) | ((to << 6) & to_square);
  }

  inline bool operator==(Move& rhs) const noexcept { return move == rhs.move; }

  // get which piece a pawn is being promoted to
  inline Promo getPromoType() const noexcept {
    return static_cast<Promo>(move & promo_type);
  }
  inline Piece::Type getPromoPieceType() const noexcept {
    switch (getSpecial()) {
    case knight: return Piece::knight;
    case bishop: return Piece::bishop;
    case rook: return Piece::rook;
    case queen: return Piece::queen;
    default: return Piece::square_type;
    }
  }
  // set which piece a pawn is being promoted to
  inline void setPromoType(Promo promo) noexcept {
    move = (move & ~promo_type) | promo;
  }

  inline Special getSpecial() const noexcept {
    return static_cast<Special>(move & special_flag);
  }
  inline void setSpecial(Special special) noexcept {
    move = (move & ~special_flag) | special;
  }

  std::string toString() const noexcept {
    std::string st;
    using Indexing::idxToString;
    switch (getSpecial()) {
    case promo:
      st += idxToString(getFromSquare()) + " -> " + idxToString(getToSquare());
      st += ": +" + getPromoPieceType();
      return st;
    default:
      st += idxToString(getFromSquare()) + " -> " + idxToString(getToSquare());
      return st;
    }
  }

  friend std::ostream& operator<<(std::ostream& os, Move& move) {
    return os << move.toString();
  }

private:
  uint16_t move;
  enum Segment : uint16_t {
    from_square = 0x3f,       // bits 0-5 (lowest 6)
    to_square = 0xfc0,        // bits 6-11
    promo_type = 0x3000,      // bits 12-13
    special_flag = 0xc000,    // bits 14-15
  };
  constexpr static inline Promo toPromo(Piece::Type type) noexcept {
    switch (type) {
    case Piece::knight: return knight;
    case Piece::bishop: return bishop;
    case Piece::rook: return rook;
    case Piece::queen: return queen;
    }
  }
};

#endif // MOVE_H
