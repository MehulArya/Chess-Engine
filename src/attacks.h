#pragma once
#include<types.h>

namespace attacks;

void init();

Bitboard knight(Square s);
Bitboard king(Square s);
Bitboard pawn_attacks(Color c, Square s);

// Sliding Attacks
Bitboard rook(Square s, Bitboard occ);
Bitboard queen(Square s, Bitboard occ);
Bitboard bishop(Square s, Bitboard occ);

}
