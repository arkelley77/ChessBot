#ifndef PIECE_H
#define PIECE_H

/* custom char functions to avoid int casting */
constexpr inline static char tolower(char c) noexcept
{
  return ('A' <= c && c <= 'Z') ? c + 'a' - 'A' : c;
}
constexpr inline static char islower(char c) noexcept { return 'a' <= c && c <= 'z'; }
constexpr inline static char isupper(char c) noexcept { return 'A' <= c && c <= 'Z'; }

namespace Piece {
  // what each piece is represented by in FEN notation
  enum Name : const char {
    // the name of an empty board square
    square = ' ',

    black_pawn = 'p',

    black_knight = 'n',
    black_bishop = 'b',
    black_rook = 'r',

    black_king = 'k',
    black_queen = 'q',

    white_pawn = 'P',

    white_knight = 'N',
    white_bishop = 'B',
    white_rook = 'R',

    white_king = 'K',
    white_queen = 'Q',
  };
  // lower-case version of Name - removing color from the mix
  enum Type : const char {
    // the type of an empty board square
    square_type = ' ',

    pawn = 'p',

    knight = 'n',
    bishop = 'b',
    rook = 'r',

    king = 'k',
    queen = 'q',
  };
  // removes piece type from consideration, focusing only on color
  enum Color : const char {
    // the color of an empty board square
    square_color = ' ',

    black = 'b',
    white = 'w',
  };

  // converts a piece Name to a Type for easy comparison
  constexpr inline Type getType(Name p) noexcept {
    return static_cast<Type>(tolower(static_cast<char>(p)));
  }
  constexpr inline Name makePiece(Type t, bool white) {
    return static_cast<Name>((white) ? toupper(static_cast<char>(t)) : t);
  }

  constexpr inline bool isPawn(Name p) noexcept { return getType(p) == pawn; }
  constexpr inline bool isKing(Name p) noexcept { return getType(p) == king; }
  constexpr inline bool isQueen(Name p) noexcept { return getType(p) == queen; }
  constexpr inline bool isRook(Name p) noexcept { return getType(p) == rook; }
  constexpr inline bool isKnight(Name p) noexcept { return getType(p) == knight; }
  constexpr inline bool isBishop(Name p) noexcept { return getType(p) == bishop; }

  constexpr inline bool isWhite(Name p) noexcept {
    return isupper(static_cast<char>(p));
  }
  constexpr inline bool isBlack(Name p) noexcept {
    return islower(static_cast<char>(p));
  }
  constexpr inline bool isSquare(Name p) noexcept { return p == square; }
  constexpr inline Color getColor(Name p) noexcept {
    return (isWhite(p)) ? white : ((isBlack(p)) ? black : square_color);
  }

  // determines whether a char is a valid piece name in FEN
  // does not return true if c is a space (this method is used for validating FEN)
  constexpr inline bool isValidName(char c) noexcept {
    return (
      (isupper(c) && (c == white_pawn || c == white_knight || c == white_bishop
        || c == white_rook || c == white_queen || c == white_king))
      || (islower(c) && (c == black_pawn || c == black_knight || c == black_bishop
        || c == black_rook || c == black_queen || c == black_king))
      );
  }
  constexpr inline bool isValidType(char c) noexcept {
    return c == square_type || c == pawn || c == knight || c == bishop
      || c == rook || c == king || c == queen;
  }
  constexpr inline bool isValidColor(char c) noexcept {
    return c == square_color || c == white || c == black;
  }
}

#endif // PIECE_H
