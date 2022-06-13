#ifndef BITBOARD_H
#define BITBOARD_H

// defines the bitboard representation of the board,
// wich is a different way of saying it's a wrapper class around
// an array of 64-bit unsigned integers.
// each bit in an integer represents a board square, and each
// integer represents a type of piece.
// thus, bitwise operations can be used to move pieces & modify
// the board, making it more efficient for most operations
//
// For more info, read https://www.chessprogramming.org/Bitboards

#include <string>
#include <sstream>
#include <vector>

#include "myModules/binary/binary.h"
#include "piece.h"

class BitBoard {
public: // type definitions & enums
  // The bitboards are indexed row-major, little-endian.
  //   8   7 15 23 31 39 47 55 63 < MSB
  //   7   6 14 22 30 38 46 54 62
  //   6   5 13 21 29 37 45 53 61
  //   5   4 12 20 28 36 44 52 60   (bit indices from
  //   4   3 11 19 27 35 43 51 59    0 @ LSB to 63 @ MSB)
  //   3   2 10 18 26 34 42 50 58
  //   2   1  9 17 25 33 41 49 57
  //   1   0  8 16 24 32 40 48 56
  //  LSB  a  b  c  d  e  f  g  h
  typedef uint64_t bb;

  // mask representing each file
  // for example, column A is the lowest byte or 0xff
  // and column H is the highest byte
  enum File : const bb {
    a = 0x00000000000000ff, b = 0x000000000000ff00,
    c = 0x0000000000ff0000, d = 0x00000000ff000000,
    e = 0x000000ff00000000, f = 0x0000ff0000000000,
    g = 0x00ff000000000000, h = 0xff00000000000000
  };
  // indexed list of columns for easy lookup
  constexpr static inline File files[8] = { a, b, c, d, e, f, g };
  // mask representing each rank
  // for example, row 1 is the lowest bit in every byte,
  // or 0x0101010101010101, while row 8 is the highest bit
  enum Rank : const bb {
    r1 = 0x0101010101010101, r2 = 0x0202020202020202,
    r3 = 0x0404040404040404, r4 = 0x0808080808080808,
    r5 = 0x1010101010101010, r6 = 0x2020202020202020,
    r7 = 0x4040404040404040, r8 = 0x8080808080808080
  };
  // indexed list of ranks for easy lookup
  constexpr static inline Rank ranks[8] = { r1, r2, r3, r4, r5, r6, r7, r8 };
  // masks to represent the long diagonals
  enum Diagonal : const bb {
    a1_to_h8 = 0x8040201008040201,
    a8_to_h1 = 0x0102040810204080,
  };
  // each value represents the location of a specific
  // board square by having only that bit set to 1
  enum Square : const bb {
    a1 = a & r1, a2 = a & r2, a3 = a & r3, a4 = a & r4,
      a5 = a & r5, a6 = a & r6, a7 = a & r7, a8 = a & r8,
    b1 = b & r1, b2 = b & r2, b3 = b & r3, b4 = b & r4,
      b5 = b & r5, b6 = b & r6, b7 = b & r7, b8 = b & r8,
    c1 = c & r1, c2 = c & r2, c3 = c & r3, c4 = c & r4,
      c5 = c & r5, c6 = c & r6, c7 = c & r7, c8 = c & r8,
    d1 = d & r1, d2 = d & r2, d3 = d & r3, d4 = d & r4,
      d5 = d & r5, d6 = d & r6, d7 = d & r7, d8 = d & r8,
    e1 = e & r1, e2 = e & r2, e3 = e & r3, e4 = e & r4,
      e5 = e & r5, e6 = e & r6, e7 = e & r7, e8 = e & r8,
    f1 = f & r1, f2 = f & r2, f3 = f & r3, f4 = f & r4,
      f5 = f & r5, f6 = f & r6, f7 = f & r7, f8 = f & r8,
    g1 = g & r1, g2 = g & r2, g3 = g & r3, g4 = g & r4,
      g5 = g & r5, g6 = g & r6, g7 = g & r7, g8 = g & r8,
    h1 = h & r1, h2 = h & r2, h3 = h & r3, h4 = h & r4,
      h5 = h & r5, h6 = h & r6, h7 = h & r7, h8 = h & r8,
  };
  // indexed list of squares for easy lookup
  constexpr static inline Square squares[64] = {
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8,
  };
  
  // compass directions based on indexing system
  // see the comment on BitBoard::bb for a clearer picture

  constexpr static inline bb light_squares = 0x55AA55AA55AA55AA;
  constexpr static inline bb dark_squares = 0xAA55AA55AA55AA55;

  enum Direction : int {
    n_west = -7, north = 1,  n_east = 9,
    west = -8,                 east = 8,
    s_west = -9, south = -1, s_east = 7,

        n_n_west = -6, n_n_east = 10,
    w_n_west = -15,        e_n_east = 17,
    w_s_west = -17,        e_s_east = 15,
        s_s_west = -10, s_s_east = 6,
  };

public: // static methods
  constexpr static inline bool isOnSquare(const bb& piece, const Square& sq) noexcept {
    return piece & sq;
  }
  constexpr static inline bool isOnFile(const bb& piece, const File& f) noexcept {
    return piece & f;
  }
  constexpr static inline bool isOnRank(const bb& piece, const Rank& r) noexcept {
    return piece & r;
  }

  // returns the file within which lies the MS1B of piece
  constexpr static inline int getCol(const bb& piece) noexcept {
    return Binary::indexOfMS1B(piece) / 8;
  }
  // converts a file index to a bitboard mask
  constexpr static inline File colToBoard(int file) noexcept {
    return files[file];
  }
  // returns the rank within which lies the MS1B of piece
  constexpr static inline int getRow(const bb& piece) noexcept {
    return Binary::indexOfMS1B(piece) % 8;
  }
  // converts a rank index to a bitboard mask
  constexpr static inline Rank rowToBoard(int rank) noexcept {
    return ranks[rank];
  }
  // converts a square index to a bitboard representation
  // with only that square set to 1
  constexpr static inline Square idxToBoard(int idx) {
    return static_cast<Square>(1ULL << idx);
  }
  
  constexpr static inline void setBit(bb& board, int bit_idx) noexcept {
    board |= idxToBoard(bit_idx);
  }
  constexpr static inline bool getBit(const bb& board, int bit_idx) {
    return board & idxToBoard(bit_idx);
  }

  constexpr static inline bb shiftOne(const bb& board, Direction dir) noexcept {
    return (dir < 0) ? (board >> -dir) : (board << dir);
  }
  constexpr static inline bb smearOne(const bb& board, Direction dir) noexcept {
    return shiftOne(board, dir) | board;
  }

  static std::vector<bb> getEachPiece(bb board) noexcept;

  // converts board to a string while rotating left 90 degrees
  // for easy printing & reading
  static std::string toString(bb board, char one = '1') noexcept;

public: // instance methods
  inline BitBoard() noexcept { clear(); };
  inline BitBoard(BitBoard& to_copy) noexcept;
  
  // clear the BitBoard
  inline void clear() noexcept;

  // set up the BitBoard to match the starting position for chess
  void setUp() noexcept;
  
  // set up the BitBoard based on an arbitrary position
  // defined by Forsyth-Edwards Notation
  void setUp(const char* fen);
  inline void setUp(std::string& fen) { setUp(fen.c_str()); }

  // Place the desired piece on the board
  // ! Ignores any piece that may already be there, could corrupt the board
  // ! Undefined behavior when p is Piece::Name::square
  inline void pushPiece(Piece::Name p, const bb& square) noexcept;
  // Removes any piece from the board square specified
  inline void rmPiece(const bb& square) noexcept;
  // Removes a piece from the board square specified, then
  // replaces it with the piece specified
  inline void replacePiece(Piece::Name p, const bb& square) noexcept;
  
  // generates the forward moves for the pawns
  inline bb genPawnPushes(bool white_to_move) const noexcept;
  inline std::vector<bb> genPawnCaptures(bool white_to_move, const bb& en_passant_square) const noexcept;
  inline std::vector<bb> genPawnMoves(bool white_to_move, const bb& en_passant_square) const noexcept;
  inline bb genPawnThreats(const bb& pawn, bool white_to_move) const noexcept;

  std::vector<bb> genKnightMoves(bool white_to_move) const noexcept;
  inline bb genKnightThreats(const bb& knight) const noexcept;

  std::vector<bb> genBishopMoves(bool white_to_move) const noexcept;
  inline bb genBishopThreats(const bb& bishop) const noexcept;

  std::vector<bb> genRookMoves(bool white_to_move) const noexcept;
  inline bb genRookThreats(const bb& rook) const noexcept;

  std::vector<bb> genQueenMoves(bool white_move) const noexcept;
  inline bb genQueenThreats(const bb& queen) const noexcept;

  bb genKingMoves(bool white_to_move) const noexcept;
  inline bb genKingThreats(const bb& king) const noexcept;

  // used to determine check
  bb genThreats(bool white_to_move) const noexcept;

  // convert the entire board representation to a string which can be
  // easily printed out, with the proper piece names
  std::string toString() const noexcept;

  friend std::ostream& operator<<(std::ostream& os, const BitBoard& board) {
    return os << board.toString();
  }
private:
  constexpr static inline size_t num_boards = 8;
  bb boards[num_boards];
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
};

#endif // BITBOARD_H
