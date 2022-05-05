#pragma once

#include <chrono>
#include "chess.h"
#include "search.h"
#include "evaluate.h"
#include "timemanager.h"
#include "tt.h"

// search constants and pruning parameters
static constexpr int maxPly         = 64;
static constexpr int windowSize     = 50;
static constexpr int fullDepthMoves = 4;
static constexpr int reductionLimit = 3;

// null move (no move)
#define nullMove Move(NO_SQ, NO_SQ, Nonetype, 0)

// MVV LVA [attacker][victim]
static constexpr int MVV_LVA[12][12] = {
    {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

    {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
};

class Search {
public:
    TranspositionTable TT;
    TimeManager timer;
    uint64_t nodes;
    int ply;

    int  pvLength[maxPly];
    Move pvTable[maxPly][maxPly];

    int  history[12][64];
    Move killers[2][maxPly];

    int followPV, scorePV;

public:
    // main search functions
    template<Color c> void search(Position pos, int depth);
    template<Color c> int quiescence(Position pos, int alpha, int beta);
    template<Color c> int negamax(Position pos, int alpha, int beta, int depth);

    // move ordering/scoring functions
    int scoreMove(Position pos, Move move);
    void sortMoves(Position pos, Moves &moveList);
    void enablePVScoring(Moves moveList);
};

// Quiescence search
template<Color c> 
int Search::quiescence(Position pos, int alpha, int beta) {
    // static evaluation
    int evaluation = evaluate(pos);

    nodes++;

    if (ply > maxPly - 1)
        return evaluation;

    // every 2048 nodes, check if time is up
    if ((nodes & 2047) == 0 )
        timer.Check();
    
    // stop search if time is up
    if (timer.Stop)
        return 0;
    
    if (evaluation >= beta)
        return beta;

    if (evaluation > alpha)
        alpha = evaluation;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // sort moves
    sortMoves(pos, moveList);

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

// Negamax search
template<Color c> 
int Search::negamax(Position pos, int alpha, int beta, int depth) {
    // score of current position
    int score = 0;

    // increment nodes
    nodes++;

    // initialize pv length
    pvLength[ply] = ply;

    // if depth limit is greater than max ply.
    // exit if current ply is greater than max ply
    if (ply >= maxPly) {
        return evaluate(pos);
    }

    // every 2048 nodes, check if time is up
    if ((nodes & 2047) == 0)
        timer.Check();
    
    // stop search if time is up
    if (timer.Stop) {
        return 0;
    }

    // define hash flag
    int hashFlag = HashFlagAlpha;
    

    // check if king is in check 
    // check if current node is PV node 
    bool inCheck = pos.isSquareAttacked<~c>(pos.KingSq<c>());
    bool isPVNode = beta - alpha > 1;

    // increment depth if king is in check
    if (inCheck)
        depth++;

    // check if we have reached the depth limit
    // then search all possible captures 
    if (depth == 0) {
        //return evaluate(pos);
        return quiescence<c>(pos, alpha, beta);
    }

    // read hash entry if we're not in a root ply and hash entry is available
    // and current node is not a PV node
    score = TT.Read(pos.hashKey, alpha, beta, ply, (uint8_t)depth);
    if (ply > 0 && score != NoHashEntry && !isPVNode)
        // if the move has already been searched (hence has a value)
        // we just return the score for this move without searching it
        return score;
    

    // legal moves counter
    int legalMoves = 0;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // if we are following PV line
    if (followPV == 1) 
        // enable PV move scoring
        enablePVScoring(moveList);

    // sort moves
    sortMoves(pos, moveList);

    // number of moves searched in the move list
    int movesSearched = 0;

    // iterate over legal moves
    for (int i = 0; i < moveList.count; i++) {
        // initialize current move
        Move move = moveList.moves[i];

        // check if move is a capture
        bool isCapture = pos.board[move.target()] != None;

        // increment ply
        ply++;

        // make move
        pos.makemove<c>(move);

        // increment legal moves counter
        legalMoves++;

        // recursively call negamax
        //score = -negamax<~c>(pos, -beta, -alpha, depth - 1);

        /*
        // full depth search
        if moves_searched == 0 {
            // recursively call negamax normally
            score = -search.negamax(pos, -beta, -alpha, depth - 1)
        } else {
            // condition to consider LMR
            if moves_searched >= full_depth_moves && 
                depth >= reduction_limit && in_check == false &&
                 move.get_move_capture() == 0 && move.get_move_promoted() == 0 {
                // search current move with reduced depth
                score = -search.negamax(pos, -alpha - 1, -alpha, depth - 2)
            } else {
                // hack to ensure that full-depth search is done
                score = alpha + 1
            }

            // PV search
            if score > alpha {
                score = -search.negamax(pos, -alpha - 1, -alpha, depth - 1)
                // check for failure
                if (score > alpha) && (score < beta) {
                    score = -search.negamax(pos, -beta, -alpha, depth - 1)
                }
            }
        }
        */

       // full depth search
        if (movesSearched == 0) 
            // recursively call negamax normally
            score = -negamax<~c>(pos, -beta, -alpha, depth - 1);
        else {
            // condition to consider LMR
            if (movesSearched >= fullDepthMoves && depth >= reductionLimit &&
            !inCheck && !isCapture && !move.promoted()) {
                // search current move with reduced depth
                score = -negamax<~c>(pos, -alpha - 1, -alpha, depth - 2);
            }
            else 
                // hack to ensure that full-depth search is done
                score = alpha + 1;
            
            // PV search
            if (score > alpha) {
                score = -negamax<~c>(pos, -alpha - 1, -alpha, depth - 1);
                // check for failure
                if ((score > alpha) && score < beta) {
                    score = -negamax<~c>(pos, -beta, -alpha, depth - 1);
                }
            }
        }


        // unmake move
        pos.unmakemove<c>(move);

        // decrement ply
        ply--;

        // increment moves searched
        movesSearched++;

        // found a better move
        if (score > alpha) {
            // switch hash flag from storing score of fail-low node
            // to the one storing score for PV node
            hashFlag = HashFlagExact;

            // only quiet moves 
            if (pos.board[move.target()] == None) {
                // store history moves
                history[makePiece(pos.sideToMove, move.piece())][move.target()] += depth;
            }

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

            // fail-hard beta cutoff
            if (score >= beta) {
                // store hash entry with the score equal to beta
                TT.Store(pos.hashKey, (uint8_t)depth, HashFlagBeta, beta, ply);

                // only quiet moves
                if (pos.board[move.target()] == None) {
                    // store killer moves
                    killers[1][ply] = killers[0][ply];
                    killers[0][ply] = move;
                }

                return beta;
            }
        }
    }
 
    // no legal moves
    if (legalMoves == 0) {
        // checkmate
        if (inCheck) 
            return -checkmate + ply;
        // if not, then stalemate
        else
            return 0;
    }

    // store hash entry with the score equal to alpha
    TT.Store(pos.hashKey, (uint8_t)depth, hashFlag, alpha, ply);

    // node (move) fails low
    return alpha;
}

// root search function (iterative deepening search)
template<Color c> 
void Search::search(Position pos, int depth) {
    // start search timer
    auto t1 = std::chrono::high_resolution_clock::now();

    // start time manager timer
    timer.Start();

    // initialize best move to null (no move)
    Move bestMove = nullMove;

    // last iteration score
    int lastScore = 0;

    // reset search info
    nodes    = 0;
    ply      = 0;
    followPV = 0;
    scorePV  = 0;

    memset(pvLength, 0, sizeof(pvLength));
    memset(pvTable, 0, sizeof(pvTable));
    memset(killers, 0, sizeof(killers));
    memset(history, 0, sizeof(history));

    // initialize alpha beta bounds
    int alpha = -infinity;
    int beta = infinity;

    // iterative deepening loop
    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        // enable follow PV line flag
        followPV = 1;

        // search for best move within position
        int score = negamax<c>(pos, alpha, beta, currentDepth);

        // get cumulative search time
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // stop if time is up and return best move
        if (timer.Stop) {
            if (bestMove == nullMove && currentDepth == 1) 
                bestMove = pvTable[0][0];
            break;
        }
        

        // adjust aspiration window technique
        if ((score <= alpha) || (score >= beta)) {
            alpha = -infinity;
            beta  = infinity;    
            currentDepth--;
            continue;
        }

        alpha = score - windowSize;
        beta = score + windowSize;


        // If the score between this current iteration and the last iteration drops,
        // take more time on the current search to make sure we find the best move.
        if (currentDepth > 1 && lastScore > score && lastScore-score >= 30) 
            timer.setSoftTimeForMove(timer.softTimeForMove * 13 / 10);

        // save current best move
        bestMove = pvTable[0][0];

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
        lastScore = score;
    }

    std::cout << "bestmove " << bestMove.toUci() << std::endl;
}


