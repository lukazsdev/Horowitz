package main

// pawn attacks table
var pawn_attacks [2][64]uint64

// knight attacks table
var knight_attacks [64]uint64

// generate pawn attacks
func mask_pawn_attacks(side int, square int) uint64 {
	var attacks, bitboard uint64 = 0, 0
	bitboard = set_bit(bitboard, square)

	if side == white {
		attacks |= (bitboard >> 7) & (^MASK_FILE[FILE_A])
		attacks |= (bitboard >> 9) & (^MASK_FILE[FILE_H])
	} else {
		attacks |= (bitboard << 7) & (^MASK_FILE[FILE_H])
		attacks |= (bitboard << 9) & (^MASK_FILE[FILE_A])
	}

	return attacks
}

// generate knight attacks
func mask_knight_attacks(square int) uint64 {
	var attacks, bitboard uint64 = 0, 0
	bitboard = set_bit(bitboard, square)

	attacks |= (bitboard >> 17) & (^MASK_FILE[FILE_H])
	attacks |= (bitboard >> 15) & (^MASK_FILE[FILE_A])
	attacks |= (bitboard << 17) & (^MASK_FILE[FILE_A])
	attacks |= (bitboard << 15) & (^MASK_FILE[FILE_H])

	attacks |= (bitboard >> 10) & ^(MASK_FILE[FILE_G] | MASK_FILE[FILE_H])
	attacks |= (bitboard >> 6) & ^(MASK_FILE[FILE_A] | MASK_FILE[FILE_B])
	attacks |= (bitboard << 10) & ^(MASK_FILE[FILE_A] | MASK_FILE[FILE_B])
	attacks |= (bitboard << 6) & ^(MASK_FILE[FILE_G] | MASK_FILE[FILE_H])

	return attacks
}

func initialize_leaper_attacks() {
	for square := 0; square < 64; square++ {
		pawn_attacks[white][square] = mask_pawn_attacks(white, square)
		pawn_attacks[black][square] = mask_pawn_attacks(black, square)

		knight_attacks[square] = mask_knight_attacks(square)
	}
}
