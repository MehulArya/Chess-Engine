#include "search.h"
#include "movegen.h"
#include "eval.h"
#include <algorithm>
#include <climits>
#include <cstring>

static const int INF = 32000;

static int mvv_lva_score(Move move) {
    static const int victim_values[13] = {0, 100, 320, 330, 500, 900, 0, 100, 320, 330, 500, 900, 0};
    static const int attacker_values[13] = {0, 1, 3, 3, 5, 9, 0, 1, 3, 3, 5, 9, 0};

    if (!move.is_capture()) return 0;

    Piece captured = move.captured();
    Piece moved = move.moved();
    return victim_values[static_cast<int>(captured)] * 10 - attacker_values[static_cast<int>(moved)];
}

static void score_moves(MoveList& moves, int* scores, Move killer1, Move killer2) {
    for (std::size_t i = 0; i < moves.count; ++i) {
        int score = mvv_lva_score(moves.moves[i]);
        if (moves.moves[i].v == killer1.v || moves.moves[i].v == killer2.v) {
            if (score == 0) score = 50000;
        }
        scores[i] = score;
    }
}

int alpha_beta(Board& board, int depth, int alpha, int beta, int ply) {
    static Move killer_moves[MAX_PLY][2];
    static bool killers_initialized = false;
    if (!killers_initialized) {
        for (int i = 0; i < MAX_PLY; ++i) {
            killer_moves[i][0] = Move{0};
            killer_moves[i][1] = Move{0};
        }
        killers_initialized = true;
    }

    if (depth == 0) {
        return evaluate(board);
    }

    MoveList moves;
    generateLegalMoves(board, moves);

    if (moves.count == 0) {
        if (board.isInCheck(board.side_to_move)) {
            return -INF + ply;
        }
        return 0;
    }

    int scores[256];
    Move killer1 = (ply < MAX_PLY) ? killer_moves[ply][0] : Move{0};
    Move killer2 = (ply < MAX_PLY) ? killer_moves[ply][1] : Move{0};
    score_moves(moves, scores, killer1, killer2);

    int best_score_in_branch = -INF;
    Move best_move_in_branch = moves.moves[0];

    for (std::size_t i = 0; i < moves.count; ++i) {
        std::size_t best_idx = i;
        int best_score = scores[i];

        for (std::size_t j = i + 1; j < moves.count; ++j) {
            if (scores[j] > best_score) {
                best_score = scores[j];
                best_idx = j;
            }
        }

        if (best_idx != i) {
            std::swap(moves.moves[i], moves.moves[best_idx]);
            std::swap(scores[i], scores[best_idx]);
        }

        Undo undo;
        undo.move = moves.moves[i];
        undo.captured = board.pieceAt(moves.moves[i].to());
        undo.castling_rights = board.castling_rights;
        undo.ep_square = board.ep_square;
        undo.halfmove_clock = board.halfmove_clock;
        undo.fullmove_number = board.fullmove_number;
        undo.zobrist_key = board.zobrist_key;

        board.makeMove(moves.moves[i]);

        int score = -alpha_beta(board, depth - 1, -beta, -alpha, ply + 1);

        board.unmakeMove(undo);

        if (score > best_score_in_branch) {
            best_score_in_branch = score;
            best_move_in_branch = moves.moves[i];
        }

        if (score >= beta) {
            if (ply < MAX_PLY) {
                Move m = moves.moves[i];
                if (mvv_lva_score(m) == 0) {
                    if (killer_moves[ply][0].v != m.v) {
                        killer_moves[ply][1] = killer_moves[ply][0];
                        killer_moves[ply][0] = m;
                    }
                }
            }
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

Move search_best_move(Board& board, int depth) {
    MoveList moves;
    generateLegalMoves(board, moves);

    int scores[256];
    score_moves(moves, scores, Move{0}, Move{0});

    Move best_move = moves.moves[0];
    int best_score = -INF;

    for (std::size_t i = 0; i < moves.count; ++i) {
        std::size_t best_idx = i;
        int best_scr = scores[i];

        for (std::size_t j = i + 1; j < moves.count; ++j) {
            if (scores[j] > best_scr) {
                best_scr = scores[j];
                best_idx = j;
            }
        }

        if (best_idx != i) {
            std::swap(moves.moves[i], moves.moves[best_idx]);
            std::swap(scores[i], scores[best_idx]);
        }

        Undo undo;
        undo.move = moves.moves[i];
        undo.captured = board.pieceAt(moves.moves[i].to());
        undo.castling_rights = board.castling_rights;
        undo.ep_square = board.ep_square;
        undo.halfmove_clock = board.halfmove_clock;
        undo.fullmove_number = board.fullmove_number;
        undo.zobrist_key = board.zobrist_key;

        board.makeMove(moves.moves[i]);

        int score = -alpha_beta(board, depth - 1, -INF, INF, 1);

        board.unmakeMove(undo);

        if (score > best_score) {
            best_score = score;
            best_move = moves.moves[i];
        }
    }

    return best_move;
}
