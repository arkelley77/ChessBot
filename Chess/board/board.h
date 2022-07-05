#ifndef BOARD_H
#define BOARD_H

// defines a complete board representation comprised of
// both a BitBoard and a Mailbox

#include <array>
#include <string>

#include "bitboards/bitboards.h"
#include "indexing.h"
#include "movegen/movegen.h"
#include "pieces.h"

class Board {
public:
  inline Board() noexcept : bitboards(), mailbox(), flags()
    , en_passant_square() { clear(); }
  inline Board(Board& to_copy) noexcept : bitboards(to_copy.bitboards)
    , mailbox(to_copy.mailbox), flags(to_copy.flags)
    , en_passant_square(to_copy.en_passant_square) {}

  inline Board& operator=(const Board& rhs) noexcept {
    bitboards = rhs.bitboards;
    mailbox = rhs.mailbox;
    flags = rhs.flags;
    en_passant_square = rhs.en_passant_square;
  }

  inline void clear() noexcept {
    bitboards.fill(0x0);
    mailbox.fill(Piece::square);
    flags = 0;
    en_passant_square = -1;
  }

  // set up the Board based on a position defined by Forsyth-Edwards Notation
  // the default value for fen represents the starting position
  void setUp(const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  inline void setUp(std::string& fen) { setUp(fen.c_str()); }

  inline bool isWhitesMove() const noexcept { return flags & white_to_move; }
  inline bool isBlacksMove() const noexcept { return !isWhitesMove(); }
  inline void switchMoveSide() noexcept { flags ^= white_to_move; }
  inline void makeWhitesMove() noexcept { flags |= white_to_move; }
  inline void makeBlacksMove() noexcept { flags &= ~white_to_move; }

  // Read which piece is on the desired square on the board
  Piece::Name getPiece(int idx) noexcept { return mailbox[idx]; }
  // Removes any piece from the board square specified
  // Returns the piece removed
  Piece::Name rmPiece(int idx) noexcept;
  // Drops a piece onto the board square specified
  // ! If there is already a piece there, this WILL corrupt the bitboard
  void dropPiece(Piece::Name p, int idx) noexcept;
  // Removes a piece from the board square specified, then
  // replaces it with the piece specified
  // Returns the piece removed
  inline Piece::Name pushPiece(Piece::Name p, int idx) noexcept {
    Piece::Name old_piece = rmPiece(idx);
    dropPiece(p, idx);
    return old_piece;
  }

  std::vector<Move> getAllMoves() const noexcept;

  Piece::Name executeMove(Move move) noexcept;

  std::string getBuffer() const noexcept;
  std::string getBuffer(std::vector<Move>& moves) const noexcept;
  inline std::string toString() const noexcept { return Userspace::getPrettyPrint(getBuffer()); }
  std::string getBuffer(int selected_piece) const noexcept;

  inline friend std::ostream& operator<<(std::ostream& os, Board& board) { return os << board.toString(); }

private:
  constexpr static inline size_t num_bitboards = 8;
  std::array<Bitboards::bb, num_bitboards> bitboards;
  // the index in boards where each item lies
  // black & white contain all black/white pieces,
  //    regardless of type
  // & each piece board contains all pieces of that type,
  //    regardless of color
  enum IDX : size_t {
    black = 0, white = 1,
    pawns = 2, knights = 3, bishops = 4,
    rooks = 5, queens = 6, kings = 7,
  };

  std::array<Piece::Name, 64> mailbox;

  uint8_t flags;
  enum Flag : uint8_t {
    w_castle_queenside = 0x01, w_castle_kingside = 0x02,
    b_castle_queenside = 0x04, b_castle_kingside = 0x08,

    white_to_move = 0x10,
  };

  int en_passant_square;
};

#endif
