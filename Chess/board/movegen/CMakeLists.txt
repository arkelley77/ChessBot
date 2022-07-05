add_library (Movegen "movegen.cpp" "tests.cpp")

add_executable (testMovegen "tests.cpp")
target_link_libraries (testMovegen Bitboards Movegen)
