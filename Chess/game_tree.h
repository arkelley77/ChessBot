#ifndef GAME_TREE_H
#define GAME_TREE_H

// defines the game tree for chess, 
// including traversal

#include <vector>

#include "game.h"

struct GameNode {
  Game state;
  std::vector<GameNode> children;
}

#endif
