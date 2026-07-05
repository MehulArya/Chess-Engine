#include "movegen.h"
#include "attacks.h"

namespace {

inline bool isWhitePiece(Piece p) {
    return p >= Piece::WP && p <= Piece::WK;
}

inline bool isBlackPiece(Piece p) {
    return p >= Piece::BP && p <= Piece::BK;
}

inline bool isOwnPiece(Piece p, Color side) {
    return side == Color::White ? isWhitePiece(p) : isBlackPiece(p);
}

inline bool isEnemyPiece(Piece p, Color side) {
    return p != Piece::Empty && !isOwnPiece(p, side);
}

inline PieceType pawnPromotionType(int i) {
    switch (i) {
        case 0: return PieceType::Queen;
        case 1: return PieceType::Rook;
        case 2: return PieceType::Bishop;
        default: return PieceType::Knight;
    }
}

void addPromotionMoves(MoveList& out, Square from, Square to, Piece moved, Piece captured, MoveFlag flag) {
    for (int i = 0; i < 4; ++i) {
        out.add(Move::make(from, to, moved, captured, pawnPromotionType(i), flag));
    }
}

void generatePawnMoves(const Board& board, MoveList& out, Square sq, Piece pawn) {
    const Color side = board.side_to_move;
    const int rank = rank_of(sq);
    const int file = file_of(sq);
    const int dir = (side == Color::White) ? 8 : -8;
    const int start_rank = (side == Color::White) ? 1 : 6;
    const int promo_rank = (side == Color::White) ? 6 : 1;
    const int one_step_rank = rank + ((side == Color::White) ? 1 : -1);

    if (one_step_rank >= 0 && one_step_rank < 8) {
        Square one = Square(sq + dir);
        if (board.pieceAt(one) == Piece::Empty) {
            if (rank == promo_rank) {
                addPromotionMoves(out, sq, one, pawn, Piece::Empty, MoveFlag::Promotion);
            } else {
                out.add(Move::make(sq, one, pawn, Piece::Empty, PieceType::None, MoveFlag::Quiet));

                if (rank == start_rank) {
                    Square two = Square(sq + 2 * dir);
                    if (board.pieceAt(two) == Piece::Empty) {
                        out.add(Move::make(sq, two, pawn, Piece::Empty, PieceType::None, MoveFlag::DoublePawnPush));
                    }
                }
            }
        }
    }

    const int capture_files[2] = {file - 1, file + 1};
    const int capture_dirs[2] = {(side == Color::White) ? 7 : -9, (side == Color::White) ? 9 : -7};

    for (int i = 0; i < 2; ++i) {
        const int cf = capture_files[i];
        if (cf < 0 || cf > 7) continue;

        const int tr = rank + ((side == Color::White) ? 1 : -1);
        if (tr < 0 || tr > 7) continue;

        Square to = Square(sq + capture_dirs[i]);
        Piece target = board.pieceAt(to);

        if (to == board.ep_square) {
            out.add(Move::make(sq, to, pawn, side == Color::White ? Piece::BP : Piece::WP, PieceType::None, MoveFlag::EnPassant));
            continue;
        }

        if (isEnemyPiece(target, side)) {
            if (rank == promo_rank) {
                addPromotionMoves(out, sq, to, pawn, target, MoveFlag::PromoCapture);
            } else {
                out.add(Move::make(sq, to, pawn, target, PieceType::None, MoveFlag::Capture));
            }
        }
    }
}

void generateKnightMoves(const Board& board, MoveList& out, Square sq, Piece knight) {
    Bitboard targets = attacks::knight(sq);
    for (int to = 0; to < 64; ++to) {
        if (!(targets & bb_of(Square(to)))) continue;
        Piece target = board.pieceAt(Square(to));
        if (isOwnPiece(target, board.side_to_move)) continue;
        out.add(Move::make(sq, Square(to), knight, target, PieceType::None,
                           target == Piece::Empty ? MoveFlag::Quiet : MoveFlag::Capture));
    }
}

void generateSlidingMoves(const Board& board, MoveList& out, Square sq, Piece piece, Bitboard attacks_bb) {
    for (int to = 0; to < 64; ++to) {
        if (!(attacks_bb & bb_of(Square(to)))) continue;
        Piece target = board.pieceAt(Square(to));
        if (isOwnPiece(target, board.side_to_move)) continue;
        out.add(Move::make(sq, Square(to), piece, target, PieceType::None,
                           target == Piece::Empty ? MoveFlag::Quiet : MoveFlag::Capture));
    }
}

void generateKingMoves(const Board& board, MoveList& out, Square sq, Piece king) {
    Bitboard targets = attacks::king(sq);
    for (int to = 0; to < 64; ++to) {
        if (!(targets & bb_of(Square(to)))) continue;
        Piece target = board.pieceAt(Square(to));
        if (isOwnPiece(target, board.side_to_move)) continue;
        out.add(Move::make(sq, Square(to), king, target, PieceType::None,
                           target == Piece::Empty ? MoveFlag::Quiet : MoveFlag::Capture));
    }

    if (board.side_to_move == Color::White && sq == 4 && king == Piece::WK) {
        if ((board.castling_rights & static_cast<std::uint8_t>(Castling::WK)) &&
            board.pieceAt(5) == Piece::Empty && board.pieceAt(6) == Piece::Empty &&
            !board.isInCheck(Color::White) &&
            !board.isAttacked(5, Color::Black) &&
            !board.isAttacked(6, Color::Black) &&
            board.pieceAt(7) == Piece::WR) {
            out.add(Move::make(4, 6, Piece::WK, Piece::Empty, PieceType::None, MoveFlag::KingCastle));
        }
        if ((board.castling_rights & static_cast<std::uint8_t>(Castling::WQ)) &&
            board.pieceAt(1) == Piece::Empty && board.pieceAt(2) == Piece::Empty && board.pieceAt(3) == Piece::Empty &&
            !board.isInCheck(Color::White) &&
            !board.isAttacked(3, Color::Black) &&
            !board.isAttacked(2, Color::Black) &&
            board.pieceAt(0) == Piece::WR) {
            out.add(Move::make(4, 2, Piece::WK, Piece::Empty, PieceType::None, MoveFlag::QueenCastle));
        }
    }

    if (board.side_to_move == Color::Black && sq == 60 && king == Piece::BK) {
        if ((board.castling_rights & static_cast<std::uint8_t>(Castling::BK)) &&
            board.pieceAt(61) == Piece::Empty && board.pieceAt(62) == Piece::Empty &&
            !board.isInCheck(Color::Black) &&
            !board.isAttacked(61, Color::White) &&
            !board.isAttacked(62, Color::White) &&
            board.pieceAt(63) == Piece::BR) {
            out.add(Move::make(60, 62, Piece::BK, Piece::Empty, PieceType::None, MoveFlag::KingCastle));
        }
        if ((board.castling_rights & static_cast<std::uint8_t>(Castling::BQ)) &&
            board.pieceAt(57) == Piece::Empty && board.pieceAt(58) == Piece::Empty && board.pieceAt(59) == Piece::Empty &&
            !board.isInCheck(Color::Black) &&
            !board.isAttacked(59, Color::White) &&
            !board.isAttacked(58, Color::White) &&
            board.pieceAt(56) == Piece::BR) {
            out.add(Move::make(60, 58, Piece::BK, Piece::Empty, PieceType::None, MoveFlag::QueenCastle));
        }
    }
}

} // namespace

void generatePseudoLegalMoves(const Board& board, MoveList& out) {
    out.clear();

    for (int sq = 0; sq < 64; ++sq) {
        Piece piece = board.pieceAt(Square(sq));
        if (piece == Piece::Empty) continue;
        if (board.side_to_move == Color::White && !isWhitePiece(piece)) continue;
        if (board.side_to_move == Color::Black && !isBlackPiece(piece)) continue;

        switch (piece) {
            case Piece::WP:
            case Piece::BP:
                generatePawnMoves(board, out, Square(sq), piece);
                break;
            case Piece::WN:
            case Piece::BN:
                generateKnightMoves(board, out, Square(sq), piece);
                break;
            case Piece::WB:
            case Piece::BB: {
                Bitboard occ = 0;
                for (int i = 0; i < 64; ++i) {
                    if (board.pieceAt(Square(i)) != Piece::Empty) occ |= bb_of(Square(i));
                }
                generateSlidingMoves(board, out, Square(sq), piece, attacks::bishop(Square(sq), occ));
                break;
            }
            case Piece::WR:
            case Piece::BR: {
                Bitboard occ = 0;
                for (int i = 0; i < 64; ++i) {
                    if (board.pieceAt(Square(i)) != Piece::Empty) occ |= bb_of(Square(i));
                }
                generateSlidingMoves(board, out, Square(sq), piece, attacks::rook(Square(sq), occ));
                break;
            }
            case Piece::WQ:
            case Piece::BQ: {
                Bitboard occ = 0;
                for (int i = 0; i < 64; ++i) {
                    if (board.pieceAt(Square(i)) != Piece::Empty) occ |= bb_of(Square(i));
                }
                generateSlidingMoves(board, out, Square(sq), piece, attacks::queen(Square(sq), occ));
                break;
            }
            case Piece::WK:
            case Piece::BK:
                generateKingMoves(board, out, Square(sq), piece);
                break;
            default:
                break;
        }
    }
}

void generateLegalMoves(const Board& board, MoveList& out) {
    MoveList pseudo;
    generatePseudoLegalMoves(board, pseudo);

    out.clear();
    for (std::size_t i = 0; i < pseudo.count; ++i) {
        Board next = board;
        next.makeMove(pseudo.moves[i]);
        if (!next.isInCheck(!next.side_to_move)) {
            out.add(pseudo.moves[i]);
        }
    }
}
