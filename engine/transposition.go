package main

const (
	// no hash entry found constant
	no_hash_entry = 100000

	// transposition entry size
	tt_entry_size = 12

	// transposition table hash flags
	hash_flag_exact = 0
	hash_flag_alpha = 1
	hash_flag_beta  = 2
)

// struct for a transposition table entry
type TTEntry struct {
	hash_key uint64
	depth    uint8
	flag     uint8
	score    int16
}

// struct for a transposition table
type TranspositionTable struct {
	entries []TTEntry
	size    uint64
}

// resize transposition table (allocate memory in MB)
func (TT* TranspositionTable) resize(mb uint64) {
	// initialize hash size
	size := (mb * 1024 * 1024) / tt_entry_size
	TT.entries = make([]TTEntry, size)
	TT.size = size
}

// clear the transposition table
func (TT* TranspositionTable) clear() {
	var idx uint64
	for idx = 0; idx < TT.size; idx++ {
		TT.entries[idx] = TTEntry{}
	}
}