#include "uci_like.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include <iostream>
#include <sstream>
#include <string>

void uci_loop() {
    std::string line;
    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci") {
            std::cout << "id name ClassicChess" << std::endl;
            std::cout << "id author Chess-Engine" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (token == "ucinewgame") {
            board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else if (token == "position") {
            std::string pos_type;
            iss >> pos_type;
            if (pos_type == "startpos") {
                board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else if (pos_type == "fen") {
                std::string fen_part;
                std::string fen;
                for (int i = 0; i < 6; ++i) {
                    if (iss >> fen_part) {
                        fen += fen_part;
                        if (i < 5) fen += " ";
                    }
                }
                if (!fen.empty()) board.setFen(fen);
            }

            std::string moves_token;
            if (iss >> moves_token && moves_token == "moves") {
                std::string move_str;
                while (iss >> move_str) {
                    Square from, to;
                    PieceType promo;
                    if (parse_uci_move(move_str, from, to, promo)) {
                        Piece moved = board.pieceAt(from);
                        MoveFlag flag = MoveFlag::Quiet;
                        Piece captured = board.pieceAt(to);

                        if (promo != PieceType::None) {
                            flag = (captured != Piece::Empty) ? MoveFlag::PromoCapture : MoveFlag::Promotion;
                        } else if (moved == Piece::WK && from == 4 && to == 6) {
                            flag = MoveFlag::KingCastle;
                        } else if (moved == Piece::WK && from == 4 && to == 2) {
                            flag = MoveFlag::QueenCastle;
                        } else if (moved == Piece::BK && from == 60 && to == 62) {
                            flag = MoveFlag::KingCastle;
                        } else if (moved == Piece::BK && from == 60 && to == 58) {
                            flag = MoveFlag::QueenCastle;
                        } else if (moved == Piece::WP && from + 16 == to && rank_of(from) == 1) {
                            flag = MoveFlag::DoublePawnPush;
                        } else if (moved == Piece::BP && from - 16 == to && rank_of(from) == 6) {
                            flag = MoveFlag::DoublePawnPush;
                        } else if (to == board.ep_square && (moved == Piece::WP || moved == Piece::BP)) {
                            flag = MoveFlag::EnPassant;
                        } else if (captured != Piece::Empty) {
                            flag = MoveFlag::Capture;
                        }

                        board.makeMove(Move::make(from, to, moved, captured, promo, flag));
                    }
                }
            }
        } else if (token == "go") {
            int depth = 64;
            int movetime = 5000;
            std::string param;
            while (iss >> param) {
                if (param == "depth") {
                    iss >> depth;
                } else if (param == "movetime") {
                    iss >> movetime;
                }
            }

            Move best = search_best_move_id(board, depth, movetime);
            std::cout << "bestmove " << move_to_uci(best) << std::endl;
        } else if (token == "quit") {
            break;
        }
    }
}