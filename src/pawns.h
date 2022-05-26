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

// struct for a transposition table entry
struct Entry {
    uint64_t hashKey;
    uint64_t score;
};

// struct for a transposition table
class PawnHashTable {
public:
    int hashTableEntries = 0;
    Entry *entries = NULL;

public: 
    void Init(int sizeInMB);
    void Clear();

public:
    void Store(uint64_t hashKey, int score);
    int Read(uint64_t hashKey);
};