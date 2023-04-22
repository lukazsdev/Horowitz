#pragma once

#include <chrono>
#include "chess.h"
#include "search.h"
#include "evaluate.h"
#include "timemanager.h"
#include "see.h"
#include "tt.h"

// search constants and pruning parameters
static constexpr int maxPly          = 64;
static constexpr int windowSize      = 50;
static constexpr int fullDepthMoves  = 4;
static constexpr int reductionLimit  = 3;
static constexpr int staticNMPMargin = 120;

static constexpr int pvMoveScore     = 60;
static constexpr int killerMoveScore = 10;
static constexpr int maxKillers      = 2;
static constexpr int mvvLvaOffset    = 10000;
static constexpr int maxHistoryScore = 10000;


// margins for futility pruning and late move pruning
static constexpr int futilityMargins[9] = {0, 100, 160, 220, 280, 340, 400, 460, 520};
static constexpr int lateMovePruningMargins[4] = {0, 8, 12, 24};

struct Reduction {
    int moveLimit;
    int reduction;
};

static constexpr Reduction LateMoveReductions[10] = {
    {4, 2},
    {8, 3},
    {12, 4},
    {16, 5},
    {20, 6},
    {24, 7},
    {28, 8},
    {32, 9},
    {34, 10},
    {100, 12}
};

// MVV LVA [victim][attacker]
static constexpr int MvvLva[7][6] = {
    {15, 14, 13, 12, 11, 10}, 
    {25, 24, 23, 22, 21, 20}, 
    {35, 34, 33, 32, 31, 30},
    {45, 44, 43, 42, 41, 40}, 
    {55, 54, 53, 52, 51, 50}, 

    {0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0} 
};

// class for detecting repetitions
class Repetition {
public:
    uint64_t repetitionsTable[1024]{};
    int count{};

    // add a hash key (position) to table
    void Add(uint64_t hash);
    // check whether the current position has ocurred
    bool isRepetition(Position& pos);
    // reset repetitions table and reset count
    void Reset();
};


class Search {
public:
    Position pos;
    TranspositionTable TT;
    TimeManager timer;
    Repetition repetitions;
    uint64_t nodes;
    int ply;

    int  pvLength[maxPly];
    Move pvTable[maxPly][maxPly];

    int  history[2][64][64];
    Move killers[maxPly + 1][2];

    int followPV, scorePV;

public:
    // main search functions
    template<Color c> void search(int depth);
    template<Color c> int quiescence(int alpha, int beta);
    template<Color c> int negamax(int alpha, int beta, int depth, bool nmp=true);

    // move ordering/scoring functions
    void scoreMoves(Moves& moveList);
    void enablePVScoring(Moves& moveList);
    void orderMoves(Moves& moveList, int currIndex);
    void ageHistoryTable();
    
    // print the best move
    void printBestMove(Move bestMove);
};


// Quiescence search
template<Color c> 
int Search::quiescence(int alpha, int beta) {
    // every 2048 nodes, check if time is up
    if ((nodes & 2047) == 0 )
        timer.Check();
    
    // stop search if time is up
    if (timer.Stop)
        return 0;

    // static evaluation
    int bestScore = Eval::evaluate<c>(pos);

    // stop if depth exceeds ply limit
    if (ply > maxPly - 1)
        return bestScore;
    
    if (bestScore >= beta)
        return bestScore;
    
    if (bestScore > alpha)
        alpha = bestScore;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // assign score to each move
    scoreMoves(moveList);

    // iterate over legal moves
    for (int i = 0; i < moveList.count; i++) {
        // order moves
        orderMoves(moveList, i);

        // initialize current move
        Move move = moveList.moves[i];

        // check if move isn't a capture
        if (pos.board[move.target()] == None) 
            continue; 

        // SEE (static exchange evaluator)
		if (See(pos, move) < 0) 
            continue;

        // increment ply
        ply++;

        // add current position to repetitions table
        repetitions.Add(pos.hashKey);

        // increment nodes
        nodes++;

        // make move
        pos.makemove<c>(move);

        // recursively call negamax
        int score = -quiescence<~c>(-beta, -alpha);

        // unmake move
        pos.unmakemove<c>(move);

        // decrement ply
        ply--;

        // decrement repetitions index
        repetitions.count--;

        // update best score
        if (score > bestScore) 
            bestScore = score;

        // fail-hard beta cutoff
        if (score >= beta) 
            return beta;
        

        // found a better move
        if (score > alpha) 
            // PV node (move)
            alpha = score;
        
    }

    return bestScore;
 
}

// Negamax search
template<Color c> 
int Search::negamax(int alpha, int beta, int depth, bool nmp) {
    // score of current position
    int score = 0;

    // initialize pv length
    pvLength[ply] = ply;

    // if depth limit is greater than max ply.
    // exit if current ply is greater than max ply
    if (ply >= maxPly) {
        return Eval::evaluate<c>(pos);
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
    

    // search/pruning conditions
    bool inCheck = pos.isSquareAttacked<~c>(pos.KingSq<c>());
    bool isPVNode = beta - alpha > 1;
    bool canFutilityPrune = false;

    // increment depth if king is in check
    if (inCheck)
        depth++;

    // check if we have reached the depth limit
    // then search all possible captures 
    if (depth <= 0) {
        return quiescence<c>(alpha, beta);
    }

    
    // return 0 (draw) if repetition occurs
    if (ply > 0 && repetitions.isRepetition(pos)) 
        return 0;
    

    // read hash entry if we're not in a root ply and hash entry is available
    // and current node is not a PV node
    score = TT.Read(pos.hashKey, alpha, beta, ply, (uint8_t)depth);
    if (ply > 0 && score != NoHashEntry && !isPVNode)
        // if the move has already been searched (hence has a value)
        // we just return the score for this move without searching it
        return score;
    

    // static null move pruning
    if (!inCheck && !isPVNode && abs(beta) < checkmate) {
        // if current material - score margin is still good, prune branch
        int staticScore = Eval::evaluate<c>(pos);
        int scoreMargin = staticNMPMargin * depth;
        
        if (staticScore - scoreMargin >= beta)
            return beta;
    }
    
    // null move pruning (only done if we don't have non pawn material)
    if (depth >= 3 && !inCheck && ply > 0 && pos.hasNonPawnMaterial() && nmp) {
        // reduced depth
        int R = 3 + depth / 6;

        // make null move
        pos.makemove<c>(nullMove);

        // increment ply
        ply++;

        // add current position to repetitions table
        repetitions.Add(pos.hashKey);

        // hash enpassant if available
        if (pos.enpassantSquare != NO_SQ)
            pos.updateZobristEnpassant(pos.enpassantSquare);
        
        // reset enpassant square
        pos.enpassantSquare = NO_SQ;

        // switch the side (give opponent extra move)
        pos.sideToMove = ~pos.sideToMove;

        // hash the side to move
        pos.updateZobristSideToMove();

        // search moves with reduced depth to find beta cutoffs 
        score = -negamax<~c>(-beta, -beta + 1, depth - R - 1, false);

        // unmake null move (restore board state)
        pos.unmakemove<c>(nullMove); 

        // switch the side (give opponent extra move)
        pos.sideToMove = ~pos.sideToMove;

        // decrement ply
        ply--;

        // decrement repetitions index
        repetitions.count--;

        // stop search if time is up
        if (timer.Stop) 
            return 0;
        
        // fail-hard beta cutoff
        if (score >= beta) 
            // node (position) fails high
            return beta;
    }

    // futility pruning
    if (depth <= 8 && !isPVNode && !inCheck && alpha < checkmate) {
        int staticScore = Eval::evaluate<c>(pos);
        if (staticScore + futilityMargins[depth] <= alpha) {
            canFutilityPrune = true;
        }
    }
    

    // legal moves counter
    int legalMoves = 0;

    // legal moves list
    Moves moveList = pos.generateLegalMoves<c>();

    // if we are following PV line
    if (followPV == 1) 
        // enable PV move scoring
        enablePVScoring(moveList);

    // assign score to each move
    scoreMoves(moveList);

    // number of moves searched in the move list
    int movesSearched = 0;

    // best score
    int bestScore = -infinity;

    // iterate over legal moves
    for (int i = 0; i < moveList.count; i++) {
        // order moves
        orderMoves(moveList, i);

        // initialize current move
        Move move = moveList.moves[i];

        // check if move is a capture
        bool isCapture = pos.board[move.target()] != None;

        // increment ply
        ply++;

        // add current position to repetitions table
        repetitions.Add(pos.hashKey);

        // increment nodes
        nodes++;

        // make move
        pos.makemove<c>(move);

        // increment legal moves counter
        legalMoves++;

        // late move pruning
        if (depth <= 3 && !isPVNode && !inCheck && legalMoves > lateMovePruningMargins[depth]) {
            bool tactical = inCheck || (move.promoted() > 0);
            if (!tactical) {
                pos.unmakemove<c>(move);
                repetitions.count--;
                ply--;
                continue;
            }
        }

        // futility pruning
        if (canFutilityPrune && legalMoves > 1) {
            bool tactical = (inCheck) || (isCapture) || (move.promoted() > 0);
            if (!tactical) {
                pos.unmakemove<c>(move);
                repetitions.count--;
                ply--;
                continue;
            }
        }

        // late move reductions
        score = 0;

        if (legalMoves == 1) {
            // peform full-depth search
            score = -negamax<~c>(-beta, -alpha, depth - 1);
        }
        else {
            bool tactical = inCheck || isCapture ||
                killers[ply][0] == move ||
                killers[ply][1] == move ||
                move.promoted();
            
            int reduction = 0;

            if (!isPVNode && legalMoves >= fullDepthMoves && depth >= reductionLimit && !tactical) {
                for (int index = 0; index < 10; index++) {
                    Reduction red = LateMoveReductions[index];
                    if (red.moveLimit >= legalMoves) {
                        reduction = red.reduction;
                        break;
                    }
                }
            }

            int reducedDepth = std::max(depth - 1 - reduction, 1);
            if (depth == 1) {
                reducedDepth = depth - 1;
            }

            score = -negamax<~c>(-alpha - 1, -alpha, reducedDepth);

            if (score > alpha && reduction > 0) {
                score = -negamax<~c>(-beta, -alpha, reducedDepth);
                if (score > alpha) {
                    score = -negamax<~c>(-beta, -alpha, depth - 1);
                }
            }
            else if (score > alpha && score < beta) {
                score = -negamax<~c>(-beta, -alpha, depth - 1);
            }
        }

        // unmake move
        pos.unmakemove<c>(move);

        // decrement ply
        ply--;

        // decrement repetitions index
        repetitions.count--;

        // increment moves searched
        movesSearched++;

        // update best score
        if (score > bestScore)
            bestScore = score;

        // fail-hard beta cutoff
        if (score >= beta) {
            // switch hash flag to beta flag
            hashFlag = HashFlagBeta;

            // store killer moves
            if (pos.board[move.target()] == None) {
                if (!(move == killers[ply][0])) {
                    killers[ply][1] = killers[ply][0];
                    killers[ply][0] = move;
                }
            }

            // update history table
            if (pos.board[move.target()] == None) {
                history[pos.sideToMove][move.source()][move.target()] += depth * depth;
            }
            if (history[pos.sideToMove][move.source()][move.target()] >= maxHistoryScore) {
                ageHistoryTable();
            }

            break; 

        } else {

            // decrement history score
            if (pos.board[move.target()] == None) {
                if (history[pos.sideToMove][move.source()][move.target()] > 0) {
                    history[pos.sideToMove][move.source()][move.target()] -= 1;
                }
            }
        }

        // found a better move
        if (score > alpha) {
            // switch hash flag to exact flag
            hashFlag = HashFlagExact;

            // update history table
            if (pos.board[move.target()] == None) {
                history[pos.sideToMove][move.source()][move.target()] += depth * depth;
            }
            if (history[pos.sideToMove][move.source()][move.target()] >= maxHistoryScore) {
                ageHistoryTable();
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

        } else {

            // decrement history score
            if (pos.board[move.target()] == None) {
                if (history[pos.sideToMove][move.source()][move.target()] > 0) {
                    history[pos.sideToMove][move.source()][move.target()] -= 1;
                }
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

    // store hash entry with the score equal to best score
    TT.Store(pos.hashKey, (uint8_t)depth, hashFlag, bestScore, ply);

    // node (move) fails low
    return bestScore;
}

// root search function (iterative deepening search)
template<Color c> 
void Search::search(int depth) {
    // start search timer
    auto t1 = std::chrono::high_resolution_clock::now();

    // start time manager timer
    timer.Start();

    // initialize best move to null (no move)
    Move bestMove = nullMove;

    // previous best move
    Move prevBestMove = nullMove;

    // last iteration score
    int lastScore = 0;

    // reset search info
    nodes    = 0;
    ply      = 0;
    followPV = 0;
    scorePV  = 0;

    memset(pvLength, 0, sizeof(pvLength));
    memset(pvTable, 0, sizeof(pvTable));

    ageHistoryTable();

    // initialize alpha beta bounds
    int alpha = -infinity;
    int beta = infinity;

    // iterative deepening loop
    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        // enable follow PV line flag
        followPV = 1;

        // search for best move within position
        int score = negamax<c>(alpha, beta, currentDepth);

        // get cumulative search time
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // stop if time is up and return best move
        if (timer.Stop) {
            if (bestMove == nullMove && currentDepth == 1) 
                bestMove = pvTable[0][0];
            else 
                bestMove = prevBestMove;
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
                std::cout << " nodes " << nodes;
                std::cout << " nps "   << (nodes / (ms.count() + 1)) * 1000;
                std::cout << " time "  << ms.count();
                std::cout << " pv ";
                printBestMove(bestMove);
                break;
            }
            else if (score > (checkmate-100) && score < checkmate) {
                std::cout << "info score mate " << (checkmate - score) / 2 + 1 << " depth " << currentDepth;
                std::cout << " nodes " << nodes;
                std::cout << " nps "   << (nodes / (ms.count() + 1)) * 1000;
                std::cout << " time "  << ms.count();
                std::cout << " pv ";
                printBestMove(bestMove);
                break;
            }
            else {
                std::cout << "info score cp " << score << " depth " << currentDepth;
                std::cout << " nodes " << nodes;
                std::cout << " nps "   << (nodes / (ms.count() + 1)) * 1000;
                std::cout << " time "  << ms.count();
                std::cout << " pv ";
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

        // set previous best move to current best move
        prevBestMove = bestMove;
    }

    // print best move
    printBestMove(bestMove);
}
