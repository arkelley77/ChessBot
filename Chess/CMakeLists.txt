# CMakeList.txt : CMake project for Chess, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Set the project name and version
project (Chess VERSION 0.1)

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_STANDARD_REQUIRED True)

# Add source to this project's executable.
add_executable (Chess "Chess.cpp" "Chess.h" "piece.cpp" "piece.h" "bitboard.cpp" "bitboard.h" "myModules/binary/binary.h")

# TODO: Add tests and install targets if needed.
