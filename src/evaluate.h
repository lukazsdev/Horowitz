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

// evaluation penalties
static constexpr int doubledPawnPenaltyMG = 5;
static constexpr int doubledPawnPenaltyEG = 10;

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