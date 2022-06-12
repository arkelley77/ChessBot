// Chess.cpp : Defines the entry point for the application.
//

#include "Chess.h"

using namespace std;

int main()
{
  cout << "--------\n";
  BitBoard board;
  cout << board << "\n--------\n";
  board.setUp();
  cout << board << "\n--------\n";
  board.setUp("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
  cout << board << "\n--------" << endl;
  /*
  Should output the following:
  --------
          
          
          
          
          
          
          
          
  --------
  rnbqkbnr
  pppppppp
          
          
          
          
  PPPPPPPP
  RNBQKBNR
  --------
  rnbqkbnr
  pp ppppp
          
    p     
      P   
       N  
  PPPP PPP
  RNBQKB R
  --------

  */

  cout << board.toString(board.light_squares, '#') << "\n--------\npawn moves" << endl;

  std::vector<BitBoard::bb> pawn_moves = board.genPawnMoves(true, 0);
  cout << board.toString(pawn_moves[0] | pawn_moves[1] | pawn_moves[2]);
  cout << "\n--------" << endl;

  cout << "knight moves" << endl;
  std::vector<BitBoard::bb> knight_moves = board.genKnightMoves(true);
  cout << board.toString(knight_moves[0] | knight_moves[1]);
  cout << "\n--------" << endl;

  cout << "new setup" << endl;
  board.setUp("/3n/4B//// ");

  cout << board << "\n--------\nbishop moves" << endl;

  std::vector<BitBoard::bb> bishop_moves = board.genBishopMoves(true);

  cout << board.toString(bishop_moves[0]) << endl;

  cout << "--------" << endl;

  cout << "new setup" << endl;
  board.setUp("/3n1R///// ");

  cout << board << "\n--------\nrook moves" << endl;

  std::vector<BitBoard::bb> rook_moves = board.genRookMoves(true);

  cout << board.toString(rook_moves[0]) << endl;

  cout << "--------" << endl;

  cout << "new setup" << endl;
  board.setUp("/3n1Q/4Q//// ");

  cout << board << "\n--------\nqueen moves" << endl;

  std::vector<BitBoard::bb> queen_moves = board.genQueenMoves(true);

  cout << board.toString(queen_moves[0]) << endl;

  cout << "--------" << endl;

  cout << board.toString(queen_moves[1]) << endl;

  cout << "--------" << endl;
  return 0;
}
