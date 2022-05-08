#include "tt.h"

void TranspositionTable::Clear() {
    // init hash table entry pointer
    TTEntry *hashEntry;

    // loop over TT elements
    for (hashEntry = entries; hashEntry < entries + hashTableEntries; hashEntry++)
    {
        // reset TT inner fields
        hashEntry->hashKey = 0;
        hashEntry->depth   = 0;
        hashEntry->flag    = 0;
        hashEntry->score   = 0;
    }
}

// dynamically allocate memory for hash table
void TranspositionTable::Init(int sizeInMB) {
    // init hash size
    int hashSize = 0x100000 * sizeInMB;

    // init number of hash entries
    hashTableEntries = hashSize / sizeof(TTEntry);

    // free hash table if not empty
    if (entries != NULL) {
        printf("    Clearing hash memory...\n");
          
        // free hash table dynamic memory
        free(entries);
    }

    // allocate memory
    entries = (TTEntry *) malloc(hashTableEntries * sizeof(TTEntry));

    // if allocation has failed
    if (entries == NULL)
    {
        printf("    Couldn't allocate memory for hash table, trying %dMB...", sizeInMB / 2);
        
        // try to allocate with half size
        Init(sizeInMB / 2);
    }

    // if allocation succeeded
    else
    {
        // clear hash table
        Clear();
        
        printf("Hash table is initialized with %d entries\n", hashTableEntries);
    }
}



// store an entry in the transposition table
void TranspositionTable::Store(uint64_t hashKey, uint8_t depth, uint8_t flag, int score, int ply) {
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    TTEntry *hashEntry = &entries[hashKey % hashTableEntries];

    // store hash entry data in TT
    hashEntry->hashKey = hashKey;
    hashEntry->depth   = depth;
    hashEntry->flag    = flag;

    // store score independent from the actual path
    // from root node (position) to current node (position)
    if (score < -checkmate) 
        score -= ply;
    
    if (score > checkmate) 
        score += ply;

    hashEntry->score = (uint64_t)score;
}

// read an entry from the transposition table
int TranspositionTable::Read(uint64_t hashKey, int alpha, int beta, int ply, uint8_t depth) {
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    TTEntry *hashEntry = &entries[hashKey % hashTableEntries];

    // make sure we're dealing with exact position we need
    if (hashEntry->hashKey == hashKey) {
        // make sure we're are dealing with the exact depth search is currently at
        if (hashEntry->depth >= depth) {
            // extract stored score from TT entry
            int score = (int)hashEntry->score;
            
            // retrieve score independent from the actual path
            // from root node (position) to current node (position)
            if (score < -checkmate) 
                score += ply;
            
            if (score > checkmate) 
                score -= ply;
            
             // match the exact (PV node) score 
             if (hashEntry->flag == HashFlagExact) 
                 // return exact (PV node) score
                 return score;
             
             // match alpha (fail-low node) score
             if ((hashEntry->flag == HashFlagAlpha) && (score <= alpha))  
                // return alpha (fail-low node) score
                return alpha;
             
            // match beta (fail-high node) score
            if ((hashEntry->flag == HashFlagBeta) && (score >= beta)) 
                // return beta (fail-high node) score
                return beta;
            
        }
    }

    return NoHashEntry;
}