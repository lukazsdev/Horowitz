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

// class holding random numbers
// for zobrist hashing
class Zobrist {
public:
    uint32_t randomState;

    uint64_t pieceKeys[12][64];
    uint64_t enpassantKeys[64];
    uint64_t castleKeys[16];
    uint64_t sideKey;

public:
    Zobrist() : randomState(1804289383) {};
    void initRandomKeys();

private:
    uint64_t random64();
    uint32_t random32();
};

// zobrist instance
extern Zobrist zobrist;

