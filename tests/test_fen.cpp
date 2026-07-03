#include "move.h"
#include "board.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UCI coordinate moves round-trip through parser and formatter", "[move]") {
    Square from{};
    Square to{};
    PieceType promo{};

    REQUIRE(parse_uci_move("e2e4", from, to, promo));
    REQUIRE(from == make_sq(4, 1));
    REQUIRE(to == make_sq(4, 3));
    REQUIRE(promo == PieceType::None);

    Move move = Move::make(from, to, Piece::WP, Piece::Empty, promo, MoveFlag::Quiet);
    REQUIRE(move_to_uci(move) == "e2e4");
}

TEST_CASE("UCI promotion moves preserve promotion piece", "[move]") {
    Square from{};
    Square to{};
    PieceType promo{};

    REQUIRE(parse_uci_move("e7e8q", from, to, promo));
    REQUIRE(from == make_sq(4, 6));
    REQUIRE(to == make_sq(4, 7));
    REQUIRE(promo == PieceType::Queen);

    Move move = Move::make(from, to, Piece::WP, Piece::Empty, promo, MoveFlag::Promotion);
    REQUIRE(move_to_uci(move) == "e7e8q");
}

TEST_CASE("UCI parser rejects invalid coordinates and promotion pieces", "[move]") {
    Square from{};
    Square to{};
    PieceType promo{};

    REQUIRE_FALSE(parse_uci_move("i2e4", from, to, promo));
    REQUIRE_FALSE(parse_uci_move("e2e9", from, to, promo));
    REQUIRE_FALSE(parse_uci_move("e7e8x", from, to, promo));
    REQUIRE_FALSE(parse_uci_move("e2e", from, to, promo));
}

TEST_CASE("Board preserves the standard starting FEN", "[fen]") {
    Board board;
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    board.setFen(fen);

    REQUIRE(board.getFen() == fen);
}

TEST_CASE("Board preserves FEN state fields", "[fen]") {
    Board board;
    const std::string fen = "r3k2r/pppq1ppp/2npbn2/3Np3/2B1P3/2N2Q2/PPP2PPP/R3K2R b KQkq e3 7 12";

    board.setFen(fen);

    REQUIRE(board.getFen() == fen);
}

TEST_CASE("Board serializes missing castling and en passant fields", "[fen]") {
    Board board;
    const std::string fen = "8/8/8/3k4/8/4K3/8/8 w - - 42 88";

    board.setFen(fen);

    REQUIRE(board.getFen() == fen);
}
