# Chess

This is a chess bot I'm working on. The plan is to use minmax to select moves once I get all the board state & move generation algorithms working.

So far, all that works is the basic board representation (only via bitboard) and most legal move generation.
There are still several edge cases for me to debug, but currently it runs at about 20% the speed of perft.

# Roadmap
1) Fix legal move generation
2) Optimize (SIMD?)
3) MCTS w/ neural net
