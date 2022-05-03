#include "chess.h"

// class holding random numbers
// for zobrist hashing
class Zobrist {
public:
    uint32_t randomState;

    uint64_t pieceKeys[12][64];
    uint64_t enpassantKeys[64];
    uint64_t castleKeys[16];
    uint64_t sideKey;

public:
    Zobrist() : randomState(1804289383) {};
    void initRandomKeys();
    uint64_t generateHashKey(Position pos);

private:
    uint64_t random64();
    uint32_t random32();
};

// zobrist instance
extern Zobrist zobrist;

