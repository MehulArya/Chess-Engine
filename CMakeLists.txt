cmake_minimum_required(VERSION 3.20)
project(classic_chess LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(ENGINE_ENABLE_SANITIZERS "Enable ASan/UBSan in Debug" ON)
add_library(engine
src/move.cpp
src/board.cpp
src/attacks.cpp
src/movegen.cpp
src/zobrist.cpp
src/tt.cpp
src/eval.cpp
src/search.cpp
src/uci_like.cpp
)
target_include_directories(engine PUBLIC src)
add_executable(chesscli src/main.cpp)
target_link_libraries(chesscli PRIVATE engine)
include(cmake/CompilerWarnings.cmake)
engine_set_warnings(engine)
engine_set_warnings(chesscli)
if(MSVC)
    target_compile_options(engine PRIVATE /permissive- /Zc:__cplusplus)
else()
    target_compile_options(engine PRIVATE -fno-omit-frame-pointer)
endif()

# Sanitizers (non-MSVC)
if(ENGINE_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(engine PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
    target_link_options(engine PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
    target_compile_definitions(engine PRIVATE
        $<$<CONFIG:Debug>:ENGINE_DEBUG>
    )
endif()
enable_testing()
add_subdirectory(tests)

