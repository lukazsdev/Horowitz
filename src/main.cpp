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
    uci.bootEngine();
    //uci.UCILoop();

    Position pos = Position();
    printf("%llx\n", zobrist.generateHashKey(pos));
    pos.enpassantSquare = SQ_E4;
    printf("%llx\n", zobrist.generateHashKey(pos));
}