#include "board.h"
#include "zobrist.h"
#include <catch2/catch_test_macros.hpp>

namespace {

Undo make_undo(const Board& board, Move move, Piece captured = Piece::Empty) {
    return Undo{
        move,
        captured,
        board.castling_rights,
        board.ep_square,
        board.halfmove_clock,
        board.fullmove_number,
        board.zobrist_key
    };
}

void require_make_unmake_restores_fen(Board& board, Move move, Piece captured = Piece::Empty) {
    const std::string before = board.getFen();
    const Undo undo = make_undo(board, move, captured);

    board.makeMove(move);
    board.unmakeMove(undo);

    REQUIRE(board.getFen() == before);
}

} // namespace

TEST_CASE("Make and unmake restores a quiet move", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/8/8/8/8/8/4P3/4K3 w - - 0 1");

    Move move = Move::make(make_sq(4, 1), make_sq(4, 2), Piece::WP, Piece::Empty, PieceType::None, MoveFlag::Quiet);

    require_make_unmake_restores_fen(board, move);
}

TEST_CASE("Make and unmake restores a capture", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/8/8/8/8/5p2/4P3/4K3 w - - 0 1");

    Move move = Move::make(make_sq(4, 1), make_sq(5, 2), Piece::WP, Piece::BP, PieceType::None, MoveFlag::Capture);

    require_make_unmake_restores_fen(board, move, Piece::BP);
}

TEST_CASE("Make and unmake restores double pawn push state", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/8/8/8/8/8/4P3/4K3 w - - 0 1");

    Move move = Move::make(make_sq(4, 1), make_sq(4, 3), Piece::WP, Piece::Empty, PieceType::None, MoveFlag::DoublePawnPush);
    const Undo undo = make_undo(board, move);

    board.makeMove(move);
    REQUIRE(board.getFen() == "8/8/8/8/4P3/8/8/4K3 b - e3 0 1");

    board.unmakeMove(undo);
    REQUIRE(board.getFen() == "8/8/8/8/8/8/4P3/4K3 w - - 0 1");
}

TEST_CASE("Make and unmake restores a promotion", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/4P3/8/8/8/8/8/4K3 w - - 0 1");

    Move move = Move::make(make_sq(4, 6), make_sq(4, 7), Piece::WP, Piece::Empty, PieceType::Queen, MoveFlag::Promotion);
    const Undo undo = make_undo(board, move);

    board.makeMove(move);
    REQUIRE(board.getFen() == "4Q3/8/8/8/8/8/8/4K3 b - - 0 1");

    board.unmakeMove(undo);
    REQUIRE(board.getFen() == "8/4P3/8/8/8/8/8/4K3 w - - 0 1");
}

TEST_CASE("Make and unmake restores en passant", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/8/8/3pP3/8/8/8/4K3 w - d6 0 1");

    Move move = Move::make(make_sq(4, 4), make_sq(3, 5), Piece::WP, Piece::BP, PieceType::None, MoveFlag::EnPassant);
    const Undo undo = make_undo(board, move, Piece::BP);

    board.makeMove(move);
    REQUIRE(board.getFen() == "8/8/3P4/8/8/8/8/4K3 b - - 0 1");

    board.unmakeMove(undo);
    REQUIRE(board.getFen() == "8/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
}

TEST_CASE("Make and unmake restores white castling", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("8/8/8/8/8/8/8/R3K2R w KQ - 0 1");

    Move king_side = Move::make(make_sq(4, 0), make_sq(6, 0), Piece::WK, Piece::Empty, PieceType::None, MoveFlag::KingCastle);
    require_make_unmake_restores_fen(board, king_side);

    Move queen_side = Move::make(make_sq(4, 0), make_sq(2, 0), Piece::WK, Piece::Empty, PieceType::None, MoveFlag::QueenCastle);
    require_make_unmake_restores_fen(board, queen_side);
}

TEST_CASE("Make and unmake restores black castling", "[make-unmake]") {
    zobrist::init();

    Board board;
    board.setFen("r3k2r/8/8/8/8/8/8/8 b kq - 0 1");

    Move king_side = Move::make(make_sq(4, 7), make_sq(6, 7), Piece::BK, Piece::Empty, PieceType::None, MoveFlag::KingCastle);
    require_make_unmake_restores_fen(board, king_side);

    Move queen_side = Move::make(make_sq(4, 7), make_sq(2, 7), Piece::BK, Piece::Empty, PieceType::None, MoveFlag::QueenCastle);
    require_make_unmake_restores_fen(board, queen_side);
}
