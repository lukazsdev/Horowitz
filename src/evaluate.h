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

#include <algorithm>

#include "chess.h"
#include "psqt.h"
#include "pawns.h"

namespace Eval {

// Pawn hash table
extern PawnHashTable PT;

// evaluation masks
extern Bitboard whitePassedMasks[64];
extern Bitboard blackPassedMasks[64];

// init evaluation masks
void init();

// struct for holding opening 
// and endgame scores
struct EvalInfo {
    int MGScores[2]{};
    int EGScores[2]{};
};

// evaluation penalties
static constexpr int doubledPawnPenaltyMG = 5;
static constexpr int doubledPawnPenaltyEG = 10;

// passed pawn bonus for each rank (white's perspective)
static constexpr int passedPawnBonusMG[8] = {0, 10, 30, 50, 75, 100, 150, 200};
static constexpr int passedPawnBonusEG[8] = {0, 10, 30, 50, 75, 100, 150, 200};

// static evaluation function. Returns 
// the score relative to the side to move 
template<Color c>
int evaluate(Position& pos) {
    // initialize evaluation scores
    EvalInfo eval = {};

    // phase for tappered evaluation 
    int phase   = pos.phase;
    int MGPhase = phase;
    if (MGPhase > 24) MGPhase = 24;
    int EGPhase = 24 - MGPhase;

    // if current pawn structure is not in pawn hash table then store 
    // current pawn structure eval in hash table. Else, retrieve the score.
    int hashMGScores[2], hashEGScores[2];

    hashMGScores[White] = PT.Read(pos.pawnHashKey, White, MG);
    hashMGScores[Black] = PT.Read(pos.pawnHashKey, Black, MG);
    hashEGScores[White] = PT.Read(pos.pawnHashKey, White, EG);
    hashEGScores[Black] = PT.Read(pos.pawnHashKey, Black, EG);

    if (hashMGScores[White] == NoHashEntry ||
        hashMGScores[Black] == NoHashEntry ||
        hashEGScores[White] == NoHashEntry ||
        hashEGScores[Black] == NoHashEntry) {

            // reset hash scores
            hashMGScores[White] = 0;
            hashMGScores[Black] = 0;
            hashEGScores[White] = 0;
            hashEGScores[Black] = 0;

            // evaluate pawns
            Bitboard whitePawns = pos.Pawns<White>();
            Bitboard blackPawns = pos.Pawns<Black>();
            Bitboard allPawns = whitePawns | blackPawns;
            while (allPawns) {
                Square sq = poplsb(allPawns);
                Color color = (Color)(pos.board[sq] / 6);
                uint8_t file = file_of(sq);
                uint8_t rank = rank_of(sq);

                if (color == White) {
                    // doubled pawn penalty
                    int whiteDoubled = popCount(whitePawns & MASK_FILE[file]);
                    hashMGScores[White] -= (whiteDoubled - 1) * doubledPawnPenaltyMG;
                    hashEGScores[White] -= (whiteDoubled - 1) * doubledPawnPenaltyEG;
                    // passed pawn bonus
                    if ((whitePassedMasks[sq] & blackPawns) == 0) {
                        hashMGScores[White] += passedPawnBonusMG[rank];
                        hashEGScores[White] += passedPawnBonusEG[rank];
                    }
                }
                else {
                    // doubled pawn penalty
                    int blackDoubled = popCount(blackPawns & MASK_FILE[file]);
                    hashMGScores[Black] -= (blackDoubled - 1) * doubledPawnPenaltyMG;
                    hashEGScores[Black] -= (blackDoubled - 1) * doubledPawnPenaltyEG;
                    // passed pawn bonus
                    if ((blackPassedMasks[sq] & whitePawns) == 0) {
                        hashMGScores[Black] += passedPawnBonusMG[7 - rank];
                        hashEGScores[Black] += passedPawnBonusEG[7 - rank];
                    }
                }
            }

            // store pawn structure scores in pawn hash table
            PT.Store(pos.pawnHashKey, hashMGScores, hashEGScores);
        }
    
    // add pawn scores to main eval
    eval.MGScores[White] += hashMGScores[White];
    eval.MGScores[Black] += hashMGScores[Black];
    eval.EGScores[White] += hashEGScores[White];
    eval.EGScores[Black] += hashEGScores[Black];

    eval.MGScores[c] += pos.mat_mg[c] + pos.psqt_mg[c]; 
    eval.EGScores[c] += pos.mat_eg[c] + pos.psqt_eg[c]; 

    eval.MGScores[~c] += pos.mat_mg[~c] + pos.psqt_mg[~c]; 
    eval.EGScores[~c] += pos.mat_eg[~c] + pos.psqt_eg[~c]; 

    // get total midgame and endgame scores
    int MGScore = eval.MGScores[c] - eval.MGScores[~c];
    int EGScore = eval.EGScores[c] - eval.EGScores[~c];

    // interpolate midgame and endgame scores (tappered eval)
    return (MGScore * MGPhase + EGScore * EGPhase) / 24;
}

} // end of namespace eval