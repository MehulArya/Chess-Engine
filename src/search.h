#pragma once
#include "board.h"
#include "move.h"

int alpha_beta(Board& board, int depth, int alpha, int beta, int ply);
Move search_best_move(Board& board, int depth);