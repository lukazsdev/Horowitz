package main

const (
    // hash table size (~20MB)
    hash_size = 800000

    // no hash entry found constant
    no_hash_entry = 20000

    // transposition table hash flags
    hash_flag_exact uint8 = 0
    hash_flag_alpha uint8 = 1
    hash_flag_beta  uint8 = 2
)

// struct for a transposition table entry
type TTEntry struct {
    hash_key uint64
    score    uint64
    depth    uint8
    flag     uint8
}

// struct for a transposition table
type TranspositionTable struct {
    entries []TTEntry
    size    uint64
}

// resize transposition table (allocate memory in MB)
func (TT* TranspositionTable) init() {
    TT.size = hash_size
    TT.entries = make([]TTEntry, hash_size)
}

// clear the transposition table
func (TT* TranspositionTable) clear() {
    var idx uint64
    for idx = 0; idx < TT.size; idx++ {
        TT.entries[idx] = TTEntry{}
    }
}

// store an entry in transposition table
func  (TT* TranspositionTable) store(hash_key uint64, depth, flag uint8, score, ply int) {
    entry := &TT.entries[hash_key % TT.size]
    entry.hash_key = hash_key
    entry.depth = depth
    entry.flag = flag

    // store score independent from the actual path
    // from root node (position) to current node (position)

    if score < -checkmate {
        score -= ply
    }

    if score > checkmate {
        score += ply
    }

    entry.score = uint64(score)
}

func (TT* TranspositionTable) read(hash_key uint64, alpha, beta, ply int, depth uint8) int {
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    entry := TT.entries[hash_key % TT.size]

    // make sure we're dealing with exact position we need
    if entry.hash_key == hash_key {
        // make sure we're are dealing with the exact depth search is currently at
        if entry.depth >= depth {
            // extract stored score from TT entry
            score := int(entry.score)

            // retrieve score independent from the actual path
            // from root node (position) to current node (position)
            if score < -checkmate {
                score += ply
            }

            if score > checkmate {
                score -= ply
            }

             // match the exact (PV node) score 
             if entry.flag == hash_flag_exact {
                 // return exact (PV node) score
                 return score
             }

             // match alpha (fail-low node) score
             if (entry.flag == hash_flag_alpha) && (score <= alpha) {
                // return alpha (fail-low node) score
                return alpha
            }
            
            // match beta (fail-high node) score
            if (entry.flag == hash_flag_beta) && (score >= beta) {
                // return beta (fail-high node) score
                return beta
            }
        }

    }

    return no_hash_entry

}