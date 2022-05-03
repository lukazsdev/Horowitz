#include <vector>
#include "search.h"

int Search::scoreMove(Position pos, Move move) {
    int moveScore = 0;

    // if PV move scoring is allowed
    if (scorePV == 1) {
        // make sure we are dealing with PV move
        if (pvTable[0][ply] == move) {
            // disable score PV flag
            scorePV = 0;

            // give PV move highest score to search it first
            return 20000;
        }
    }

    // if move is a capture
    if (pos.board[move.target()] != None) {
        int attacker = makePiece(pos.sideToMove, move.piece());
        int victim   = pos.board[move.target()];
        moveScore += MVV_LVA[attacker][victim] + 10000;
    }
    // if move is enpassant capture
    else if (move.target() == pos.enpassantSquare && move.piece() == Pawn) {
        int attacker = makePiece(pos.sideToMove, Pawn);
        int victim = makePiece(~pos.sideToMove, Pawn);
        moveScore += MVV_LVA[attacker][victim] + 10000;
    }
    // score first killer move
    else if (killers[0][ply] == move) 
        moveScore += 9000;
    // score second killer move
    else if (killers[1][ply] == move) 
        moveScore += 8000;
    // score history move
    else 
        moveScore += history[makePiece(pos.sideToMove, move.piece())][move.target()];

    return moveScore;
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

void Search::sortMoves(Position pos, Moves &moveList) {
    // move scores
    std::vector<int> moveScores;

    // score each move in move list
    for (int count = 0; count < moveList.count; count++) {
        moveScores.push_back(scoreMove(pos, moveList.moves[count]));
    }

    // sort moves based on scores
    for (int currentMove = 0; currentMove < moveList.count; currentMove++) {
        for (int nextMove = currentMove + 1; nextMove < moveList.count; nextMove++) {
            // compare current and next move scores 
            if (moveScores[currentMove] < moveScores[nextMove]) {
                // swap scores
                int tempScore = moveScores[currentMove];
                moveScores[currentMove] = moveScores[nextMove];
                moveScores[nextMove] = tempScore;

                // swap moves
                Move tempMove = moveList.moves[currentMove];
                moveList.moves[currentMove] = moveList.moves[nextMove];
                moveList.moves[nextMove] = tempMove;
            }
        }
    }
}