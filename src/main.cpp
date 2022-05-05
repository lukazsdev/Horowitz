#include <iostream>
#include <thread>
#include "chess.h"
#include "evaluate.h"
#include "search.h"
#include "uci.h"
#include "zobrist.h"

using namespace Testing;

int main() {
    //PerftTesting perft;
    //perft.RunPerftTest();
    UCIInterface uci;
    //uci.UCILoop();
    uci.bootEngine();

    uci.pos.parseFEN("rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1");
    uci.pos.print();
    printf("%llx\n", uci.pos.hashKey);
    //uci.pos.updateZobristPiece(WhitePawn, SQ_A2);
    //uci.pos.updateZobristPiece(WhitePawn, SQ_A3);
    //uci.pos.updateZobristSideToMove();
    //printf("%llx\n", uci.pos.hashKey);
    
    Moves moveList = uci.pos.generateLegalMoves<White>();
    for (int i = 0; i < moveList.count; i++) {
        Move move = moveList.moves[i];
        uci.pos.makemove<White>(move);
        //std::cout << "\n\n";
        //printf("%llx\n", uci.pos.hashKey);
        uci.pos.unmakemove<White>(move);
        //printf("%llx\n", uci.pos.hashKey);
        //std::cout << "\n\n";
    }
    
    
}