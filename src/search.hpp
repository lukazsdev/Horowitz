#pragma once

#include "chess.hpp"

// search constants
static constexpr int maxPly = 64;
static constexpr int infinity = 10000;
static constexpr int checkmate = 9000;

class Search {
public:
    uint64_t nodes;
    int ply;

    void search(Position pos, int depth);
    int quiescence(Position pos, int alpha, int beta);
    int negamax(Position pos, int alpha, int beta, int depth);
};