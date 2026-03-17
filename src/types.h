#pragma once
#include<cstdint>
#include<string>
#include<string_view>
#include<array>
#include<vector>

using Bitboard = std::uint64_t;

enum class Color : std::uint8_t {
	White = 0, Black = 1
};

inline constexpr Color operator!(Color c){
	return (c == Color::White) ? Color::Black : Color::White;
}

enum class Piece : std::uint8_t{
	Empty = 0,
	WP, WN, WB, WR, WQ, WK,
	BP, BN, BB, BR, BQ, BK
};

enum class PieceType : std::uint8_t{
	Pawn, Knight, Bishop, Rook, Queen, King, None
};

enum class Castling : std::uint8_t{
	None = 0,
	WK = 1 << 0,
	WQ = 1 << 1,
	BK = 1 << 2,
	BQ = 1 << 3	
};

inline constexpr Castling operator|(Castling a, Castling b){
	return static_cast<Castling>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}	

using Square = std::uint8_t;

inline constexpr Square make_sq(int file, int rank){
	return Square(rank * 8 + file);
}

inline constexpr int file_of(Square s){
	return int(s) & 7;
}

inline constexpr int rank_of(Square s){
	return int(s) >> 3;
}


inline constexpr Bitboard bb_of(Square s){
	return Bitboard(1) << s;
}

inline constexpr int MAX_PLY = 128;
inline constexpr int MAX_MOVES = 268;
