package main

import "fmt"


/*

	Board representation:

	e.g binary: ...000100010 = ... d1 c1 b1 a1 (a1 is least significant bit)

	Bitboard:

	a8 b8 c8 d8 e8 f8 g8 h8     56 57 58 59 60 61 62 63
	a7 b7 c7 d7 e7 f7 g7 h7     56 57 58 59 60 61 62 63
	a6 b6 c6 d6 e6 f6 g6 h6     48 49 50 51 52 53 54 55
	a5 b5 c5 d5 e5 f5 g5 h5  =  32 33 34 35 36 37 38 39
	a4 b4 c4 d4 e4 f4 g4 h4     24 25 26 27 28 29 30 31
	a3 b3 c3 d3 e3 f3 g3 h3     16 17 18 19 20 21 22 23
	a2 b2 c2 d2 e2 f2 g2 h2      8  9 10 11 12 13 14 15
	a1 b1 c1 d1 e1 f1 g1 h1      0  1  2  3  4  5  6  7

*/

// define piece bitboards
var BITBOARDS [12]uint64

// occupancy bitboards
var OCCUPANCIES [3]uint64

// side to move
var SIDE int = -1

// enpassant square
var ENPASSANT int = NO_SQ

// castling rights
var CASTLE int = 0



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
