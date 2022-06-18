# CMakeList.txt : CMake project for Chess, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Set the project name and version
project (Chess VERSION 0.1)

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory ("board")

# Add source to this project's executable.
add_executable (Chess Chess.h Chess.cpp)

# TODO: Add tests and install targets if needed.


# Link libraries
target_link_libraries(Chess PUBLIC Board Bitboard)


target_include_directories(Chess PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/include")
