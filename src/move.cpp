#include "move.h"
#include <cassert>

Move Move::make(Square from, Square to, Piece moved, Piece captured, PieceType promo, MoveFlag flag){
	std::uint32_t v = 0;
	v |= (std::uint32_t(to) & 63u);
	v |= (std::uint32_t(from) & 63u) << 6;
	v |= (std::uint32_t(promo) & 15u) << 12;
	v |= (std::uint32_t(moved) & 15u) << 16;
	v |= (std::uint32_t(captured) & 15u) << 20;
	v |= (std::uint32_t(flag) & 15u) << 24;
	return Move{v};
}

Square Move::from() const { return Square((v >> 6) & 63u); }
Square Move::to() const { return Square(v & 63u); }
Piece Move::moved() const { return Piece((v >> 16) & 15u); }
Piece Move::captured() const { return Piece((v >> 20) & 15u); }
PieceType Move::promo() const { return PieceType((v >> 12) & 15u); }
MoveFlag Move::flag() const { return MoveFlag((v >> 24) & 15u); }

bool Move::is_capture() const{
	const auto f = flag();
	return f == MoveFlag::Promotion || f == MoveFlag::PromoCapture;
}

bool Move::is_promotion() const{
	const auto f = flag();
	return f == MoveFlag::Promotion || f == MoveFlag::PromoCapture;
}

// UCI
static char file_to_char(int f){ return char('a' + f); }
static char rank_to_char(int r){ return char('1' + r); }

std::string move_to_uci(const Move& m){
	auto f1 = file_of(m.from());
	auto r1 = rank_of(m.from());
	auto f2 = file_of(m.to());
	auto r2 = rank_of(m.to());

	std::string s;
	s += file_to_char(f1); 
	s += rank_to_char(r1);
	s += file_to_char(f2);
	s += file_to_char(r2);

	if(m.is_promotion()){
		char pc = 'q';
		switch(m.promo()){
			case PieceType::Queen: pc = 'q';
					       break;
			case PieceType::Rook: pc = 'r';
					      break;
			case PieceType::Bishop: pc = 'b';
					        break;
			case PieceType::Knight: pc = 'n';
					        break;
			default: break;
		}
		s += pc;
	}
	return s;
}


bool parse_uci_move(const std::string &s, Square &from, Square &to, PieceType &promo){
	if(s.size() < 4) return false;
	int f1 = s[0] - 'a', r1 = s[1] - '1';
	int f2 = s[2] - 'a', r2 = s[3] - '1';

	if(f1 < 0 || f1 > 7 || f2 < 0 || f2 > 7 || r1 < 0 || r1 > 7 || r2 < 0 || r2 > 7) return false;

	from = make_sq(f1, r1);
	to = make_sq(f2, r2);
	promo = PieceType::None;
	if(s.size() >= 5){
		switch (s[4]){
			case 'q': promo = PieceType::Queen;
				  break;
			case 'r': promo = PieceType::Rook;
				  break;
			case 'b': promo = PieceType::Bishop;
				  break;
			case 'n': promo = PieceType::Knight;
				  break;
			default: return false;
		}
	}

	return true;
}
