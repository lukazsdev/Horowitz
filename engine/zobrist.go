package main

//import "fmt"

// Zobrist instance
var Zob Zobrist

// struct with random numbers for Zobrist hashing
type Zobrist struct {
	piece_keys [12][64]uint64
	enpassant_keys [64]uint64
	castle_keys    [16]uint64
	side_key           uint64
}

// generate almost unique hash for given position
func (Zob *Zobrist) generate_hash_key(pos Position) uint64 {
	// final hash key
	var final_key uint64

	// general purpose piece bitboard
	var bitboard Bitboard

	// loop over piece bitboards
	for piece := white_pawn; piece <= black_king; piece++ {
		// initialize bitboard to current piece
		bitboard = pos.bitboards[piece]

		// loop over pieces within bitboard 
		for bitboard > 0 {
			// get square occupied by piece and pop LSB
			square := bitboard.pop_lsb()

			// hash piece
			final_key ^= Zob.piece_keys[piece][square]
		}
	}
	
	// if enpassant square exists
	if pos.enpassant_square != NO_SQ {
		// hash enpassant square
		final_key ^= Zob.enpassant_keys[pos.enpassant_square]
	}

	// hash castling rights
	final_key ^= Zob.castle_keys[pos.castling_rights]

	// hash side to move (only if side is black)
	if pos.side_to_move == black {
		final_key ^= Zob.side_key
	}
	
	// return final hash key
	return final_key
}

// initialize random keys
func (Zob *Zobrist) init_random_keys() {
	// update pseudo random number state
	random_state = 1804289383

	// loop over pieces 
	for piece := white_pawn; piece <= black_king; piece++ {
		// loop over squares
		for square := 0; square < 64; square++ {
			// initialize random piece keys
			Zob.piece_keys[piece][square] = random_u64()
		}
	}

	// loop over board squares
	for square := 0; square < 64; square++ {
		// initialize random enpassant keys
		Zob.enpassant_keys[square] = random_u64() 
	}

	// loop over castling keys
    for index := 0; index < 16; index++ {
        // initialize castling keys
        Zob.castle_keys[index] = random_u64()
	}

	// init random side key
    Zob.side_key = random_u64()
    
}

// pseudo random number state
var random_state uint32 = 1804289383

// generate 32-bit pseudo legal numbers
func random_u32() uint32 {
	// get current state
	number := random_state

	// XOR shift algorithm
	number ^= number << 13
	number ^= number >> 17
	number ^= number << 5

	// update random number state
	random_state = number

	// return random number
	return number
}

func random_u64() uint64 {
	// define 4 random numbers
	var n1, n2, n3, n4 uint64

	// initialize random numbers
	n1 = uint64(random_u32() & 0xFFFF)
	n2 = uint64(random_u32() & 0xFFFF)
	n3 = uint64(random_u32() & 0xFFFF)
	n4 = uint64(random_u32() & 0xFFFF)

	return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48)
}