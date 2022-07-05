#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <vector>
#include <string>
#include <sstream>

#include "../../../include/myModules/binary/binary.h"
#include "../pretty_print.h"
#include "../indexing.h"
#include "../pieces.h"

namespace Bitboards {
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
  constexpr inline File files[8] = { a, b, c, d, e, f, g };
  // mask representing each rank
  // for example, row 1 is the lowest bit in every byte,
  // or 0x0101010101010101, while row 8 is the highest bit
  enum Rank : const bb {
    r1 = 0x0101010101010101, r2 = 0x0202020202020202,
    r3 = 0x0404040404040404, r4 = 0x0808080808080808,
    r5 = 0x1010101010101010, r6 = 0x2020202020202020,
    r7 = 0x4040404040404040, r8 = 0x8080808080808080,

    north_mask = ~r1, south_mask = ~r8,
    dbl_north_mask = north_mask & ~r2, dbl_south_mask = south_mask & ~r7,
    quad_north_mask = dbl_north_mask & ~r3 & ~r4, quad_south_mask = dbl_south_mask & ~r6 & ~r5
  };
  // indexed list of ranks for easy lookup
  constexpr inline Rank ranks[8] = { r1, r2, r3, r4, r5, r6, r7, r8 };
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
  constexpr inline Square squares[64] = {
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8,
  };

  constexpr inline bool isOnSquare(const bb& piece, const Square& sq) noexcept { return piece & sq; }
  constexpr inline bool isOnFile(const bb& piece, const File& f) noexcept { return piece & f; }
  constexpr inline bool isOnRank(const bb& piece, const Rank& r) noexcept { return piece & r; }

  // returns the file within which lies the MS1B of piece
  constexpr inline int getCol(const bb& piece) noexcept { return Binary::indexOfMS1B(piece) / 8; }
  // converts a file index to a bitboard mask
  constexpr inline File colToBoard(int file) noexcept { return files[file]; }
  // returns the rank within which lies the MS1B of piece
  constexpr inline int getRow(const bb& piece) noexcept { return Binary::indexOfMS1B(piece) % 8; }
  // converts a rank index to a bitboard mask
  constexpr inline Rank rowToBoard(int rank) noexcept { return ranks[rank]; }
  // converts a square index to a bitboard representation
  // with only that square set to 1
  constexpr inline Square idxToBoard(int idx) { return static_cast<Square>(1ULL << idx); }

  constexpr inline bool getBit(const bb& board, int bit_idx) noexcept { return board & idxToBoard(bit_idx); }
  constexpr inline void setBit(bb& board, int bit_idx) noexcept { board |= idxToBoard(bit_idx); }

  // shift along the axes of the board

  constexpr inline bb shiftN(const bb& board) noexcept { return (board << Indexing::north) & north_mask; }
  constexpr inline bb shift2N(const bb& board) noexcept { return (board << 2 * Indexing::north) & dbl_north_mask; }
  constexpr inline bb shift4N(const bb& board) noexcept { return (board << 4 * Indexing::north) & quad_north_mask; }

  constexpr inline bb shiftS(const bb& board) noexcept { return (board >> -Indexing::south) & south_mask; }
  constexpr inline bb shift2S(const bb& board) noexcept { return (board >> -2 * Indexing::south) & dbl_south_mask; }
  constexpr inline bb shift4S(const bb& board) noexcept { return (board >> -4 * Indexing::south) & quad_south_mask; }

  constexpr inline bb shiftE(const bb& board) noexcept { return (board << Indexing::east); }
  constexpr inline bb shift2E(const bb& board) noexcept { return (board << 2 * Indexing::east); }
  constexpr inline bb shift4E(const bb& board) noexcept { return (board << 4 * Indexing::east); }

  constexpr inline bb shiftW(const bb& board) noexcept { return (board >> -Indexing::west); }
  constexpr inline bb shift2W(const bb& board) noexcept { return (board >> -2 * Indexing::west); }
  constexpr inline bb shift4W(const bb& board) noexcept { return (board >> -4 * Indexing::west); }

  // shift diagonally

  constexpr inline bb shiftNE(const bb& board) noexcept { return (board << Indexing::n_east) & north_mask; }
  constexpr inline bb shift2NE(const bb& board) noexcept { return (board << 2 * Indexing::n_east) & dbl_north_mask; }
  constexpr inline bb shift4NE(const bb& board) noexcept { return (board << 4 * Indexing::n_east) & quad_north_mask; }

  constexpr inline bb shiftNW(const bb& board) noexcept { return (board >> -Indexing::n_west) & north_mask; }
  constexpr inline bb shift2NW(const bb& board) noexcept { return (board >> -2 * Indexing::n_west) & dbl_north_mask; }
  constexpr inline bb shift4NW(const bb& board) noexcept { return (board >> -4 * Indexing::n_west) & quad_north_mask; }

  constexpr inline bb shiftSE(const bb& board) noexcept { return (board << Indexing::s_east) & south_mask; }
  constexpr  inline bb shift2SE(const bb& board) noexcept { return (board << 2 * Indexing::s_east) & dbl_south_mask; }
  constexpr  inline bb shift4SE(const bb& board) noexcept { return (board << 4 * Indexing::s_east) & quad_south_mask; }

  constexpr inline bb shiftSW(const bb& board) noexcept { return (board >> -Indexing::s_west) & south_mask; }
  constexpr inline bb shift2SW(const bb& board) noexcept { return (board >> -2 * Indexing::s_west) & dbl_south_mask; }
  constexpr inline bb shift4SW(const bb& board) noexcept { return (board >> -4 * Indexing::s_west) & quad_south_mask; }

  // shift according to knight moves

  constexpr inline bb shiftNNE(const bb& board) noexcept { return (board << Indexing::n_n_east) & dbl_north_mask; }
  constexpr inline bb shiftNNW(const bb& board) noexcept { return (board >> -Indexing::n_n_west) & dbl_north_mask; }
  constexpr inline bb shiftENE(const bb& board) noexcept { return (board << Indexing::e_n_east) & north_mask; }
  constexpr inline bb shiftWNW(const bb& board) noexcept { return (board >> -Indexing::w_n_west) & north_mask; }
  constexpr inline bb shiftESE(const bb& board) noexcept { return (board << Indexing::e_s_east) & south_mask; }
  constexpr inline bb shiftWSW(const bb& board) noexcept { return (board >> -Indexing::w_s_west) & south_mask; }
  constexpr inline bb shiftSSE(const bb& board) noexcept { return (board << Indexing::s_s_east) & dbl_south_mask; }
  constexpr inline bb shiftSSW(const bb& board) noexcept { return (board >> -Indexing::s_s_west) & dbl_south_mask; }

  // when available_squares is the result of a castShadow call, this performs an obstructed fill

  constexpr inline bb castRayN(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftN(pieces);
    pieces |= available_squares & shift2N(pieces);
    pieces |= available_squares & shift4N(pieces);
    return pieces;
  }
  constexpr inline bb castRayS(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftS(pieces);
    pieces |= available_squares & shift2S(pieces);
    pieces |= available_squares & shift4S(pieces);
    return pieces;
  }
  constexpr inline bb castRayE(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftE(pieces);
    pieces |= available_squares & shift2E(pieces);
    pieces |= available_squares & shift4E(pieces);
    return pieces;
  }
  constexpr inline bb castRayW(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftW(pieces);
    pieces |= available_squares & shift2W(pieces);
    pieces |= available_squares & shift4W(pieces);
    return pieces;
  }

  constexpr inline bb castRayNW(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftNW(pieces);
    pieces |= available_squares & shift2NW(pieces);
    pieces |= available_squares & shift4NW(pieces);
    return pieces;
  }
  constexpr inline bb castRayNE(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftNE(pieces);
    pieces |= available_squares & shift2NE(pieces);
    pieces |= available_squares & shift4NE(pieces);
    return pieces;
  }
  constexpr inline bb castRaySW(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftSW(pieces);
    pieces |= available_squares & shift2SW(pieces);
    pieces |= available_squares & shift4SW(pieces);
    return pieces;
  }
  constexpr inline bb castRaySE(bb pieces, bb available_squares = -1) noexcept {
    pieces |= available_squares & shiftSE(pieces);
    pieces |= available_squares & shift2SE(pieces);
    pieces |= available_squares & shift4SE(pieces);
    return pieces;
  }

  // combines castRay and castShadow to create obstructed fill

  constexpr inline bb obstructedFillN(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftN(pieces);
    available_squares &= shiftN(available_squares);
    pieces |= available_squares & shift2N(pieces);
    available_squares &= shift2N(available_squares);
    pieces |= available_squares & shift4N(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillS(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftS(pieces);
    available_squares &= shiftS(available_squares);
    pieces |= available_squares & shift2S(pieces);
    available_squares &= shift2S(available_squares);
    pieces |= available_squares & shift4S(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillE(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftE(pieces);
    available_squares &= shiftE(available_squares);
    pieces |= available_squares & shift2E(pieces);
    available_squares &= shift2E(available_squares);
    pieces |= available_squares & shift4E(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillW(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftW(pieces);
    available_squares &= shiftW(available_squares);
    pieces |= available_squares & shift2W(pieces);
    available_squares &= shift2W(available_squares);
    pieces |= available_squares & shift4W(pieces);
    return pieces;
  }

  constexpr inline bb obstructedFillNW(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftNW(pieces);
    available_squares &= shiftNW(available_squares);
    pieces |= available_squares & shift2NW(pieces);
    available_squares &= shift2NW(available_squares);
    pieces |= available_squares & shift4NW(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillNE(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftNE(pieces);
    available_squares &= shiftNE(available_squares);
    pieces |= available_squares & shift2NE(pieces);
    available_squares &= shift2NE(available_squares);
    pieces |= available_squares & shift4NE(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillSW(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftSW(pieces);
    available_squares &= shiftSW(available_squares);
    pieces |= available_squares & shift2SW(pieces);
    available_squares &= shift2SW(available_squares);
    pieces |= available_squares & shift4SW(pieces);
    return pieces;
  }
  constexpr inline bb obstructedFillSE(bb pieces, bb available_squares) noexcept {
    // this method not only casts a ray (in the board variable),
    // it also casts a shadow (in the available_squares variable),
    // allowing for fewer operations to be performed overall.
    pieces |= available_squares & shiftSE(pieces);
    available_squares &= shiftSE(available_squares);
    pieces |= available_squares & shift2SE(pieces);
    available_squares &= shift2SE(available_squares);
    pieces |= available_squares & shift4SE(pieces);
    return pieces;
  }

  // Isolates each set bit of the bitboard, from most significant to least significant
  std::vector<bb> getEachPiece(bb board) noexcept;

  void buffer(bb board, std::string& buf, char one = 'o');
  inline std::string toBuffer(bb board, char one = 'o', std::string buf = std::string(64, ' ')) {
    buffer(board, buf, one);
    return buf;
  }
  inline std::string toString(bb board, char one = '#') {
    std::string buf = toBuffer(board, one);
    return Userspace::getPrettyPrint(buf);
  }
}

#endif // BITBOARDS_H
