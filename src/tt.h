#pragma once

#include "chess.h"

// no hash entry found constant
static constexpr int NoHashEntry = 100000;

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