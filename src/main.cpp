#include <iostream>
#include "chess.hpp"
#include "evaluate.hpp"
#include "search.hpp"

using namespace Testing;

int main() {
    //PerftTesting perft;
    //perft.RunPerftTest();
    Position pos = Position("r2k1b1r/p1ppq2p/np3np1/5p2/1PPP4/P3PQ2/3N1PPP/R1B1K2R w KQ - 1 13");
    pos.print();
    Search search;
    search.search<White>(pos, 4);
}