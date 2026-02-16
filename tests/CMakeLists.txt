include(FetchContent)
FetchContent_Declare(
Catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)
add_executable(engine_tests
test_perft.cpp
test_fen.cpp
test_make_unmake.cpp
test_search_smoke.cpp
)
target_link_libraries(engine_tests PRIVATE engine Catch2::Catch2WithMain)
add_test(NAME engine_tests COMMAND engine_tests)
