#pragma once

#include "board.h"
#include <array>
#include <cstddef>

struct MoveList {
    std::array<Move, MAX_MOVES> moves{};
    std::size_t count{0};

    void clear() { count = 0; }
    void add(Move move) { moves[count++] = move; }
};

void generatePseudoLegalMoves(const Board& board, MoveList& out);
