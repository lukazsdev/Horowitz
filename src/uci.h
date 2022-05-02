#include "chess.h"
#include "search.h"

class UCIInterface {
public:
    Position pos;
    Search search;

public:
    void UCILoop();
    void bootEngine();
    Move parseMove(std::string move);
};

