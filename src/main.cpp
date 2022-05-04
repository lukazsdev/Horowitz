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

    uci.pos.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ");
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