#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "tt.h"
#include <algorithm>
#include <climits>
#include <cstring>
#include <chrono>

static const int INF = 32000;

static int mvv_lva_score(Move move) {
    static const int victim_values[13] = {0, 100, 320, 330, 500, 900, 0, 100, 320, 330, 500, 900, 0};
    static const int attacker_values[13] = {0, 1, 3, 3, 5, 9, 0, 1, 3, 3, 5, 9, 0};

    if (!move.is_capture()) return 0;

    Piece captured = move.captured();
    Piece moved = move.moved();
    return victim_values[static_cast<int>(captured)] * 10 - attacker_values[static_cast<int>(moved)];
}

static void score_moves(MoveList& moves, int* scores, Move tt_move, Move killer1, Move killer2) {
    for (std::size_t i = 0; i < moves.count; ++i) {
        int score = mvv_lva_score(moves.moves[i]);
        if (moves.moves[i].v == tt_move.v) {
            score = 100000;
        } else if (moves.moves[i].v == killer1.v || moves.moves[i].v == killer2.v) {
            if (score == 0) score = 50000;
        }
        scores[i] = score;
    }
}

static int quiescence(Board& board, int alpha, int beta, int ply) {
    int stand_pat = evaluate(board);

    if (stand_pat >= beta) {
        return beta;
    }
    if (stand_pat > alpha) {
        alpha = stand_pat;
    }

    MoveList moves;
    generateLegalMoves(board, moves);

    int scores[256];
    score_moves(moves, scores, Move{0}, Move{0}, Move{0});

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

        if (!moves.moves[i].is_capture()) continue;

        Undo undo;
        undo.move = moves.moves[i];
        undo.captured = board.pieceAt(moves.moves[i].to());
        undo.castling_rights = board.castling_rights;
        undo.ep_square = board.ep_square;
        undo.halfmove_clock = board.halfmove_clock;
        undo.fullmove_number = board.fullmove_number;
        undo.zobrist_key = board.zobrist_key;

        board.makeMove(moves.moves[i]);

        int score = -quiescence(board, -beta, -alpha, ply + 1);

        board.unmakeMove(undo);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
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
        return quiescence(board, alpha, beta, ply);
    }

    TTEntry entry;
    Move tt_move{0};
    if (tt.probe(board.zobrist_key, entry)) {
        if (entry.depth >= depth) {
            if (entry.flag == TT_EXACT) {
                return entry.score;
            }
            if (entry.flag == TT_LOWER && entry.score >= beta) {
                return beta;
            }
            if (entry.flag == TT_UPPER && entry.score <= alpha) {
                return alpha;
            }
        }
        tt_move = entry.best_move;
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
    score_moves(moves, scores, tt_move, killer1, killer2);

    Move best_move_in_branch = moves.moves[0];
    int original_alpha = alpha;

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
            tt.store(board.zobrist_key, score, moves.moves[i], depth, TT_LOWER);
            return beta;
        }
        if (score > alpha) {
            alpha = score;
            best_move_in_branch = moves.moves[i];
        }
    }

    std::uint8_t flag = (alpha > original_alpha) ? TT_EXACT : TT_UPPER;
    tt.store(board.zobrist_key, alpha, best_move_in_branch, depth, flag);
    return alpha;
}

Move search_best_move(Board& board, int depth) {
    MoveList moves;
    generateLegalMoves(board, moves);

    int scores[256];
    score_moves(moves, scores, Move{0}, Move{0}, Move{0});

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

Move search_best_move_id(Board& board, int max_depth, int movetime_ms) {
    MoveList moves;
    generateLegalMoves(board, moves);

    Move best_move = moves.moves[0];
    auto start_time = std::chrono::steady_clock::now();

    for (int current_depth = 1; current_depth <= max_depth; ++current_depth) {
        int scores[256];
        score_moves(moves, scores, Move{0}, Move{0}, Move{0});

        bool search_completed = true;
        int best_score = -INF;
        Move depth_best = moves.moves[0];

        for (std::size_t i = 0; i < moves.count; ++i) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
            if (elapsed >= movetime_ms) {
                search_completed = false;
                break;
            }

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

            int score = -alpha_beta(board, current_depth - 1, -INF, INF, 1);

            board.unmakeMove(undo);

            if (score > best_score) {
                best_score = score;
                depth_best = moves.moves[i];
            }
        }

        if (search_completed) {
            best_move = depth_best;
        }
    }

    return best_move;
}