#include "evaluate.h"

// static evaluation function
int evaluate(Position pos) {
    // initialize eval instance
    Evaluation eval;

    // initialize current game phase 
    // and game phase score
    int phaseScore = getPhaseScore(pos);
    int phase = getPhase(phaseScore);

    // loop over all pieces
    for (int bb_piece = (int)WhitePawn; bb_piece <= (int)BlackKing; bb_piece++) {
        Bitboard bitboard = pos.PiecesBB[bb_piece];
        
        PieceType piece = (PieceType)(bb_piece % 6);
        Color color = (Color)(bb_piece / 6);

        // increment opening and endgame scores based on
        // the current piece's positional and material score
        while (bitboard) {
            Square sq = poplsb(bitboard);

            eval.OPScores[color] += materialScore[openingPhase][piece];
            eval.EGScores[color] += materialScore[endgamePhase][piece];

            evalPiece(piece, pos, color, sq, eval);
        }
    }
    // tappered evaluation
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

// evaluate pawns
void evalPawn(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Pawn][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Pawn][FLIP_SQ[color][sq]];
}

// evaluate knights
void evalKnight(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Knight][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Knight][FLIP_SQ[color][sq]];
}

// evaluate bishops
void evalBishop(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Bishop][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Bishop][FLIP_SQ[color][sq]];
}

// evaluate rooks
void evalRook(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Rook][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Rook][FLIP_SQ[color][sq]];
}

// evaluate queens
void evalQueen(Position pos, Color color, Square sq, Evaluation &eval) {
    eval.OPScores[color] += PSQT[openingPhase][Queen][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][Queen][FLIP_SQ[color][sq]];
}

// evaluate king
void evalKing(Position pos, Color color, Square sq, Evaluation &eval) {
    //std::cout << sq << std::endl;
    eval.OPScores[color] += PSQT[openingPhase][King][FLIP_SQ[color][sq]];
    eval.EGScores[color] += PSQT[endgamePhase][King][FLIP_SQ[color][sq]];
}

// get game phase score
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

// get current game phase
int getPhase(int phaseScore) {
    if (phaseScore > openingPhaseScore) 
        return openingPhase;

    else if (phaseScore < endgamePhaseScore) 
        return endgamePhase;
    
    return middlegamePhase;
}

// function for calling the evaluation
// function for the given piece
void evalPiece(PieceType piece, Position pos, Color color, Square sq, Evaluation &eval) {
    switch (piece) {
        case Pawn: evalPawn(pos, color, sq, eval);     break;
        case Knight: evalKnight(pos, color, sq, eval); break;
        case Bishop: evalBishop(pos, color, sq, eval); break;
        case Rook: evalRook(pos, color, sq, eval);     break;
        case Queen: evalQueen(pos, color, sq, eval);   break;
        case King: evalKing(pos, color, sq, eval);     break;
        default:                                       break;
    }

}

// this is implemented so that the score is positive 
// if the current side to move is better than the opponent
// and negative otherwise
int perspective(int score, Color color) {
    return color == White ? score : -score;
}