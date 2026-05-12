#include "board.h"
#include "attacks.h"
#include "zobrist.h"
#include <iostream>

int main() {
    attacks::init();
    zobrist::init();

    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "Board initialized successfully!" << std::endl;
    std::cout << "FEN: " << b.getFen() << std::endl;

    return 0;
}
