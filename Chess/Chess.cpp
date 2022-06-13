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

  cout << board.toString(board.light_squares, '#') << "\n--------" << endl;

  cout << "Black threats\n";
  cout << board.toString(board.genThreats(true)) << "\n--------" << endl;
  cout << "White threats\n";
  cout << board.toString(board.genThreats(false)) << "\n--------" << endl;
  return 0;
}
