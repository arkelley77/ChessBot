add_subdirectory ("piece")
add_subdirectory ("bitboard")
add_subdirectory ("mailbox")

add_library (Board board.h board.cpp "move.h")
