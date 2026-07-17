#include "attacks.h"
#include "zobrist.h"
#include "uci_like.h"

int main() {
    attacks::init();
    zobrist::init();
    uci_loop();
    return 0;
}
