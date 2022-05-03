#include "tt.h"

// clear TT and initialize size
void TranspositionTable::Init() {
    size = HashTableSize;
    Clear();
}

// clear transposition table
void TranspositionTable::Clear() {
    for (int idx = 0; idx < size; idx++) {
        entries[idx].hashKey  = 0;
        entries[idx].score    = 0;
        entries[idx].depth    = 0;
        entries[idx].flag     = 0;
    }
}

// store an entry in the transposition table
void TranspositionTable::Store(uint64_t hashKey, uint8_t depth, uint8_t flag, int score, int ply) {
    int index = hashKey % size;
    entries[index].hashKey = hashKey;
    entries[index].depth   = depth;
    entries[index].flag    = flag;

    // store score independent from the actual path
    // from root node (position) to current node (position)
    if (score < -checkmate) 
        score -= ply;
    
    if (score > checkmate) 
        score += ply;

    entries[index].score = (uint64_t)score;
}

// read an entry from the transposition table
int TranspositionTable::Read(uint64_t hashKey, int alpha, int beta, int ply, uint8_t depth) {
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    TTEntry entry = entries[hashKey % size];

    // make sure we're dealing with exact position we need
    if (entry.hashKey == hashKey) {
        // make sure we're are dealing with the exact depth search is currently at
        if (entry.depth >= depth) {
            // extract stored score from TT entry
            int score = (int)entry.score;
            
            // retrieve score independent from the actual path
            // from root node (position) to current node (position)
            if (score < -checkmate) 
                score += ply;
            
            if (score > checkmate) 
                score -= ply;
            
             // match the exact (PV node) score 
             if (entry.flag == HashFlagExact) 
                 // return exact (PV node) score
                 return score;
             
             // match alpha (fail-low node) score
             if ((entry.flag == HashFlagAlpha) && (score <= alpha)) 
                // return alpha (fail-low node) score
                return alpha;
             
            // match beta (fail-high node) score
            if ((entry.flag == HashFlagBeta) && (score >= beta)) 
                // return beta (fail-high node) score
                return beta;
            
        }
    }

    return NoHashEntry;
}