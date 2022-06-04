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

#include "evaluate.h"

namespace Eval {

// defin pawn hash table
PawnHashTable PT;

// evaluation masks
Bitboard whitePassedMasks[64];
Bitboard blackPassedMasks[64];

void init() {
  for (Square sq = Square(0); sq < Square(64); sq = Square(sq + 1)) {
    uint8_t file = file_of(sq);
    uint8_t rank = rank_of(sq);
    Bitboard fileMask = (file == FILE_A) ? MASK_FILE[FILE_A] | MASK_FILE[FILE_B] :
                        (file == FILE_H) ? MASK_FILE[FILE_H] | MASK_FILE[FILE_G] :
                        MASK_FILE[file - 1] | MASK_FILE[file] | MASK_FILE[file + 1];
    

    Bitboard whitePassedMask = 0ULL;
    for (int currRank = (int)rank + 1; currRank <= (int)RANK_8; currRank++) {
      whitePassedMask |= fileMask & MASK_RANK[currRank];
    }

    Bitboard blackPassedMask = 0ULL;
    for (int currRank = (int)rank - 1; currRank >= (int)RANK_1; currRank--) {
      blackPassedMask |= fileMask & MASK_RANK[currRank];
    }

    whitePassedMasks[sq] = whitePassedMask;
    blackPassedMasks[sq] = blackPassedMask;
  }
}

} // end of namespace eval