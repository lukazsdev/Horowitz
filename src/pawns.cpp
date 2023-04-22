#include "pawns.h"

void PawnHashTable::Clear() {
    // init hash table entry pointer
    Entry *hashEntry;

    // loop over TT elements
    for (hashEntry = entries; hashEntry < entries + hashTableEntries; hashEntry++)
    {
        // reset TT inner fields
        hashEntry->hashKey = 0;
        memset(hashEntry->MGScores, 0, sizeof(hashEntry->MGScores));
        memset(hashEntry->EGScores, 0, sizeof(hashEntry->EGScores));
    }
}

// dynamically allocate memory for hash table
void PawnHashTable::Init(int sizeInMB) {
    // init hash size
    int hashSize = 0x100000 * sizeInMB;

    // init number of hash entries
    hashTableEntries = hashSize / sizeof(Entry);

    // free hash table if not empty
    if (entries != NULL) {
        printf("    Clearing hash memory...\n");
          
        // free hash table dynamic memory
        free(entries);
    }

    // allocate memory
    entries = (Entry *) malloc(hashTableEntries * sizeof(Entry));

    // if allocation has failed
    if (entries == NULL)
    {
        printf("    Couldn't allocate memory for hash table, trying %dMB...", sizeInMB / 2);
        
        // try to allocate with half size
        Init(sizeInMB / 2);
    }

    // if allocation succeeded, clear hash table
    else Clear();
}

void PawnHashTable::Store(uint64_t hashKey, int mg_scores[2], int eg_scores[2]) {
    // create a pawn entry instance pointer to particular hash entry storing
    // the scoring data for the current pawn structure if available
    Entry *hashEntry = &entries[hashKey % hashTableEntries];

    // store hash entry data in hash table
    hashEntry->hashKey = hashKey;
    hashEntry->MGScores[0] = mg_scores[0];
    hashEntry->MGScores[1] = mg_scores[1];
    hashEntry->EGScores[0] = eg_scores[0];
    hashEntry->EGScores[1] = eg_scores[1];
}

int PawnHashTable::Read(uint64_t hashKey, Color color, Phase phase) {
    // create a pawn entry instance pointer to particular hash entry storing
    // the scoring data for the current pawn structure if available
    Entry *hashEntry = &entries[hashKey % hashTableEntries];

    // make sure we're dealing with exact position we need
    if (hashEntry->hashKey == hashKey) {
        // return hash entry score since pawn structure matches
        if (color == White) {
            return (phase == MG) ? hashEntry->MGScores[White] : 
                    hashEntry->EGScores[White];
        }
        else return (phase == MG) ? hashEntry->MGScores[Black] : 
                    hashEntry->EGScores[Black];
    }

    // return no entry if hashes don't match
    return NoHashEntry;
}