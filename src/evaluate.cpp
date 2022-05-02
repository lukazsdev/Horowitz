#include "evaluate.h"

int evaluate(Position pos) {
    Evaluation eval;

    int phaseScore = getPhaseScore(pos);
    int phase = getPhase(phaseScore);

    for (int bb_piece = (int)WhitePawn; bb_piece <= (int)BlackKing; bb_piece++) {
        Bitboard bitboard = pos.PiecesBB[bb_piece];
        
        PieceType piece = (PieceType)(bb_piece % 6);
        Color color = (Color)(bb_piece / 6);

        while (bitboard) {
            Square sq = poplsb(bitboard);

            eval.OPScores[color] += materialScore[openingPhase][piece];
            eval.EGScores[color] += materialScore[endgamePhase][piece];

            evalPiece(piece, pos, color, sq, eval);
        }
    }
    int openingScore = eval.OPScores[White] - eval.OPScores[Black];
    int endgameScore = eval.EGScores[White] - eval.EGScores[Black];

    if (phase == middlegamePhase) 
        eval.score = (openingScore * phaseScore + endgameScore * 
                (openingPhaseScore - phaseScore)) / openingPhaseScore;
    
    else if (phase == openingPhase) {
        eval.score = openingScore;
    }

    else if (phase == endgamePhase) 
        eval.score = endgameScore;

    return perspective(eval.score, pos.sideToMove);
}

void evalPawn(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Pawn][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Pawn][FLIP_SQ[color][sq]];
}

void evalKnight(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Knight][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Knight][FLIP_SQ[color][sq]];
}

void evalBishop(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Bishop][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Bishop][FLIP_SQ[color][sq]];
}

void evalRook(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Rook][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Rook][FLIP_SQ[color][sq]];
}

void evalQueen(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Queen][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Queen][FLIP_SQ[color][sq]];
}


void evalKing(Position pos, Color color, Square sq, Evaluation &eval) {
    //std::cout << sq << std::endl;
    eval.OPScores[color] += PSQT[openingPhase][King][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][King][FLIP_SQ[color][sq]];
}

int getPhaseScore(Position pos) {
    int white_piece_scores = 0, black_piece_scores = 0;

    for (Piece piece = WhiteKnight; piece <= WhiteQueen; piece = Piece(piece + 1)) {
        white_piece_scores += popCount(pos.PiecesBB[piece]) * materialScore[openingPhase][piece % 6];
    }

    for (Piece piece = BlackKnight; piece <= BlackQueen; piece = Piece(piece + 1)) {
        black_piece_scores += popCount(pos.PiecesBB[piece]) * materialScore[openingPhase][piece % 6];
    }

    return white_piece_scores + black_piece_scores;
}

int getPhase(int phaseScore) {
    if (phaseScore > openingPhaseScore) 
        return openingPhase;

    else if (phaseScore < endgamePhaseScore) 
        return endgamePhase;
    
    return middlegamePhase;
}


void evalPiece(PieceType piece, Position pos, Color color, Square sq, Evaluation &eval) {
    switch (piece) {
        case Pawn: evalPawn(pos, color, sq, eval); break;
        case Knight: evalKnight(pos, color, sq, eval); break;
        case Bishop: evalBishop(pos, color, sq, eval); break;
        case Rook: evalRook(pos, color, sq, eval); break;
        case Queen: evalQueen(pos, color, sq, eval); break;
        case King: evalKing(pos, color, sq, eval); break;
        default: break;
    }

}


int perspective(int score, Color color) {
    if (color == White)
        return score;
    else
        return -score;
}