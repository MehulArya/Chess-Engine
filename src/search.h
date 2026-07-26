#pragma once
#include "board.h"
#include "move.h"

int alpha_beta(Board& board, int depth, int alpha, int beta, int ply);
Move search_best_move(Board& board, int depth);
Move search_best_move_id(Board& board, int max_depth, int movetime_ms);