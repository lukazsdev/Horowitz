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

// evaluation bonuses
static constexpr int tempoBonus = 5;
static constexpr int bishopPairBonus = 10;

// maximum material to consider position as an endgame
static constexpr float materialEndgameStart = 1500;

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

    // phase for tappered evaluation and material values
    int phase = pos.phase;

    // material scores for both sides
    int ourMaterial   = pos.mat_eg[c];
    int theirMaterial = pos.mat_eg[~c];

    // add tempo bonus to side to move
    eval.MGScores[c] += tempoBonus;

    // add bishop pair bonus 
    Bitboard usBishops    = pos.Bishops<c>();
    Bitboard theirBishops = pos.Bishops<~c>();

    if (popCount(usBishops) > 1) {
        eval.MGScores[c] += bishopPairBonus;
        eval.EGScores[c] += bishopPairBonus;
    }

    if (popCount(theirBishops) > 1) {
        eval.MGScores[~c] += bishopPairBonus;
        eval.EGScores[~c] += bishopPairBonus;
    }

    // if current pawn structure is not in pawn hash table then store 
    // current pawn structure eval in hash table. Else, retrieve the score.
    int tableEval = PT.Read(pos.pawnHashKey);
    if (tableEval == NoHashEntry) { 
        // reset table evaluation
        tableEval = 0;

        // evaluate pawns
        Bitboard whitePawns = pos.Pawns<White>();
        Bitboard blackPawns = pos.Pawns<Black>();
        Bitboard allPawns = whitePawns | blackPawns;
        while (allPawns) {
            Square sq = poplsb(allPawns);
            Color color = (Color)(pos.board[sq] / 6);
            uint8_t file = file_of(sq);
            // doubled pawn penalty
            int whiteDoubled = popCount(whitePawns & MASK_FILE[file]);
            int blackDoubled = popCount(blackPawns & MASK_FILE[file]);
            tableEval += (color == White) ? (-(whiteDoubled - 1) * 5) : ((blackDoubled - 1) * 5);
        }

        // store pawn score in PT relative to side
        PT.Store(pos.pawnHashKey, tableEval);
    }

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

    // add pawn hash table evaluation to final evaluation
    MGScore += (c == White) ? tableEval : -tableEval;

    // interpolate midgame and endgame scores (tappered eval)
    int MGPhase = phase;
    if (MGPhase > 24) MGPhase = 24;
    int EGPhase = 24 - MGPhase;
    return (MGScore * MGPhase + EGScore * EGPhase) / 24;
}

} // end of namespace eval