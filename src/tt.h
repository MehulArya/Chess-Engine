#pragma once
#include "types.h"
#include "move.h"
#include <cstdint>

struct TTEntry {
    std::uint64_t key{0};
    int score{0};
    Move best_move{0};
    int depth{-1};
    std::uint8_t flag{0};
};

enum TTFlag : std::uint8_t {
    TT_EXACT = 0,
    TT_LOWER = 1,
    TT_UPPER = 2
};

class TranspositionTable {
public:
    TranspositionTable();
    ~TranspositionTable();
    void clear();
    void store(std::uint64_t key, int score, Move best_move, int depth, std::uint8_t flag);
    bool probe(std::uint64_t key, TTEntry& entry) const;
    void prefetch(std::uint64_t key) const;

private:
    static const std::size_t TABLE_SIZE = 1 << 20;
    TTEntry* table;
};

extern TranspositionTable tt;