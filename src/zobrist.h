#pragma once
#include "types.h"
#include <cstdint>

namespace zobrist {
    extern std::uint64_t pieces[64][13];
    extern std::uint64_t side_to_move;
    extern std::uint64_t castling[16];
    extern std::uint64_t ep_file[8];

    void init();
}
