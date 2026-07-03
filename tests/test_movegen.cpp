#include "movegen.h"
#include "attacks.h"
#include "zobrist.h"
#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace {

std::vector<std::string> to_uci_sorted(const MoveList& list) {
    std::vector<std::string> moves;
    moves.reserve(list.count);
    for (std::size_t i = 0; i < list.count; ++i) {
        moves.push_back(move_to_uci(list.moves[i]));
    }
    std::sort(moves.begin(), moves.end());
    return moves;
}

} // namespace

TEST_CASE("Pseudo-legal move generation returns 20 moves in the starting position", "[movegen]") {
    attacks::init();
    zobrist::init();

    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    MoveList list;
    generatePseudoLegalMoves(board, list);

    REQUIRE(list.count == 20);

    const std::vector<std::string> expected = {
        "a2a3", "a2a4", "b1a3", "b1c3", "b2b3", "b2b4", "c2c3", "c2c4",
        "d2d3", "d2d4", "e2e3", "e2e4", "f2f3", "f2f4", "g1f3", "g1h3",
        "g2g3", "g2g4", "h2h3", "h2h4"
    };

    REQUIRE(to_uci_sorted(list) == expected);
}
