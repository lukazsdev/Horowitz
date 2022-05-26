/*
  Horowitz, a UCI compatible chess engine. 
  Copyright (C) 2022 by OliveriQ.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pawns.h"

void PawnHashTable::Clear() {
    // init hash table entry pointer
    Entry *hashEntry;

    // loop over TT elements
    for (hashEntry = entries; hashEntry < entries + hashTableEntries; hashEntry++)
    {
        // reset TT inner fields
        hashEntry->hashKey = 0;
        hashEntry->score   = 0;
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



// store an entry in the transposition table
void PawnHashTable::Store(uint64_t hashKey, int score) {
    // create a pawn entry instance pointer to particular hash entry storing
    // the scoring data for the current pawn structure if available
    Entry *hashEntry = &entries[hashKey % hashTableEntries];

    // store hash entry data in hash table
    hashEntry->hashKey = hashKey;
    hashEntry->score = (uint64_t)score;
}

// read an entry from the transposition table
int PawnHashTable::Read(uint64_t hashKey) {
    // create a pawn entry instance pointer to particular hash entry storing
    // the scoring data for the current pawn structure if available
    Entry *hashEntry = &entries[hashKey % hashTableEntries];

    // make sure we're dealing with exact position we need
    if (hashEntry->hashKey == hashKey) {
        // return hash entry score since pawn structure matches
        return hashEntry->score;
    }

    // return no entry if hashes don't match
    return NoHashEntry;
}