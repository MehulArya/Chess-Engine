#pragma once
#include "types.h"

enum class MoveFlag : std::uint8_t{
	Quiet = 0,
	Capture = 1,
	KingCastle = 3,
	QueenCastle = 4,
	EnPassant = 5,
	Promotion = 6,
	PromoCapture = 7
};

struct Move{
	std::uint32_t v{0};
	static Move make(Square from, Square to, Piece moved, Piece captured, PieceType promo, MoveFlag flag);

	Square from() const;
	Square to() const;
	Piece moved() const;
	Piece captured() const;
	Piece promo() const;
	MoveFlag flag() const;

	bool is_capture() const;
	bool is_promotion() const;
};

std::string move_to_uci(const Move &m);
bool parse_uci_move(const std::string &s, Square &from, Square &to, PieceType &promo);

