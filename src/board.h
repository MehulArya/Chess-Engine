#pragma once
#include "types.h"
#include "move.h"
#include <string>
#include <array>
#include <cstdint>

struct Undo {
    Move move;
    Piece captured{Piece::Empty};
    std::uint8_t castling_rights{0};
    Square ep_square{Square(255)};
    std::uint16_t halfmove_clock{0};
    std::uint16_t fullmove_number{1};
    std::uint64_t zobrist_key{0};
};

class Board {
public:
    std::array<Piece, 64> squares{};
    Color side_to_move{Color::White};
    Square ep_square{Square(255)};
    std::uint8_t castling_rights{0};
    std::uint16_t halfmove_clock{0};
    std::uint16_t fullmove_number{1};
    std::uint64_t zobrist_key{0};

    Board() = default;   // Constructor
    void setFen(const std::string& fen);
    std::string getFen() const;

    void makeMove(Move move);
    void unmakeMove(const Undo& undo);

    bool isInCheck(Color c) const;
    bool isAttacked(Square s, Color by) const;
    Piece pieceAt(Square s) const { return squares[s]; }

private:
    void updateZobrist();
};

// Helper functions for UCI output
char file_to_char(int f);
char rank_to_char(int r);
