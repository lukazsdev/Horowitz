#pragma once

#include "chess.hpp"
#include "search.hpp"
#include "evaluate.hpp"

// search constants
static constexpr int maxPly = 64;
static constexpr int infinity = 10000;
static constexpr int checkmate = 9000;

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
    if (depth == 0) {
        return evaluate(pos);
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

template<Color c> 
void Search::search(Position pos, int depth) {
    // reset search info
    nodes = 0;
    ply = 0;
    followPV = 0;
    scorePV = 0;

    memset(pvLength, 0, sizeof(pvLength));
    memset(pvTable, 0, sizeof(pvTable));


    int score = negamax<c>(pos, -infinity, infinity, depth);
    Move bestMove = pvTable[0][0];

    // loop over pv line
    std::cout << "pv: ";
    for (int i = 0; i < pvLength[0]; i++) {
        // print move
        std::cout << pvTable[0][i].toUci() << " ";
    }
    std::cout << std::endl;

    std::cout << "bestmove " << bestMove.toUci() << std::endl;
}


