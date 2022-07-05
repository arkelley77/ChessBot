#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <string>
#include "indexing.h"

namespace Userspace {
  inline std::string getPrettyPrint(const std::string& mailbox_rep) {
    std::string buf = "   +---+---+---+---+---+---+---+---+\n ";

    for (int rank = 7; rank >= 0; --rank) {
      buf += std::to_string(rank + 1);
      for (int file = 0; file < 8; ++file) {
        buf += " | ";
        buf += mailbox_rep[Indexing::getIDX(rank, file)];
      }
      buf += " |\n   +---+---+---+---+---+---+---+---+\n ";
    }
    buf += "    a   b   c   d   e   f   g   h\n";

    return buf;
  }
}

#endif // PRETTY_PRINT_H
