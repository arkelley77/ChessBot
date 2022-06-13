#include "mailbox.h"

inline MailBox::MailBox(const Piece::Name board[]) noexcept {
  for (size_t i = 0; i < 64; ++i) {
    this->board[i] = board[i];
  }
}
inline MailBox::MailBox(const MailBox& to_copy) noexcept {
  for (size_t i = 0; i < 64; ++i) {
    board[i] = to_copy.board[i];
  }
}

constexpr inline void MailBox::clear() noexcept {
  for (size_t i = 0; i < 64; ++i) {
    board[i] = Piece::square;
  }
}

inline void MailBox::setUp() {
  setUp("rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR ");
}

void MailBox::setUp(const char* fen) {
#define THROW_INVALID_FEN throw std::invalid_argument("Invalid FEN")
  if (fen == nullptr) THROW_INVALID_FEN;
  size_t i = 0;
  for (; fen[i] != ' '; ++i) {
    size_t row = 7, col = 0;
    if (fen[i] == '\0') THROW_INVALID_FEN; // ended early
    else if (row > 7 || col >= 8) THROW_INVALID_FEN; // too many squares
    else if (fen[i] == '/') {
      --row;
      col = 0;
    }
    else if ('0' < fen[i] && fen[i] <= '8') {
      col += fen[i] - '0';
    }
    else if (Piece::isValidName(fen[i])) {
      board[row + col * 8] = static_cast<Piece::Name>(fen[i]);
    }
    else THROW_INVALID_FEN;
  }
#undef THROW_INVALID_FEN
}

std::string MailBox::toString() const noexcept {
  std::string buf(71, ' '); // 64 squares + 7 newlines = 71 chars
  for (size_t i = 8; i < 71; i += 9) {
    buf[i] = '\n';
  }
  for (size_t row = 0; row < 8; ++row) {
    for (size_t col = 0; col < 8; ++col) {
      buf[col + 9 * row] = board[row + 8 * col];
    }
  }
  return buf;
}
