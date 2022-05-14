#pragma once

#include "chess.h"

namespace Eval {

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
static constexpr int TotalPhase = PawnPhase*16 + KnightPhase*4 + BishopPhase* 4 + RookPhase*4 + QueenPhase*2;

// array used for mapping piece to its phase
static constexpr int PhaseValues[6] = {
    PawnPhase, 
    KnightPhase, 
    BishopPhase, 
    RookPhase,
    QueenPhase
};

// material piece values for midgame and endgame
static constexpr int PieceValueMG[6] = {82, 337, 365, 477, 1025,  0};
static constexpr int PieceValueEG[6] = {94, 281, 297, 512,  936,  0};

// mobility for midgame and endgame
//static constexpr int PieceMobilityMG[4] = { 3, 4, 5, 1 };
//static constexpr int PieceMobilityEG[4] = { 4, 4, 3, 7 };

// piece square tables (positional piece scores)
extern const int PSQT_MG[6][64];
extern const int PSQT_EG[6][64];
extern const int FLIP_SQ[2][64];

// evaluate pawns
template<Color c>
void evalPawn(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Pawn] + PSQT_MG[Pawn][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Pawn] + PSQT_EG[Pawn][FLIP_SQ[c][sq]];
}

// evaluate knights
template<Color c>
void evalKnight(Position& pos, Square sq, EvalInfo &eval) {
    // add material + piece square table scores
    eval.MGScores[c] += PieceValueMG[Knight] + PSQT_MG[Knight][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Knight] + PSQT_EG[Knight][FLIP_SQ[c][sq]];

    /*
    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();

    // evaluate piece mobility
    Bitboard moves = pos.GetKnightAttacks(sq) ^ ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += (mobility - 4) * PieceMobilityMG[Knight - 1];
    eval.EGScores[c] += (mobility - 4) * PieceMobilityEG[Knight - 1];
    */
}

// evaluate bishops
template<Color c>
void evalBishop(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Bishop] + PSQT_MG[Bishop][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Bishop] + PSQT_EG[Bishop][FLIP_SQ[c][sq]];

    /*
    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = pos.GetBishopAttacks(sq, allBB) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += (mobility - 7) * PieceMobilityMG[Bishop - 1];
    eval.EGScores[c] += (mobility - 7) * PieceMobilityEG[Bishop - 1];
    */
}

// evaluate rooks
template<Color c>
void evalRook(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Rook] + PSQT_MG[Rook][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Rook] + PSQT_EG[Rook][FLIP_SQ[c][sq]];

    /*
    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = pos.GetRookAttacks(sq, allBB) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += (mobility - 7) * PieceMobilityMG[Rook - 1];
    eval.EGScores[c] += (mobility - 7) * PieceMobilityEG[Rook - 1];
    */
}

// evaluate queens
template<Color c>
void evalQueen(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Queen] + PSQT_MG[Queen][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Queen] + PSQT_EG[Queen][FLIP_SQ[c][sq]];

    /*
    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = (pos.GetRookAttacks(sq, allBB) | pos.GetBishopAttacks(sq, allBB)) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += (mobility - 14) * PieceMobilityMG[Queen - 1];
    eval.EGScores[c] += (mobility - 14) * PieceMobilityEG[Queen - 1];
    */
}

// evaluate kings
template<Color c>
void evalKing(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[King] + PSQT_MG[King][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[King] + PSQT_EG[King][FLIP_SQ[c][sq]];
}

template<Color c>
void evalPiece(Position& pos, PieceType pt, Square sq, EvalInfo &eval) {
    switch (pt) {
        case Pawn:
            evalPawn<c>(pos, sq, eval);
            break;
        case Knight:
            evalKnight<c>(pos, sq, eval);
            break;
        case Bishop:
            evalBishop<c>(pos, sq, eval);
            break;
        case Rook:
            evalRook<c>(pos, sq, eval);
            break;
        case Queen:
            evalQueen<c>(pos, sq, eval);
            break;
        default: 
            break;
        }
}

// main evaluation function. Returns score relative to the side to move
template<Color c>
int evaluate(Position& pos) {
    EvalInfo eval;

    // PawnPhase*16 + KnightPhase*4 + BishopPhase*4 + RookPhase*4 + QueenPhase*2
    int phase = TotalPhase;
    
    Bitboard allBB = pos.allPieces<White>() | pos.allPieces<Black>();
    
    while (allBB) {
        Square sq    = poplsb(allBB);

        Piece piece  = pos.board[sq];
        PieceType pt = (PieceType)(piece % 6);
        Color color  = (Color)(piece / 6);

        if (color == White) 
            evalPiece<White>(pos, pt, sq, eval);
        else 
            evalPiece<Black>(pos, pt, sq, eval);
        
        phase -= PhaseValues[pt];
    }

    int MGScore = eval.MGScores[c] - eval.MGScores[~c];
    int EGScore = eval.EGScores[c] - eval.EGScores[~c];

    phase = (phase*256 + (TotalPhase / 2)) / TotalPhase;
    return (MGScore * (256 - phase) + EGScore * phase) / 256;
}



}; // end if evaluation namespace