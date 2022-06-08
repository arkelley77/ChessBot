#ifndef BINARY_H
#define BINARY_H

#include <climits>
#include <type_traits>

namespace Binary {
  // template <class numeric, class u_int>

  #define VALID_NUMERIC std::conjunction<std::is_arithmetic<numeric>, std::negation<std::is_pointer<numeric>>>
  #define VALID_INTEGRAL std::conjunction<std::is_integral<integral>, std::negation<std::is_pointer<integral>>>
  #define VALID_U_INT std::conjunction<std::is_integral<u_int>, std::is_unsigned<u_int>, std::negation<std::is_pointer<u_int>>>

  #define NUMERIC_BIT (sizeof(numeric)*CHAR_BIT)
  #define INTEGRAL_BIT (sizeof(integral)*CHAR_BIT)
  #define U_INT_BIT (sizeof(u_int)*CHAR_BIT)

  // Rotates x bitwise right by num_bits bits
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    void> ror(numeric* x) noexcept {
    *x = (*x >> 1) | (*x << (NUMERIC_BIT - 1));
  }
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    numeric> ror(numeric x) noexcept {
    ror(&x);
    return x;
  }
  template <class numeric, class u_int>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC, VALID_U_INT>,
    void> ror(numeric& x, u_int num_bits) {
    *x = (*x >> num_bits) | (*x << (NUMERIC_BIT - num_bits));
  }
  template <class numeric, class u_int>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC, VALID_U_INT>,
    numeric> ror(numeric x, u_int num_bits) {
    ror(&x, num_bits);
    return x;
  }

  // Rotates x bitwise left by 1 bit
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    void> rol(numeric* x) noexcept {
    *x = (*x << 1) | (*x >> (NUMERIC_BIT - 1));
  }
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    void> rol(numeric& x) noexcept {
    rol(&x);
    return x;
  }
  template <class numeric, class u_int>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC, VALID_U_INT>,
    void> rol(numeric* x, u_int num_bits) noexcept {
    *x = (*x << num_bits) | (*x >> (NUMERIC_BIT - num_bits));
  }
  template <class numeric, class u_int>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC, VALID_U_INT>,
    numeric> rol(numeric x, u_int num_bits) noexcept {
    rol(&x, num_bits);
    return x;
  }

  // Counts the number of bits in x that are set to 1
  template <class numeric>
  constexpr std::enable_if_t<VALID_NUMERIC::value, int> countSetBits(numeric x) noexcept {
    if (!x) return 0;
    else if (!(x &= (x - 1))) return 1; // set LS1B to 0 by two's-complement magic

    // we've now established that at least 2 bits are set
    int count = 2;
    while (x &= x - 1) { // reset LS1B again
      // if the conditional evaluates true, we know there's another LS1B,
      // even though we haven't reset it yet
      ++count;
    } 
    return count;
  }

  // isolates the most significant 1 bit in x
  template <class numeric>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC>,
    void> isolateMS1B(numeric* x) noexcept {
    *x |= *x >> 1;
    if constexpr (NUMERIC_BIT > 2) *x |= *x >> 2;    // 4-bit finishes here
    if constexpr (NUMERIC_BIT > 4) *x |= *x >> 4;    // 8-bit
    if constexpr (NUMERIC_BIT > 8) *x |= *x >> 8;    // 16-bit
    if constexpr (NUMERIC_BIT > 16) *x |= *x >> 16;  // 32-bit
    if constexpr (NUMERIC_BIT > 32) *x |= *x >> 32;  // 64-bit
    if constexpr (NUMERIC_BIT > 64) *x |= *x >> 64;  // 128-bit, if it ever comes around
    // after that, every bit to the right of & including the MS1B is set
    // so do one right-shift to unset the MS1B
    // then a simple xor will tell us the result
    *x ^= *x >> 1;
  }
  template <class numeric>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC>,
    numeric> isolateMS1B(numeric x) noexcept {
    isolateMS1B(&x);
    return x;
  }

  // isolates the least significant 1 bit in x
  template <class numeric>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC>,
    void> isolateLS1B(numeric* x) noexcept {
    // two's-complement trickery
    // (-x) == (~x + 1)
    // ~x will create a group of 1s to the right of the LS1B preceded by a 0
    // then, when 1 is added, these 1s overflow into the 0
    // thus, x & (-x) results in only the LS1B being set
    *x &= (-*x);
    return *x;
  }
  template <class numeric>
  constexpr inline std::enable_if_t<std::conjunction_v<VALID_NUMERIC>,
    numeric> isolateLS1B(numeric x) noexcept {
    isolateLS1B(&x);
    return x;
  }

  // finds the index of the most significant 1 bit in x
  // relative to the least significant possible bit (rightmost bit)
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    int> indexOfMS1B(numeric x) noexcept {
    if (!x) return NUMERIC_BIT + 1;

    int ms1b_idx = 0;

    // performing binary search
    // this is an unrolled loop because:
    // a) I didn't want to have to deal with the different int sizes in a loop
    // b) it avoids jmp instructions which is great
    // mostly a because the compiler would take care of b

    if constexpr (NUMERIC_BIT > 64) {  // 128-bit start here
      if (x >= (1ULL << 64)) {
        ms1b_idx += 64;
        x = x >> 64;
      }
    }
    if constexpr (NUMERIC_BIT > 32) {  // 64-bit start here
      if (x >= (1ULL << 32)) {
        ms1b_idx += 32;
        x = x >> 32;
      }
    }
    if constexpr (NUMERIC_BIT > 16) {  // 32-bit start here
      if (x >= (1UL << 16)) {
        ms1b_idx += 16;
        x = x >> 16;
      }
    }
    if constexpr (NUMERIC_BIT > 8) {   // 16-bit start here
      if (x >= (1U << 8)) {
        ms1b_idx += 8;
        x = x >> 8;
      }
    }
    if constexpr (NUMERIC_BIT > 4) {   // 8-bit start here
      if (x >= (1U << 4)) {
        ms1b_idx += 4;
        x = x >> 4;
      }
    }
    if (x >= (1U << 2)) {
      ms1b_idx += 2;
      x = x >> 2;
    }
    if (x >= (1U
      << 1)) {
      ms1b_idx += 1;
      x = x >> 1;
    }
    return ms1b_idx;
  }

  // finds the index of the least significant 1 bit in x
  // relative to the least significant possible bit (rightmost bit)
  template <class numeric>
  constexpr inline std::enable_if_t<VALID_NUMERIC::value,
    int> indexOfLS1B(numeric x) noexcept {
    return indexOfMS1B(isolateLS1B(x));
  }

  #undef VALID_NUMERIC
  #undef VALID_INTEGRAL
  #undef VALID_U_INT

  #undef NUMERIC_BIT
  #undef INTEGRAL_BIT
  #undef U_INT_BIT
};

#endif
