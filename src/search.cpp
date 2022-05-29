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

void Search::scoreMove(Move& move) {
    move.score = 0;

    // if PV move scoring is allowed
    if (scorePV == 1) {
        // make sure we are dealing with PV move
        if (pvTable[0][ply] == move) {
            // disable score PV flag
            scorePV = 0;

            // give PV move highest score to search it first
            move.score = 20000;
            return;
        }
    }

    // if move is a capture
    if (pos.board[move.target()] != None) {
        int attacker = makePiece(pos.sideToMove, move.piece());
        int victim   = pos.board[move.target()];
        move.score += MVV_LVA[attacker][victim] + 10000;
    }
    // if move is enpassant capture
    else if (move.target() == pos.enpassantSquare && move.piece() == Pawn) {
        int attacker = makePiece(pos.sideToMove, Pawn);
        int victim = makePiece(~pos.sideToMove, Pawn);
        move.score += MVV_LVA[attacker][victim] + 10000;
    }
    // score first killer move
    else if (killers[0][ply] == move) 
        move.score += 9000;
    // score second killer move
    else if (killers[1][ply] == move) 
        move.score += 8000;
    // score history move
    else 
        move.score += history[makePiece(pos.sideToMove, move.piece())][move.target()];
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
    // asign score to each move
    for (int i = 0; i < moveList.count; i++) {
        scoreMove(moveList.moves[i]);
    }

    // insertion sort
    int i = moveList.count;
    for (int cmove = 1; cmove < moveList.count; cmove++) {
        Move temp = moveList.moves[cmove];
        for (i = cmove-1; i>=0 && (moveList.moves[i].score < temp.score); i--) 
            moveList.moves[i+1] = moveList.moves[i];
        moveList.moves[i+1] = temp;
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