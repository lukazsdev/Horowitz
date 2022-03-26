package main

import "fmt"

// file masks
var MASK_FILE = [8]uint64{
	0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
	0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
}

// rank masks
var MASK_RANK = [8]uint64{
	0xff, 0xff00, 0xff0000, 0xff000000,
	0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000,
}

// bitwise manipulation functions
func set_bit(bitboard uint64, square int) uint64 {
	return bitboard | (1 << square)
}

func get_bit(bitboard uint64, square int) uint64 {
	return bitboard & (1 << square)
}

func pop_bit(bitboard uint64, square int) uint64 {
	if get_bit(bitboard, square) > 0 {
		return bitboard ^ (1 << square)
	}
	return bitboard
}

// print bitboard
func print_bitboard(bitboard uint64) {
	for rank := 0; rank < 8; rank++ {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if file == 0 {
				fmt.Print(" ", 8-rank, " ")
			}

			bit := get_bit(bitboard, square)
			if bit > 0 {
				fmt.Print(" ", 1)
			} else {
				fmt.Print(" ", 0)
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("    a b c d e f g h\n\n")
}
