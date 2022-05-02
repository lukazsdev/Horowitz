#pragma once

#include <chrono>
#include "chess.h"
#include "search.h"
#include "evaluate.h"

// search constants
static constexpr int maxPly = 64;
static constexpr int infinity = 10000;
static constexpr int checkmate = 9000;
static constexpr int windowSize = 50;

class Search {
public:
    uint64_t nodes;
    int ply;

    int pvLength[maxPly];
    Move pvTable[maxPly][maxPly];

    int followPV, scorePV;

public:
    template<Color c> void search(Position pos, int depth);
    template<Color c> int quiescence(Position pos, int alpha, int beta);
    template<Color c> int negamax(Position pos, int alpha, int beta, int depth);
};

template<Color c> 
int Search::quiescence(Position pos, int alpha, int beta) {
    int evaluation = evaluate(pos);

    nodes++;

    if (ply > maxPly - 1)
        return evaluation;
    
    if (evaluation >= beta)
        return beta;

    if (evaluation > alpha)
        alpha = evaluation;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // iterate over legal moves
    for (int i = 0; i < moveList.count; i++) {
        // initialize current move
        Move move = moveList.moves[i];

        // check if move isn't a capture
        if (pos.board[move.target()] == None) 
            continue;
        

        // increment ply
        ply++;

        // make move
        pos.makemove<c>(move);

        // recursively call negamax
        int score = -quiescence<~c>(pos, -beta, -alpha);

        // unmake move
        pos.unmakemove<c>(move);

        // decrement ply
        ply--;

        // fail-hard beta cutoff
        if (score >= beta) {
            return beta;
        }

        // found a better move
        if (score > alpha) {
            // PV node (move)
            alpha = score;
        }
    }

    return alpha;
 
}

// negamax search
template<Color c> 
int Search::negamax(Position pos, int alpha, int beta, int depth) {
    // increment nodes
    nodes++;

    // initialize pv length
    pvLength[ply] = ply;

    // if depth limit is greater than max ply.
    // exit if current ply is greater than max ply
    if (ply >= maxPly) {
        return evaluate(pos);
    }

    // check if king is in check
    bool in_check = pos.isSquareAttacked<~c>(pos.KingSq<c>());

    // check if we have reached the depth limit
    // then search all possible captures 
    if (depth == 0) {
        //return evaluate(pos);
        return quiescence<c>(pos, alpha, beta);
    }

    // legal moves counter
    int legalMoves = 0;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // iterate over legal moves
    for (int i = 0; i < moveList.count; i++) {
        // initialize current move
        Move move = moveList.moves[i];

        // increment ply
        ply++;

        // make move
        pos.makemove<c>(move);

        // increment legal moves counter
        legalMoves++;

        // recursively call negamax
        int score = -negamax<~c>(pos, -beta, -alpha, depth - 1);

        // unmake move
        pos.unmakemove<c>(move);

        // decrement ply
        ply--;

        // fail-hard beta cutoff
        if (score >= beta) {
            return beta;
        }

        // found a better move
        if (score > alpha) {
            // PV node (move)
            alpha = score;

            // write pv move to table
            pvTable[ply][ply] = move;

            // copy move from deeper ply into current ply's line
            for (int next_ply = ply + 1; next_ply < pvLength[ply + 1]; next_ply++) {
                pvTable[ply][next_ply] = pvTable[ply + 1][next_ply];
            }

            // adjust pv length
            pvLength[ply] = pvLength[ply + 1];
        }
    }
 
    // no legal moves
    if (legalMoves == 0) {
        // checkmate
        if (in_check) 
            return -checkmate + ply;
        // if not, then stalemate
        else
            return 0;
    }

    // node (move) fails low
    return alpha;
}

// root search function (iterative deepening search)
template<Color c> 
void Search::search(Position pos, int depth) {
    // start search timer
    auto t1 = std::chrono::high_resolution_clock::now();

    // initialize best move to null (no move)
    Move *bestMove = NULL;

    // last iteration score
    //int lastScore = 0;

    // reset search info
    nodes = 0;
    ply = 0;
    followPV = 0;
    scorePV = 0;

    memset(pvLength, 0, sizeof(pvLength));
    memset(pvTable, 0, sizeof(pvTable));

    // initialize alpha beta bounds
    int alpha = -infinity;
    int beta = infinity;

    // iterative deepening loop
    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        // enable follow PV line flag
        followPV = 1;

        // search for best move within position
        int score = negamax<c>(pos, -infinity, infinity, currentDepth);

        // get cumulative search time
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // adjust aspiration window technique
        if ((score <= alpha) || (score >= beta)) {
            alpha = -infinity;
            beta  = infinity;    
            currentDepth--;
            continue;
        }

        alpha = score - windowSize;
        beta = score + windowSize;

        // save current best move
        bestMove = &pvTable[0][0];

        // if PV is available (print search info)
        if (pvLength[0] > 0) {
            if (score > -checkmate && score < -(checkmate-100)) {
                std::cout << "info score mate " << -(score + checkmate) / 2 - 1 << " depth " << currentDepth;
                std::cout << " nodes " << nodes << " time " << ms.count() << " pv ";
            }
            else if (score > (checkmate-100) && score < checkmate) {
                std::cout << "info score mate " << (checkmate - score) / 2 + 1 << " depth " << currentDepth;
                std::cout << " nodes " << nodes << " time " << ms.count() << " pv ";
            }
            else {
                std::cout << "info score cp " << score << " depth " << currentDepth;
                std::cout << " nodes " << nodes << " time " << ms.count() << " pv ";
            }
        }

        // loop over pv line
        for (int i = 0; i < pvLength[0]; i++) {
            // print move
            Move pvMove = pvTable[0][i];
            if (pvMove.promoted()) {
                std::cout << pvMove.toUci() << promotedPieceToChar[pvMove.piece()] << " ";
            }
            else std::cout << pvMove.toUci() << " ";
        }
        std::cout << std::endl;

        // set previous score to current score
        //lastScore = score;
    }

    std::cout << "bestmove " << bestMove->toUci() << std::endl;
}


