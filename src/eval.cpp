#include "eval.h"
#include "attacks.h"
#include <cstdint>

static const int PAWN_VALUE = 100;
static const int KNIGHT_VALUE = 320;
static const int BISHOP_VALUE = 330;
static const int ROOK_VALUE = 500;
static const int QUEEN_VALUE = 900;

static const int PST[6][64] = {
    {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    },
    {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    },
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    },
    {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    }
};

static int piece_value(Piece p) {
    switch (p) {
        case Piece::WP: case Piece::BP: return PAWN_VALUE;
        case Piece::WN: case Piece::BN: return KNIGHT_VALUE;
        case Piece::WB: case Piece::BB: return BISHOP_VALUE;
        case Piece::WR: case Piece::BR: return ROOK_VALUE;
        case Piece::WQ: case Piece::BQ: return QUEEN_VALUE;
        default: return 0;
    }
}

static int pst_index(Piece p) {
    switch (p) {
        case Piece::WP: case Piece::BP: return 0;
        case Piece::WN: case Piece::BN: return 1;
        case Piece::WB: case Piece::BB: return 2;
        case Piece::WR: case Piece::BR: return 3;
        case Piece::WQ: case Piece::BQ: return 4;
        case Piece::WK: case Piece::BK: return 5;
        default: return 0;
    }
}

static bool is_white_piece(Piece p) {
    return p >= Piece::WP && p <= Piece::WK;
}

static int pawn_structure_score(const Board& board) {
    int score = 0;

    for (int c = 0; c < 2; ++c) {
        Color color = (c == 0) ? Color::White : Color::Black;
        Piece pawn = (color == Color::White) ? Piece::WP : Piece::BP;
        int sign = (color == Color::White) ? 1 : -1;

        int pawn_counts[8] = {0};
        int pawn_files[8] = {0};

        for (int sq = 0; sq < 64; ++sq) {
            if (board.pieceAt(Square(sq)) == pawn) {
                int f = file_of(Square(sq));
                pawn_counts[f]++;
                pawn_files[f] = 1;
            }
        }

        for (int f = 0; f < 8; ++f) {
            if (pawn_counts[f] > 1) {
                score -= 15 * sign * (pawn_counts[f] - 1);
            }
        }

        for (int f = 0; f < 8; ++f) {
            if (pawn_counts[f] == 0) continue;
            int left = (f > 0) ? pawn_files[f - 1] : 0;
            int right = (f < 7) ? pawn_files[f + 1] : 0;
            if (left == 0 && right == 0) {
                score -= 20 * sign;
            }
        }

        for (int sq = 0; sq < 64; ++sq) {
            if (board.pieceAt(Square(sq)) != pawn) continue;
            int f = file_of(Square(sq));
            int r = rank_of(Square(sq));

            bool blocked = false;
            int dir = (color == Color::White) ? 1 : -1;
            for (int tr = r + dir; tr >= 0 && tr < 8; tr += dir) {
                int tsq = make_sq(f, tr);
                Piece other_pawn = (color == Color::White) ? Piece::BP : Piece::WP;
                if (board.pieceAt(tsq) == other_pawn) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                int advance = (color == Color::White) ? r : (7 - r);
                int bonus = advance * advance;
                if (bonus > 0) score += bonus * sign;
            }
        }
    }

    return score;
}

static int king_safety_score(const Board& board) {
    int score = 0;

    for (int c = 0; c < 2; ++c) {
        Color color = (c == 0) ? Color::White : Color::Black;
        Piece king = (color == Color::White) ? Piece::WK : Piece::BK;
        Piece pawn = (color == Color::White) ? Piece::WP : Piece::BP;
        int sign = (color == Color::White) ? 1 : -1;

        int king_sq = -1;
        for (int sq = 0; sq < 64; ++sq) {
            if (board.pieceAt(Square(sq)) == king) {
                king_sq = sq;
                break;
            }
        }
        if (king_sq < 0) continue;

        int kf = file_of(Square(king_sq));
        int kr = rank_of(Square(king_sq));

        if (kr != 0 && kr != 7) continue;

        int dir = (color == Color::White) ? 1 : -1;

        int shield_files[3];
        if (kf == 6 || kf == 7) {
            shield_files[0] = 5;
            shield_files[1] = 6;
            shield_files[2] = 7;
        } else if (kf == 0 || kf == 1) {
            shield_files[0] = 0;
            shield_files[1] = 1;
            shield_files[2] = 2;
        } else {
            shield_files[0] = kf - 1;
            shield_files[1] = kf;
            shield_files[2] = kf + 1;
        }

        int shield_count = 0;
        for (int sf = 0; sf < 3; ++sf) {
            if (shield_files[sf] < 0 || shield_files[sf] > 7) continue;
            for (int delta = 1; delta <= 3; ++delta) {
                int tr = kr + dir * delta;
                if (tr < 0 || tr > 7) continue;
                Square tsq = make_sq(shield_files[sf], tr);
                if (board.pieceAt(tsq) == pawn) {
                    shield_count++;
                }
            }
        }

        if (shield_count < 3) {
            score -= (3 - shield_count) * 15 * sign;
        }
    }

    return score;
}

static int mobility_score(const Board& board) {
    int score = 0;

    Bitboard occ = 0;
    for (int sq = 0; sq < 64; ++sq) {
        if (board.pieceAt(Square(sq)) != Piece::Empty) {
            occ |= bb_of(Square(sq));
        }
    }

    for (int sq = 0; sq < 64; ++sq) {
        Piece p = board.pieceAt(Square(sq));
        if (p == Piece::Empty) continue;

        int sign = is_white_piece(p) ? 1 : -1;
        Bitboard targets = 0;

        switch (p) {
            case Piece::WN: case Piece::BN:
                targets = attacks::knight(Square(sq));
                break;
            case Piece::WB: case Piece::BB:
                targets = attacks::bishop(Square(sq), occ);
                break;
            case Piece::WR: case Piece::BR:
                targets = attacks::rook(Square(sq), occ);
                break;
            case Piece::WQ: case Piece::BQ:
                targets = attacks::queen(Square(sq), occ);
                break;
            default:
                break;
        }

        int count = 0;
        Bitboard t = targets;
        while (t) {
            int tsq = __builtin_ctzll(t);
            Piece tp = board.pieceAt(Square(tsq));
            if ((sign == 1 && !(tp >= Piece::WP && tp <= Piece::WK)) &&
                (sign == -1 && !(tp >= Piece::BP && tp <= Piece::BK))) {
                count++;
            }
            t &= t - 1;
        }

        if (count > 0) {
            score += count * sign;
        }
    }

    return score;
}

static int bishop_pair_score(const Board& board) {
    int score = 0;
    int wbishops = 0;
    int bbishops = 0;

    for (int sq = 0; sq < 64; ++sq) {
        Piece p = board.pieceAt(Square(sq));
        if (p == Piece::WB) wbishops++;
        if (p == Piece::BB) bbishops++;
    }

    if (wbishops >= 2) score += 30;
    if (bbishops >= 2) score -= 30;

    return score;
}

int evaluate(const Board& board) {
    int score = 0;

    for (int sq = 0; sq < 64; ++sq) {
        Piece p = board.pieceAt(Square(sq));
        if (p == Piece::Empty) continue;

        int val = piece_value(p);
        int idx = pst_index(p);
        int pst_val = PST[idx][sq];

        if (is_white_piece(p)) {
            score += val + pst_val;
        } else {
            score -= val + pst_val;
        }
    }

    score += pawn_structure_score(board);
    score += king_safety_score(board);
    score += mobility_score(board);
    score += bishop_pair_score(board);

    return score;
}