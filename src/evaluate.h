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

namespace Eval {

void init();
void printTables();

// lookup tables for midgame and endgame scores
extern int MGTable[12][64];
extern int EGTable[12][64];

// struct for holding opening 
// and endgame scores
struct EvalInfo {
    int MGScores[2]{};
    int EGScores[2]{};
};

// maps a piece to how much weight it should have on the phase of the game
static constexpr int PawnPhase   = 1;
static constexpr int KnightPhase = 1;
static constexpr int BishopPhase = 1;
static constexpr int RookPhase   = 2;
static constexpr int QueenPhase  = 4;

// array used for mapping piece to its phase
static constexpr int PhaseValues[6] = {
    PawnPhase, 
    KnightPhase, 
    BishopPhase, 
    RookPhase,
    QueenPhase
};

// material piece values for midgame and endgame
static constexpr int PieceValueMG[6] = {102, 337, 365, 477, 1025,  0};
static constexpr int PieceValueEG[6] = {94, 281, 297, 512,  936,  0};
static constexpr int PieceValue[6] = {100, 320, 320, 500, 900, 0};

// doubled pawn penalties
static constexpr int doubledPawnsPenaltyMG = 5;
static constexpr int doubledPawnsPenaltyEG = 10;

// rook on open and semi-open file bonuses
static constexpr int openFileBonusMG     = 10;
static constexpr int semiOpenFileBonusMG = 5;
static constexpr int openFileBonusEG     = 10;
static constexpr int semiOpenFileBonusEG = 5;

// king safety bonuses
static constexpr int kingShieldBonus = 5;

// maximum material to consider position as an endgame
static constexpr float materialEndgameStart = 1620;

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


// pawn evaluation
template<Color c>
void evaluatePawn(Position& pos, Square sq, EvalInfo& eval) {
    Bitboard usPawns = pos.Pawns<c>();
    int file = file_of(sq);
    int doubledPawns = popCount(usPawns & MASK_FILE[file]);
    if (doubledPawns > 1) {
        eval.MGScores[c] -= (doubledPawns - 1) * doubledPawnsPenaltyMG;
        eval.EGScores[c] -= (doubledPawns - 1) * doubledPawnsPenaltyEG;
    }
}

// rook evaluation
template<Color c>
void evaluateRook(Position& pos, Square sq, EvalInfo& eval) {
    Bitboard usPawns    = pos.Pawns<c>();
    Bitboard theirPawns = pos.Pawns<~c>();
    int file = file_of(sq);
    if ((usPawns & MASK_FILE[file]) == 0) {
        eval.MGScores[c] += semiOpenFileBonusMG;
        eval.EGScores[c] += semiOpenFileBonusEG;
    }
    if (((usPawns | theirPawns) & MASK_FILE[file]) == 0) {
        eval.MGScores[c] += openFileBonusMG;
        eval.EGScores[c] += openFileBonusEG;
    }
}

// king evaluation
template<Color c>
void evaluateKing(Position& pos, Square sq, EvalInfo& eval) {
    Bitboard usPawns    = pos.Pawns<c>();
    Bitboard theirPawns = pos.Pawns<~c>();
    int file = file_of(sq);
    if ((usPawns & MASK_FILE[file]) == 0) 
        eval.MGScores[c] -= semiOpenFileBonusMG;
    if (((usPawns | theirPawns) & MASK_FILE[file]) == 0) 
        eval.MGScores[c] -= openFileBonusMG;

    eval.MGScores[c] += popCount(pos.GetKingAttacks(sq) & usPawns) * kingShieldBonus;
}

template<Color c>
void evaluatePiece(Position& pos, PieceType pt, Square sq, EvalInfo& eval) {
    switch (pt) {
    case Pawn:
        evaluatePawn<c>(pos, sq, eval);
        break;
    case Rook:
        evaluateRook<c>(pos, sq, eval);
        break;
    case King:
        evaluateKing<c>(pos, sq, eval);
        break;
    default:
        break;
    }
}

// static evaluation function. Returns 
// the score relative to the side to move 
template<Color c>
int evaluate(Position& pos) {
    // initialize evaluation scores
    EvalInfo eval = {};

    // phase for tappered evaluation and material values
    int phase = 0, ourMaterial = 0, theirMaterial = 0;
    
    // retrieve bitboard of both occupancies
    Bitboard allBB = pos.allPieces<White>() | pos.allPieces<Black>();
    
    // loop over allBB to retrieve each piece 
    // and procede to evaluate them individually
    while (allBB) {
        // retrieve current piece info
        Square sq   = poplsb(allBB);
        Piece p     = pos.board[sq];
        Color color = (Color)(p / 6);

        // evaluate current piece
        eval.MGScores[color] += MGTable[p][sq];
        eval.EGScores[color] += EGTable[p][sq];

        // update game phase
        PieceType pt = (PieceType)(p % 6);
        phase += PhaseValues[pt];

        // evaluate other piece factors 
        if (color == White) 
            evaluatePiece<White>(pos, pt, sq, eval);
        else 
            evaluatePiece<Black>(pos, pt, sq, eval);

        // update material values
        if (color == c) ourMaterial += PieceValue[pt];
        else theirMaterial += PieceValue[pt];
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

    // get total midgame and endgame scores
    int MGScore = eval.MGScores[c] - eval.MGScores[~c];
    int EGScore = eval.EGScores[c] - eval.EGScores[~c];

    // interpolate midgame and endgame scores (tappered eval)
    int MGPhase = phase;
    if (MGPhase > 24) MGPhase = 24;
    int EGPhase = 24 - MGPhase;
    return (MGScore * MGPhase + EGScore * EGPhase) / 24;
}

} // end of namespace eval