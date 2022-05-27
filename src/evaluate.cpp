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
	
// mop up evaluation functions
float endgamePhaseWeight(int materialWithoutPawns) {
  float multiplier = 1 / materialEndgameStart;
  return 1 - std::min((float)1, (float)materialWithoutPawns * multiplier);
}

void init() {
  
}

} // end of namespace eval