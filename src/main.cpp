#include <iostream>
#include "chess.hpp"
#include "evaluate.hpp"
#include "search.hpp"

using namespace Testing;

int main() {
    //PerftTesting perft;
    //perft.RunPerftTest();
    Position pos = Position();
    pos.print();
    Search search;
    search.search<White>(pos, 4);
}