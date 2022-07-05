#ifndef INDEXING_H
#define INDEXING_H

#include <string>

namespace Indexing {
  enum Direction : int {
                      n_n_west = -6, n_n_east = 10,
  w_n_west = -15, n_west = -7, north =  1, n_east = 9, e_n_east = 17,
                   west = -8,              east = 8,
  w_s_west = -17, s_west = -9, south = -1, s_east = 7, e_s_east = 15,
                      s_s_west = -10, s_s_east = 6
  };

  enum FileIDX : int {
    a = 0, b = 8, c = 16, d = 24, e = 32, f = 40, g = 48, h = 56
  };
  enum RankIDX : int {
    r1 = 0, r2 = 1, r3 = 2, r4 = 3, r5 = 4, r6 = 5, r7 = 6, r8 = 7
  };

  inline int getIDX(int row, int col) noexcept {
    return col * 8 + row;
  }

  inline int getFileIDX(int idx) noexcept {
    return idx / 8;
  }
  inline char getFile(int idx) noexcept {
    return 'a' + getFileIDX(idx);
  }

  inline int getRankIDX(int idx) noexcept {
    return idx % 8;
  }
  inline char getRank(int idx) noexcept {
    return '1' + getRankIDX(idx);
  }

  inline std::string idxToString(int idx) noexcept {
    std::string st;
    st.push_back(getFile(idx));
    st.push_back(getRank(idx));
    return st;
  }

  inline int stringToIdx(std::string coords) {
    int idx = 0;
    if (coords.size() != 2) throw std::invalid_argument("");
    if ('a' <= coords[0] && coords[0] <= 'h') {
      idx += 8 * (coords[0] - 'a');
    }
    else if ('A' <= coords[0] && coords[0] <= 'H') {
      idx += 8 * (coords[0] - 'A');
    }
    else throw std::invalid_argument("");
    if ('1' <= coords[1] && coords[1] <= '8') {
      idx += coords[1] - '1';
    }
    else throw std::invalid_argument("");
    return idx;
  }
}

#endif // INDEXING_H
