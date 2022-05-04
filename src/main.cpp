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
    printf("%llx\n", uci.pos.hashKey);
}