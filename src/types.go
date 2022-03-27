package main

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

// ASCII pieces
var ascii_pieces = [12]byte{'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'k', 'b', 'r', 'q', 'k'}

// convert ASCII pieces to encoded constants
var char_pieces = []int{'P':P, 'N':N, 'B':B, 'R':R, 'Q':Q, 'K':K, 'p':p, 'n':n, 'b':b, 'r':r, 'q':q, 'k':k}

var square_to_coordinates = []string{
	"SQ_A1", "SQ_B1", "SQ_C1", "SQ_D1", "SQ_E1", "SQ_F1", "SQ_G1", "SQ_H1",
	"SQ_A2", "SQ_B2", "SQ_C2", "SQ_D2", "SQ_E2", "SQ_F2", "SQ_G2", "SQ_H2",
	"SQ_A3", "SQ_B3", "SQ_C3", "SQ_D3", "SQ_E3", "SQ_F3", "SQ_G3", "SQ_H3",
	"SQ_A4", "SQ_B4", "SQ_C4", "SQ_D4", "SQ_E4", "SQ_F4", "SQ_G4", "SQ_H4",
	"SQ_A5", "SQ_B5", "SQ_C5", "SQ_D5", "SQ_E5", "SQ_F5", "SQ_G5", "SQ_H5",
	"SQ_A6", "SQ_B6", "SQ_C6", "SQ_D6", "SQ_E6", "SQ_F6", "SQ_G6", "SQ_H6",
	"SQ_A7", "SQ_B7", "SQ_C7", "SQ_D7", "SQ_E7", "SQ_F7", "SQ_G7", "SQ_H7",
	"SQ_A8", "SQ_B8", "SQ_C8", "SQ_D8", "SQ_E8", "SQ_F8", "SQ_G8", "SQ_H8",
}


const (
	WHITE = 0
	BLACK = 1
)

const (
	WK = 1
	WQ = 2
	BK = 4
	BQ = 8
)

const (
	P = 0
	N = 1
	B = 2
	R = 3
	Q = 4
	K = 5
	p = 6
	n = 7
	b = 8
	r = 9
	q = 10
	k = 11
)

const (
	FILE_A = 0
	FILE_B = 1
	FILE_C = 2
	FILE_D = 3
	FILE_E = 4
	FILE_F = 5
	FILE_G = 6
	FILE_H = 7
)

const (
	RANK_1 = 0
	RANK_2 = 1
	RANK_3 = 2
	RANK_4 = 3
	RANK_5 = 4
	RANK_6 = 5
	RANK_7 = 6
	RANK_8 = 7
)

const (
	SQ_A1 = 0
	SQ_B1 = 1
	SQ_C1 = 2
	SQ_D1 = 3
	SQ_E1 = 4
	SQ_F1 = 5
	SQ_G1 = 6
	SQ_H1 = 7

	SQ_A2 = 8
	SQ_B2 = 9
	SQ_C2 = 10
	SQ_D2 = 11
	SQ_E2 = 12
	SQ_F2 = 13
	SQ_G2 = 14
	SQ_H2 = 15

	SQ_A3 = 16
	SQ_B3 = 17
	SQ_C3 = 18
	SQ_D3 = 19
	SQ_E3 = 20
	SQ_F3 = 21
	SQ_G3 = 22
	SQ_H3 = 23

	SQ_A4 = 24
	SQ_B4 = 25
	SQ_C4 = 26
	SQ_D4 = 27
	SQ_E4 = 28
	SQ_F4 = 29
	SQ_G4 = 30
	SQ_H4 = 31

	SQ_A5 = 32
	SQ_B5 = 33
	SQ_C5 = 34
	SQ_D5 = 35
	SQ_E5 = 36
	SQ_F5 = 37
	SQ_G5 = 38
	SQ_H5 = 39

	SQ_A6 = 40
	SQ_B6 = 41
	SQ_C6 = 42
	SQ_D6 = 43
	SQ_E6 = 44
	SQ_F6 = 45
	SQ_G6 = 46
	SQ_H6 = 47

	SQ_A7 = 48
	SQ_B7 = 49
	SQ_C7 = 50
	SQ_D7 = 51
	SQ_E7 = 52
	SQ_F7 = 53
	SQ_G7 = 54
	SQ_H7 = 55

	SQ_A8 = 56
	SQ_B8 = 57
	SQ_C8 = 58
	SQ_D8 = 59
	SQ_E8 = 60
	SQ_F8 = 61
	SQ_G8 = 62
	SQ_H8 = 63
	NO_SQ = 64
)
