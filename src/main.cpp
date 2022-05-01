#include <iostream>
#include "chess.hpp"
#include "evaluate.hpp"
#include "search.hpp"

using namespace Testing;

int main() {
    //PerftTesting perft;
    //perft.RunPerftTest();
    Position pos = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    pos.print();
    Search search;
    search.search<White>(pos, 4);
}