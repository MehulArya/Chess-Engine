#include "search.h"
#include "movegen.h"
#include "eval.h"
#include <algorithm>
#include <climits>

static const int INF = 32000;

int alpha_beta(Board& board, int depth, int alpha, int beta, int ply) {
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

    for (std::size_t i = 0; i < moves.count; ++i) {
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

    Move best_move = moves.moves[0];
    int best_score = -INF;

    for (std::size_t i = 0; i < moves.count; ++i) {
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