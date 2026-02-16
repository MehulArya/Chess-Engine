# CMake generated Testfile for 
# Source directory: /home/buzz007/Desktop/ChessEngine/Chess-Engine/tests
# Build directory: /home/buzz007/Desktop/ChessEngine/Chess-Engine/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[engine_tests]=] "/home/buzz007/Desktop/ChessEngine/Chess-Engine/build/tests/engine_tests")
set_tests_properties([=[engine_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/buzz007/Desktop/ChessEngine/Chess-Engine/tests/CMakeLists.txt;15;add_test;/home/buzz007/Desktop/ChessEngine/Chess-Engine/tests/CMakeLists.txt;0;")
subdirs("../_deps/catch2-build")
