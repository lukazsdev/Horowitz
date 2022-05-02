#include <sstream>
#include <thread>
#include "chess.h"
#include "search.h"

// class for UCI Protocol
class UCIInterface {
public:
    Position pos;
    Search search;

    std::istringstream iss;
    std::thread threads;

public:
    void beginThread(int depth);
    void stopThread();
    bool isSearching();

public:
    void UCILoop();
    void bootEngine();
    void parseGoCommand();
    Move parseMove(std::string move);
};

