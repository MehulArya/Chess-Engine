#include "eval.h"
#include "search.h"
#include "board.h"
#include "movegen.h"
#include "attacks.h"
#include "zobrist.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Evaluation is symmetric for starting position", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int score = evaluate(board);
    Board flipped;
    flipped.setFen("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr b KQkq - 0 1");
    int flipped_score = evaluate(flipped);
    REQUIRE(score == -flipped_score);
}

TEST_CASE("Evaluation gives white advantage for extra white pawn", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("rnbqkbnr/pppp1ppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3");
    int score = evaluate(board);
    REQUIRE(score > 50);
}

TEST_CASE("Evaluation detects material disadvantage for side down a pawn", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("8/8/8/8/4K3/8/4p3/4k3 w - - 0 1");
    int score = evaluate(board);
    REQUIRE(score < -50);
}

TEST_CASE("Search on starting position returns a legal move", "[search]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move best = search_best_move(board, 1);
    MoveList legal;
    generateLegalMoves(board, legal);
    bool found = false;
    for (std::size_t i = 0; i < legal.count; ++i) {
        if (legal.moves[i].v == best.v) found = true;
    }
    REQUIRE(found);
}

TEST_CASE("Depth 2 search does not crash on starting position", "[search]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    REQUIRE_NOTHROW(search_best_move(board, 2));
}

TEST_CASE("Evaluation does not crash on any position", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("8/8/8/8/8/2k5/2p5/2K5 w - - 0 1");
    REQUIRE_NOTHROW(evaluate(board));
}

TEST_CASE("King and pawn endgame evaluation favors side with pawn", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("8/8/8/8/4k3/8/4P3/4K3 w - - 0 1");
    int score = evaluate(board);
    REQUIRE(score > 0);
}

TEST_CASE("Evaluation is deterministic", "[eval]") {
    attacks::init();
    zobrist::init();
    Board board;
    board.setFen("r3k2r/pppq1ppp/2npbn2/3Np3/2B1P3/2N2Q2/PPP2PPP/R3K2R b KQkq e3 7 12");
    int s1 = evaluate(board);
    int s2 = evaluate(board);
    REQUIRE(s1 == s2);
}