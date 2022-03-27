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

// diagonal masks
var MASK_DIAGONAL = [15]uint64 {
	0x80, 0x8040, 0x804020,
	0x80402010, 0x8040201008, 0x804020100804,
	0x80402010080402, 0x8040201008040201, 0x4020100804020100,
	0x2010080402010000, 0x1008040201000000, 0x804020100000000,
	0x402010000000000, 0x201000000000000, 0x100000000000000,
}

// anti diagonal masks
var MASK_ANTI_DIAGONAL = [15]uint64 {
	0x1, 0x102, 0x10204,
	0x1020408, 0x102040810, 0x10204081020,
	0x1020408102040, 0x102040810204080, 0x204081020408000,
	0x408102040800000, 0x810204080000000, 0x1020408000000000,
	0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
}

//array containing bitboard for each square (1 << square)
var SQUARE_BB = [64]uint64 {
	0x1, 0x2, 0x4, 0x8,
	0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000,
	0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x100000000, 0x200000000, 0x400000000, 0x800000000,
	0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
	0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
	0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
	0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
	0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
	0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
	0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
};

// bitwise manipulation functions
func rank_of(square int) int {
	return square >> 3
}

func file_of(square int) int {
	return square & 7
}

func diagonal_of(square int) int {
	return 7 + rank_of(square) - file_of(square)
}

func anti_diagonal_of(square int) int {
	return rank_of(square) + file_of(square)
}

// set_bit at given square to 1
func set_bit(bitboard uint64, square int) uint64 {
	return bitboard | (1 << square)
}

// return bit at given square
func get_bit(bitboard uint64, square int) uint64 {
	return bitboard & (1 << square)
}

// pop bit at given square (set to 0)
func pop_bit(bitboard uint64, square int) uint64 {
	if get_bit(bitboard, square) > 0 {
		return bitboard ^ (1 << square)
	}
	return bitboard
}

//returns reversed bitboard (rotate 180 degrees)
func reverse(bb uint64) uint64 {
	bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555)
	bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333)
	bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f)
	bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff)

	return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48)
}

// print bitboard
func print_bitboard(bitboard uint64) {
	for rank := 7; rank >= 0; rank-- {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if file == 0 {
				fmt.Print(" ", rank + 1, " ")
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
