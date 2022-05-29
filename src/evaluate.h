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
static constexpr int doubledPawnPenaltyEG = 17;

// king safety bonus
static constexpr int kingShieldBonus = 5;

// maximum material to consider position as an endgame
static constexpr float materialEndgameStart = 1500;

// passed pawn bonus for each rank (white's perspective)
static constexpr int passedPawnBonusMG[8] = { 0, 9, 4, 1, 13, 48, 109, 0 };
static constexpr int passedPawnBonusEG[8] = { 0, 1, 5, 25, 50, 103, 149, 0 };

// lookup table for center manhattan distance
static constexpr int CenterManhattanDistance[64] = { 
    6, 5, 4, 3, 3, 4, 5, 6,
    5, 4, 3, 2, 2, 3, 4, 5,
    4, 3, 2, 1, 1, 2, 3, 4,
    3, 2, 1, 0, 0, 1, 2, 3,
    3, 2, 1, 0, 0, 1, 2, 3,
    4, 3, 2, 1, 1, 2, 3, 4,
    5, 4, 3, 2, 2, 3, 4, 5,
    6, 5, 4, 3, 3, 4, 5, 6
};

// calculate endgame phase weight 
float endgamePhaseWeight(int materialWithoutPawns);

// mop up evaluation (used for late endgame positions)
template<Color c>
int mopUpEval(Square theirKingSq, int ourMaterial, int theirMaterial, float endgameWeight) {
    int mopUpScore = 0;
    if ((ourMaterial > theirMaterial + PieceValue[Pawn] * 2) && endgameWeight > 0) {
        mopUpScore += CenterManhattanDistance[theirKingSq] * 10;
        return (int)(mopUpScore * endgameWeight);
    }
    return 0;
}

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

    // material scores for both sides
    int ourMaterial   = pos.mat_eg[c];
    int theirMaterial = pos.mat_eg[~c];

    // total material
    int totalMaterial = ourMaterial + theirMaterial;

    // peform mop up evaluation if in endgame position
    if (totalMaterial <= materialEndgameStart) {
        // material without pawns for calculating endgame phase weight
        int ourMaterialWithoutPawns   = ourMaterial   - popCount(pos.Pawns<c>()) * PieceValue[Pawn];
        int theirMaterialWithoutPawns = theirMaterial - popCount(pos.Pawns<~c>()) * PieceValue[Pawn];
        float ourEndgamePhaseWeight   = endgamePhaseWeight(ourMaterialWithoutPawns);
        float theirEndgamePhaseWeight = endgamePhaseWeight(theirMaterialWithoutPawns);

        // mop up eval
        eval.EGScores[c]  += mopUpEval<c>(bsf(pos.Kings<~c>()), ourMaterial, theirMaterial, ourEndgamePhaseWeight);
        eval.EGScores[~c] += mopUpEval<~c>(bsf(pos.Kings<c>()), theirMaterial, ourMaterial, theirEndgamePhaseWeight);
    }
    

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