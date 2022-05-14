#pragma once

#include "chess.h"
#include "psqt.h"

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

// mobility units for each piece
static constexpr int KnightMobilityUnit = 4;
static constexpr int BishopMobilityUnit = 4;
static constexpr int RookMobilityUnit   = 5;
static constexpr int QueenMobilityUnit  = 9;  

// piece mobility for midgame and endgame
static constexpr int PieceMobilityMG[4] = {4, 5, 0, 0};
static constexpr int PieceMobilityEG[4] = {3, 5, 2, 1};

// evaluation masks 
extern Bitboard isolatedPawnMasks[8];

// penalties
static constexpr int isolatedPawnPenalityMG = 5;
static constexpr int isolatedPawnPenalityEG = 10;
static constexpr int backwardPawnPenalityMG = 20;
static constexpr int backwardPawnPenalityEG = 30;

// initialize all evaluation masks
void init();
// check whether given pawn is a backwards pawn or not
bool isBackwardsPawn(Square sq, Bitboard ourPawnsBB, Color c);

// evaluate pawns
template<Color c>
void evalPawn(Position& pos, Square sq, EvalInfo &eval) {
    // add material + piece square table scores
    eval.MGScores[c] += PieceValueMG[Pawn] + PSQT_MG[Pawn][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Pawn] + PSQT_EG[Pawn][FLIP_SQ[c][sq]];

    // retrieve our pawns bitboard
    Bitboard ourPawnsBB = pos.Pawns<c>();
    int file = file_of(sq);

    // evaluate isolated pawn
    if ((isolatedPawnMasks[file] & ourPawnsBB) == 0) {
        eval.MGScores[c] -= isolatedPawnPenalityMG;
        eval.EGScores[c] -= isolatedPawnPenalityEG;
    }

    // evaluate backward pawn (only check if both adjacent files have pawns)
    if (file > 0 && file < 7) {
        if ((MASK_FILE[file-1] & ourPawnsBB) && (MASK_FILE[file+1] & ourPawnsBB)) {
            if (isBackwardsPawn(sq, ourPawnsBB, c)) {
                eval.MGScores[c] -= backwardPawnPenalityMG;
                eval.EGScores[c] -= backwardPawnPenalityEG;
            }
        }
    }

}

// evaluate knights
template<Color c>
void evalKnight(Position& pos, Square sq, EvalInfo &eval) {
    // add material + piece square table scores
    eval.MGScores[c] += PieceValueMG[Knight] + PSQT_MG[Knight][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Knight] + PSQT_EG[Knight][FLIP_SQ[c][sq]];   

    // retrieve important bitboards
    Bitboard usBB = pos.allPieces<c>(); 

    // evaluate piece mobility
    Bitboard moves = pos.GetKnightAttacks(sq) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += ((mobility - KnightMobilityUnit) * PieceMobilityMG[Knight - 1]) / 2;
    eval.EGScores[c] += ((mobility - KnightMobilityUnit) * PieceMobilityEG[Knight - 1]) / 2;
}

// evaluate bishops
template<Color c>
void evalBishop(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Bishop] + PSQT_MG[Bishop][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Bishop] + PSQT_EG[Bishop][FLIP_SQ[c][sq]];

    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = pos.GetBishopAttacks(sq, allBB) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += ((mobility - BishopMobilityUnit) * PieceMobilityMG[Bishop - 1]) / 2;
    eval.EGScores[c] += ((mobility - BishopMobilityUnit) * PieceMobilityEG[Bishop - 1]) / 2;
    
}

// evaluate rooks
template<Color c>
void evalRook(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Rook] + PSQT_MG[Rook][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Rook] + PSQT_EG[Rook][FLIP_SQ[c][sq]];

    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = pos.GetRookAttacks(sq, allBB) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += ((mobility - RookMobilityUnit) * PieceMobilityMG[Rook - 1]) / 2;
    eval.EGScores[c] += ((mobility - RookMobilityUnit) * PieceMobilityEG[Rook - 1]) / 2;
}

// evaluate queens
template<Color c>
void evalQueen(Position& pos, Square sq, EvalInfo &eval) {
    eval.MGScores[c] += PieceValueMG[Queen] + PSQT_MG[Queen][FLIP_SQ[c][sq]];
    eval.EGScores[c] += PieceValueEG[Queen] + PSQT_EG[Queen][FLIP_SQ[c][sq]];

    
    // retrieve important bitboards
    Bitboard usBB  = pos.allPieces<c>();
    Bitboard allBB = pos.allPieces<c>() | pos.allPieces<~c>();

    // evaluate piece mobility
    Bitboard moves = (pos.GetRookAttacks(sq, allBB) | pos.GetBishopAttacks(sq, allBB)) & ~usBB;
    int mobility   = popCount(moves);

    // add mobility bonuses
    eval.MGScores[c] += ((mobility - QueenMobilityUnit) * PieceMobilityMG[Queen - 1]) / 2;
    eval.EGScores[c] += ((mobility - QueenMobilityUnit) * PieceMobilityEG[Queen - 1]) / 2;
    
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