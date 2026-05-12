#include "board.h"
#include "attacks.h"
#include "zobrist.h"
#include <sstream>
#include <cctype>

namespace zobrist {
    std::uint64_t pieces[64][13];
    std::uint64_t side_to_move;
    std::uint64_t castling[16];
    std::uint64_t ep_file[8];

    void init() {
        std::uint64_t seed = 0x123456789ABCDEF0ULL;
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 13; j++) {
                pieces[i][j] = seed = (seed * 6364136223846793005ULL) ^ 0x9E3779B97F4A7C15ULL;
            }
        }
        side_to_move = seed = (seed * 6364136223846793005ULL) ^ 0x9E3779B97F4A7C15ULL;
        for (int i = 0; i < 16; i++) {
            castling[i] = seed = (seed * 6364136223846793005ULL) ^ 0x9E3779B97F4A7C15ULL;
        }
        for (int i = 0; i < 8; i++) {
            ep_file[i] = seed = (seed * 6364136223846793005ULL) ^ 0x9E3779B97F4A7C15ULL;
        }
    }
}

char file_to_char(int f) { return char('a' + f); }
char rank_to_char(int r) { return char('1' + r); }

void Board::setFen(const std::string& fen) {
    squares.fill(Piece::Empty);
    ep_square = Square(255);
    castling_rights = 0;
    halfmove_clock = 0;
    fullmove_number = 1;
    zobrist_key = 0;

    std::istringstream iss(fen);
    std::string board_part, side_part, castling_part, ep_part;

    iss >> board_part >> side_part >> castling_part >> ep_part;

    int sq = 56;
    for (char c : board_part) {
        if (c == '/') {
            sq -= 16;
        } else if (std::isdigit(c)) {
            sq += (c - '0');
        } else {
            Piece p;
            bool is_white = std::isupper(c);
            char pc = std::tolower(c);

            switch (pc) {
                case 'p': p = is_white ? Piece::WP : Piece::BP; break;
                case 'n': p = is_white ? Piece::WN : Piece::BN; break;
                case 'b': p = is_white ? Piece::WB : Piece::BB; break;
                case 'r': p = is_white ? Piece::WR : Piece::BR; break;
                case 'q': p = is_white ? Piece::WQ : Piece::BQ; break;
                case 'k': p = is_white ? Piece::WK : Piece::BK; break;
                default: continue;
            }
            squares[sq] = p;
            sq++;
        }
    }

    side_to_move = (side_part == "w") ? Color::White : Color::Black;

    for (char c : castling_part) {
        switch (c) {
            case 'K': castling_rights |= static_cast<std::uint8_t>(Castling::WK); break;
            case 'Q': castling_rights |= static_cast<std::uint8_t>(Castling::WQ); break;
            case 'k': castling_rights |= static_cast<std::uint8_t>(Castling::BK); break;
            case 'q': castling_rights |= static_cast<std::uint8_t>(Castling::BQ); break;
        }
    }

    if (ep_part != "-") {
        int file = ep_part[0] - 'a';
        int rank = ep_part[1] - '1';
        ep_square = make_sq(file, rank);
    }

    updateZobrist();
}

std::string Board::getFen() const {
    std::string fen;

    for (int rank = 7; rank >= 0; rank--) {
        int empty = 0;
        for (int file = 0; file < 8; file++) {
            Square sq = make_sq(file, rank);
            Piece p = squares[sq];

            if (p == Piece::Empty) {
                empty++;
            } else {
                if (empty > 0) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                char c;
                bool is_white = (p == Piece::WP || p == Piece::WN || p == Piece::WB ||
                                 p == Piece::WR || p == Piece::WQ || p == Piece::WK);
                switch (p) {
                    case Piece::WP: case Piece::BP: c = 'p'; break;
                    case Piece::WN: case Piece::BN: c = 'n'; break;
                    case Piece::WB: case Piece::BB: c = 'b'; break;
                    case Piece::WR: case Piece::BR: c = 'r'; break;
                    case Piece::WQ: case Piece::BQ: c = 'q'; break;
                    case Piece::WK: case Piece::BK: c = 'k'; break;
                    default: c = '?';
                }
                fen += is_white ? std::toupper(c) : c;
            }
        }
        if (empty > 0) fen += std::to_string(empty);
        if (rank > 0) fen += '/';
    }

    fen += ' ';
    fen += (side_to_move == Color::White) ? 'w' : 'b';

    fen += ' ';
    if (castling_rights == 0) {
        fen += '-';
    } else {
        if (castling_rights & static_cast<std::uint8_t>(Castling::WK)) fen += 'K';
        if (castling_rights & static_cast<std::uint8_t>(Castling::WQ)) fen += 'Q';
        if (castling_rights & static_cast<std::uint8_t>(Castling::BK)) fen += 'k';
        if (castling_rights & static_cast<std::uint8_t>(Castling::BQ)) fen += 'q';
    }

    fen += ' ';
    if (ep_square == 255) {
        fen += '-';
    } else {
        fen += file_to_char(file_of(ep_square));
        fen += rank_to_char(rank_of(ep_square));
    }

    fen += ' ';
    fen += std::to_string(halfmove_clock);
    fen += ' ';
    fen += std::to_string(fullmove_number);

    return fen;
}

void Board::makeMove(Move move) {
    Square from = move.from();
    Square to = move.to();
    Piece moved = move.moved();
    MoveFlag flag = move.flag();
    PieceType promo = move.promo();

    Piece captured = squares[to];
    if (flag == MoveFlag::EnPassant) {
        captured = (side_to_move == Color::White) ? Piece::BP : Piece::WP;
        Square cap_sq = to + ((side_to_move == Color::White) ? -8 : +8);
        squares[cap_sq] = Piece::Empty;
    }

    if (moved == Piece::WP || moved == Piece::BP || captured != Piece::Empty) {
        halfmove_clock = 0;
    } else {
        halfmove_clock++;
    }

    if (moved == Piece::WK) castling_rights &= ~(static_cast<std::uint8_t>(Castling::WK) | static_cast<std::uint8_t>(Castling::WQ));
    if (moved == Piece::BK) castling_rights &= ~(static_cast<std::uint8_t>(Castling::BK) | static_cast<std::uint8_t>(Castling::BQ));
    if (moved == Piece::WR && from == 63) castling_rights &= ~static_cast<std::uint8_t>(Castling::WK);
    if (moved == Piece::WR && from == 56) castling_rights &= ~static_cast<std::uint8_t>(Castling::WQ);
    if (moved == Piece::BR && from == 7)  castling_rights &= ~static_cast<std::uint8_t>(Castling::BK);
    if (moved == Piece::BR && from == 0)  castling_rights &= ~static_cast<std::uint8_t>(Castling::BQ);
    if (captured == Piece::WR && to == 63) castling_rights &= ~static_cast<std::uint8_t>(Castling::WK);
    if (captured == Piece::WR && to == 56) castling_rights &= ~static_cast<std::uint8_t>(Castling::WQ);
    if (captured == Piece::BR && to == 7)  castling_rights &= ~static_cast<std::uint8_t>(Castling::BK);
    if (captured == Piece::BR && to == 0)  castling_rights &= ~static_cast<std::uint8_t>(Castling::BQ);

    ep_square = Square(255);
    if (flag == MoveFlag::DoublePawnPush) {
        int dir = (side_to_move == Color::White) ? +8 : -8;
        ep_square = to + dir;
    }

    if (flag == MoveFlag::KingCastle) {
        if (side_to_move == Color::White) {
            squares[61] = Piece::WK;
            squares[63] = Piece::Empty;
            squares[60] = Piece::Empty;
            squares[62] = Piece::WR;
        } else {
            squares[5] = Piece::BK;
            squares[7] = Piece::Empty;
            squares[4] = Piece::Empty;
            squares[6] = Piece::BR;
        }
    } else if (flag == MoveFlag::QueenCastle) {
        if (side_to_move == Color::White) {
            squares[58] = Piece::WK;
            squares[56] = Piece::Empty;
            squares[59] = Piece::WR;
            squares[60] = Piece::Empty;
        } else {
            squares[2] = Piece::BK;
            squares[0] = Piece::Empty;
            squares[3] = Piece::BR;
            squares[4] = Piece::Empty;
        }
    } else {
        squares[to] = moved;
        squares[from] = Piece::Empty;

        if (flag == MoveFlag::Promotion || flag == MoveFlag::PromoCapture) {
            Piece promo_piece;
            if (side_to_move == Color::White) {
                promo_piece = (promo == PieceType::Queen) ? Piece::WQ :
                              (promo == PieceType::Rook) ? Piece::WR :
                              (promo == PieceType::Bishop) ? Piece::WB : Piece::WN;
            } else {
                promo_piece = (promo == PieceType::Queen) ? Piece::BQ :
                              (promo == PieceType::Rook) ? Piece::BR :
                              (promo == PieceType::Bishop) ? Piece::BB : Piece::BN;
            }
            squares[to] = promo_piece;
        }
    }

    if (side_to_move == Color::Black) {
        fullmove_number++;
    }

    side_to_move = !side_to_move;

    updateZobrist();
}

void Board::unmakeMove(const Undo& undo) {
    Square from = undo.move.from();
    Square to = undo.move.to();
    MoveFlag flag = undo.move.flag();
    PieceType promo = undo.move.promo();

    side_to_move = !side_to_move;

    castling_rights = undo.castling_rights;
    halfmove_clock = undo.halfmove_clock;
    fullmove_number = undo.fullmove_number;
    ep_square = undo.ep_square;
    zobrist_key = undo.zobrist_key;

    if (flag == MoveFlag::KingCastle) {
        if (side_to_move == Color::White) {
            squares[60] = Piece::WK;
            squares[62] = Piece::Empty;
            squares[63] = Piece::WR;
            squares[61] = Piece::Empty;
        } else {
            squares[4] = Piece::BK;
            squares[6] = Piece::Empty;
            squares[7] = Piece::BR;
            squares[5] = Piece::Empty;
        }
    } else if (flag == MoveFlag::QueenCastle) {
        if (side_to_move == Color::White) {
            squares[60] = Piece::WK;
            squares[59] = Piece::Empty;
            squares[56] = Piece::WR;
            squares[58] = Piece::Empty;
        } else {
            squares[4] = Piece::BK;
            squares[3] = Piece::Empty;
            squares[0] = Piece::BR;
            squares[2] = Piece::Empty;
        }
    } else {
        if (flag == MoveFlag::Promotion || flag == MoveFlag::PromoCapture) {
            Piece original_pawn = (side_to_move == Color::White) ? Piece::WP : Piece::BP;
            squares[from] = original_pawn;
            squares[to] = undo.captured;
        } else {
            squares[from] = undo.move.moved();
            squares[to] = undo.captured;
        }

        if (flag == MoveFlag::EnPassant) {
            Square cap_sq = to + ((side_to_move == Color::White) ? -8 : +8);
            Piece cap_pawn = (side_to_move == Color::White) ? Piece::BP : Piece::WP;
            squares[cap_sq] = cap_pawn;
        }
    }
}

bool Board::isInCheck(Color c) const {
    Square king_sq = 0;
    Piece king_piece = (c == Color::White) ? Piece::WK : Piece::BK;

    for (int sq = 0; sq < 64; sq++) {
        if (squares[sq] == king_piece) {
            king_sq = Square(sq);
            break;
        }
    }

    return isAttacked(king_sq, !c);
}

bool Board::isAttacked(Square s, Color by) const {
    Bitboard knight_bb = attacks::knight(s);
    Piece knight = (by == Color::White) ? Piece::WN : Piece::BN;
    for (int i = 0; i < 64; i++) {
        if ((knight_bb & bb_of(Square(i))) && squares[i] == knight) return true;
    }

    Bitboard king_bb = attacks::king(s);
    Piece king = (by == Color::White) ? Piece::WK : Piece::BK;
    for (int i = 0; i < 64; i++) {
        if ((king_bb & bb_of(Square(i))) && squares[i] == king) return true;
    }

    Bitboard pawn_bb = attacks::pawn_attacks(by, s);
    Piece pawn = (by == Color::White) ? Piece::WP : Piece::BP;
    for (int i = 0; i < 64; i++) {
        if ((pawn_bb & bb_of(Square(i))) && squares[i] == pawn) return true;
    }

    Bitboard occ = 0;
    for (int i = 0; i < 64; i++) {
        if (squares[i] != Piece::Empty) occ |= bb_of(Square(i));
    }

    Bitboard rook_bb = attacks::rook(s, occ);
    Piece rook = (by == Color::White) ? Piece::WR : Piece::BR;
    Piece queen = (by == Color::White) ? Piece::WQ : Piece::BQ;
    for (int i = 0; i < 64; i++) {
        if ((rook_bb & bb_of(Square(i))) && (squares[i] == rook || squares[i] == queen)) return true;
    }

    Bitboard bishop_bb = attacks::bishop(s, occ);
    Piece bishop = (by == Color::White) ? Piece::WB : Piece::BB;
    for (int i = 0; i < 64; i++) {
        if ((bishop_bb & bb_of(Square(i))) && (squares[i] == bishop || squares[i] == queen)) return true;
    }

    return false;
}

void Board::updateZobrist() {
    zobrist_key = 0;

    for (int sq = 0; sq < 64; sq++) {
        if (squares[sq] != Piece::Empty) {
            zobrist_key ^= zobrist::pieces[sq][static_cast<int>(squares[sq])];
        }
    }

    if (side_to_move == Color::Black) {
        zobrist_key ^= zobrist::side_to_move;
    }

    zobrist_key ^= zobrist::castling[castling_rights];

    if (ep_square != 255) {
        zobrist_key ^= zobrist::ep_file[file_of(ep_square)];
    }
}
