#include<attacks.h>
#include<array>

namespace attacks{
	static std::array<Bitboard, 64> KNIGHT{};
	static std::array<Bitboard, 64> KING{};
	static std::array<Bitboard, 64> WPAWN{};
	static std::array<Bitboard, 64> BPAWN{};

	static bool on_board(int f, int r){ return f >= 0 && f < 8 && r >= 0 && r < 8; }

	void init(){
		for(int sq = 0; sq < 64; sq++){
			int f = sq & 7;
			int r = sq >> 3;

			// Knight
			KNIGHT[sq] = 0;
			const int kdf[8] = {+1, +2, +2, +1, -1, -2, -2, -1};
			const int kdr[8] = {+2, +1, -1, -2, -2, -1, +1, +2};

			for(int i = 0; i < 8; i++){
				int nf = f + kdf[i];
				int nr = r + kdr[i];
				if(on_board(nf, nr)) KNIGHT[sq] |= bb_of(make_sq(nf, nr));
			}

			//King
			KING[sq] = 0;
			for(int df = -1; df <= 1; ++df){
				for (int dr = -1; dr <= 1; ++dr){
					if(df == 0 && dr == 0) continue;
					int nf = f + df, nr = r + dr;
					if (on_board(nf, nr)) KING[sq] |= bb_of(make_sq(nf, nr));
				}
			}

			// Pawn Attacks
			WPAWN[sq] = BPAWN[sq] = 0;
			if(on_board(f - 1, r + 1)) WPAWN[sq] |= bb_of(make_sq(f - 1, r + 1));
			if(on_board(f + 1, r + 1)) WPAWN[sq] |= bb_of(make_sq(f + 1, r + 1));
		      	if(on_board(f - 1, r - 1)) BPAWN[sq] |= bb_of(make_sq(f - 1, r - 1));
		       	if(on_board(f + 1, r - 1)) BPAWN[sq] |= bb_of(make_sq(f + 1, r - 1));
		}
	}

Bitboard knight(Square s){ return KNIGHT[s]; }
Bitboard king(Square s){ return KING[s]; }
Bitboard pawn_attacks(Color c, Square s){ return (c == Color::White) ? WPAWN[s] : BPAWN[s]; }

static Bitboard slide(Square s, Bitboard occ, int df, int dr){
	Bitboard res = 0;
	int f = file_of(s), r = rank_of(s);
	for(;;){
		f += df; r += dr;
		if(!on_board(f, r))break;
		Square ns = make_sq(f, r);
		res |= bb_of(ns);
		if(occ & bb_of(ns)) break;
		}
	return res;
}

Bitboard rook(Square s, Bitboard occ){
	return slide(s, occ, +1, 0) | slide(s, occ, -1, 0) | slide(s, occ, 0, +1) | slide(s, occ, 0, -1);	
}

Bitboard bishop(Square s, Bitboard occ){
	return slide(s, occ, +1, +1) | slide(s, occ, +1, -1) | slide(s, occ, -1, +1) | slide(s, occ, -1, -1);
}
			
Bitboard queen(Square s, Bitboard occ){
	return rook(s, occ) | bishop(s, occ);
}
}
