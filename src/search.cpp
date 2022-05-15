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

int Search::scoreMove(Move move) {
    int moveScore = 0;

    // if PV move scoring is allowed
    if (scorePV == 1) {
        // make sure we are dealing with PV move
        if (pvTable[0][ply] == move) {
            // disable score PV flag
            scorePV = 0;

            // give PV move highest score to search it first
            return 20000;
        }
    }

    // if move is a capture
    if (pos.board[move.target()] != None) {
        int attacker = makePiece(pos.sideToMove, move.piece());
        int victim   = pos.board[move.target()];
        moveScore += MVV_LVA[attacker][victim] + 10000;
    }
    // if move is enpassant capture
    else if (move.target() == pos.enpassantSquare && move.piece() == Pawn) {
        int attacker = makePiece(pos.sideToMove, Pawn);
        int victim = makePiece(~pos.sideToMove, Pawn);
        moveScore += MVV_LVA[attacker][victim] + 10000;
    }
    // score first killer move
    else if (killers[0][ply] == move) 
        moveScore += 9000;
    // score second killer move
    else if (killers[1][ply] == move) 
        moveScore += 8000;
    // score history move
    else 
        moveScore += history[makePiece(pos.sideToMove, move.piece())][move.target()];

    return moveScore;
}

void Search::enablePVScoring(Moves moveList) {
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

void Search::sortMoves(Moves &moveList) {
    // move scores
    std::vector<int> moveScores;

    // score each move in move list
    for (int count = 0; count < moveList.count; count++) {
        moveScores.push_back(scoreMove(moveList.moves[count]));
    }

    // sort moves based on scores
    for (int currentMove = 0; currentMove < moveList.count; currentMove++) {
        for (int nextMove = currentMove + 1; nextMove < moveList.count; nextMove++) {
            // compare current and next move scores 
            if (moveScores[currentMove] < moveScores[nextMove]) {
                // swap scores
                int tempScore = moveScores[currentMove];
                moveScores[currentMove] = moveScores[nextMove];
                moveScores[nextMove] = tempScore;

                // swap moves
                Move tempMove = moveList.moves[currentMove];
                moveList.moves[currentMove] = moveList.moves[nextMove];
                moveList.moves[nextMove] = tempMove;
            }
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
