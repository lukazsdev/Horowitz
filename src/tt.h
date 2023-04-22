#pragma once

#include "chess.h"

// 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1 (test position)

// transposition table hash flags
static constexpr int HashFlagExact = 0;
static constexpr int HashFlagAlpha = 1;
static constexpr int HashFlagBeta  = 2;

// struct for a transposition table entry
struct TTEntry {
    uint64_t hashKey;
    uint64_t score;
    uint8_t depth;
    uint8_t flag;
};

// struct for a transposition table
class TranspositionTable {
public:
    int hashTableEntries = 0;
    TTEntry *entries = NULL;

public: 
    void Init(int sizeInMB);
    void Clear();

public:
    void Store(uint64_t hashKey, uint8_t depth, uint8_t flag, int score, int ply);
    int Read(uint64_t hashKey, int alpha, int beta, int ply, uint8_t depth);
};