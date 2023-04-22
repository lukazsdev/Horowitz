#pragma once

#include "chess.h"

// struct for a pawn hash table entry
struct Entry {
    uint64_t hashKey;
    int MGScores[2];
    int EGScores[2];
};

// struct for a pawn hash table
class PawnHashTable {
public:
    int hashTableEntries = 0;
    Entry *entries = NULL;

public: 
    void Init(int sizeInMB);
    void Clear();

public:
    void Store(uint64_t hashKey, int mg_scores[2], int eg_scores[2]);
    int Read(uint64_t hashKey, Color color, Phase phase);
};