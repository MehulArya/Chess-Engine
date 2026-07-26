#include "tt.h"
#include <cstring>
#include <new>

TranspositionTable tt;

TranspositionTable::TranspositionTable() {
    table = new TTEntry[TABLE_SIZE];
    clear();
}

TranspositionTable::~TranspositionTable() {
    delete[] table;
}

void TranspositionTable::clear() {
    for (std::size_t i = 0; i < TABLE_SIZE; ++i) {
        table[i].key = 0;
        table[i].score = 0;
        table[i].best_move = Move{0};
        table[i].depth = -1;
        table[i].flag = 0;
    }
}

void TranspositionTable::store(std::uint64_t key, int score, Move best_move, int depth, std::uint8_t flag) {
    std::size_t idx = key & (TABLE_SIZE - 1);
    if (depth >= table[idx].depth) {
        table[idx].key = key;
        table[idx].score = score;
        table[idx].best_move = best_move;
        table[idx].depth = depth;
        table[idx].flag = flag;
    }
}

bool TranspositionTable::probe(std::uint64_t key, TTEntry& entry) const {
    std::size_t idx = key & (TABLE_SIZE - 1);
    if (table[idx].key == key) {
        entry = table[idx];
        return true;
    }
    return false;
}

void TranspositionTable::prefetch(std::uint64_t key) const {
    std::size_t idx = key & (TABLE_SIZE - 1);
    __builtin_prefetch(&table[idx]);
}