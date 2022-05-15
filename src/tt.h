/*
  Horowitz, a UCI compatible chess engine. 
  Copyright (C) 2022 by OliveriQ.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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