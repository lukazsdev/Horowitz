#pragma once

#include "chess.h"

// hash table size (in MB)
static constexpr int HashTableSize = 800000;

// no hash entry found constant
static constexpr int NoHashEntry = 20000;

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
    TTEntry* entries;
    int size;

public: 
    void Init();
    void Clear();

public:
    void Store(uint64_t hashKey, uint8_t depth, uint8_t flag, int score, int ply);
    int Read(uint64_t hashKey, int alpha, int beta, int ply, uint8_t depth);
};