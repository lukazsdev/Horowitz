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

#include <vector>
#include "search.h"

void Search::scoreMoves(Moves& moveList) {
    for (int index = 0; index < moveList.count; index++) {
        Move move = moveList.moves[index];

        int score = 0;

        if (move == pvTable[0][ply]) {
            score += mvvLvaOffset + pvMoveScore;
        }
        else if (pos.board[move.target()] != None) {
            PieceType moved    = (PieceType)(pos.board[move.source()] % 6);
            PieceType captured = (PieceType)(pos.board[move.target()] % 6);
            score += mvvLvaOffset + MvvLva[captured][moved];
        }
        else if (move.target() == pos.enpassantSquare && move.piece() == Pawn) {
            score += mvvLvaOffset + MvvLva[Pawn][Pawn];
        }
        else {
            int moveScore = 0;
            for (int i = 0; i < maxKillers; i++) {
                if (move == killers[ply][i]) {
                    moveScore = mvvLvaOffset - (i + 1) * killerMoveScore;
                    break;
                }
            }

            if (moveScore == 0) {
                moveScore = history[pos.sideToMove][move.source()][move.target()];
            }

            score += moveScore;
        }

        moveList.moves[index].score = score;
    }
}

void Search::orderMoves(Moves& moveList, int currIndex) {
    int bestIndex = currIndex;
    int bestScore = moveList.moves[bestIndex].score;

    for (int index = bestIndex; index < moveList.count; index++) {
        if (moveList.moves[index].score > bestScore) {
            bestIndex = index;
            bestScore = moveList.moves[index].score;
        }
    }

    Move tempMove = moveList.moves[currIndex];
    moveList.moves[currIndex] = moveList.moves[bestIndex];
    moveList.moves[bestIndex] = tempMove;
}

void Search::enablePVScoring(Moves& moveList) {
    // disable following PV line
    followPV = 0;

    // loop over moves in move list
    for (int count = 0; count < moveList.count; count++) {
        // make sure we did PV move
        if (pvTable[0][ply] == moveList.moves[count]) {
            // enable move scoring
            scorePV = 1;

            // enable following PV
            followPV = 1;
        }
    }
}


void Search::ageHistoryTable() {
    for (int sq1 = 0; sq1 < 64; sq1++) {
		for (int sq2 = 0; sq2 < 64; sq2++) {
			history[pos.sideToMove][sq1][sq2] /= 2;
		}
	}
}

// add a hash key (position) to table
void Repetition::Add(uint64_t hash) {
    count++;
    repetitionsTable[count] = hash;
}

void Repetition::Reset() {
    memset(repetitionsTable, 0, sizeof(repetitionsTable));
    count = 0;
}

// check whether the current position has ocurred
bool Repetition::isRepetition(Position& pos) {
    for (int idx = 0; idx < count; idx++) {
        if (repetitionsTable[idx] == pos.hashKey)
            return true;
    }
    return false;
} 

void Search::printBestMove(Move bestMove) {
    std::cout << "bestmove" << " ";
    if (bestMove.promoted()) {
         std::cout << bestMove.toUci() << promotedPieceToChar[bestMove.piece()] << std::endl;
    }
    else std::cout << bestMove.toUci() << std::endl;
}
