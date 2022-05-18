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
	// define lookup tables
    int MGTable[12][64];
    int EGTable[12][64];

	// initialize all lookup tables
	void init() {
		// initialize middlegame and endgame tables
		for (int p = 0; p < 12; p++) {
			for (int sq = 0; sq < 64; sq++) {
				PieceType pt = (PieceType)(p % 6);
				Color c      = (Color)(p / 6);
				MGTable[p][sq] = PieceValueMG[pt] + PSQT_MG[pt][FLIP_SQ[c][sq]];
				EGTable[p][sq] = PieceValueEG[pt] + PSQT_EG[pt][FLIP_SQ[c][sq]];
			}
		}
	}

	// mop up evaluation functions
	float endgamePhaseWeight(int materialWithoutPawns) {
		float multiplier = 1 / materialEndgameStart;
		return 1 - std::min((float)1, (float)materialWithoutPawns * multiplier);
	}

	void printTables() {
		std::cout << "Middlegame:\n";
		for (int p = 0; p < 12; p++) {
			for (int sq = 0; sq < 64; sq++) {
				std::cout << MGTable[p][sq] << " ";
				if ((sq + 1) % 8 == 0) std::cout << "\n";
			}
			std::cout << "\n";
		}
	}

} // end of namespace eval