package main

const (
	// pieces
	white_pawn   uint8 = 0
	white_knight uint8 = 1
	white_bishop uint8 = 2
	white_rook   uint8 = 3
	white_queen  uint8 = 4
	white_king   uint8 = 5
	black_pawn   uint8 = 6
	black_knight uint8 = 7
	black_bishop uint8 = 8
	black_rook   uint8 = 9
	black_queen  uint8 = 10
	black_king   uint8 = 11
	no_piece     uint8 = 12

	// piece types
	Pawn   uint8 = 0
	Knight uint8 = 1
	Bishop uint8 = 2
	Rook   uint8 = 3
	Queen  uint8 = 4
	King   uint8 = 5

	// colors
	white uint8 = 0
	black uint8 = 1
	both  uint8 = 2

	// castling rights
	white_kingside_castle  uint8 = 1
	white_queenside_castle uint8 = 2
	black_kingside_castle  uint8 = 4
	black_queenside_castle uint8 = 8

	// square constants 
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8 = 56, 57, 58, 59, 60, 61, 62, 63
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7 = 48, 49, 50, 51, 52, 53, 54, 55
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6 = 40, 41, 42, 43, 44, 45, 46, 47
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5 = 32, 33, 34, 35, 36, 37, 38, 39
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4 = 24, 25, 26, 27, 28, 29, 30, 31
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3 = 16, 17, 18, 19, 20, 21, 22, 23
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2 =  8,  9, 10, 11, 12, 13, 14, 15
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1 =  0,  1,  2,  3,  4,  5,  6,  7
	
	NO_SQ = 64

	// files and ranks indexes
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H = 0, 1, 2, 3, 4, 5, 6, 7 
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 = 0, 1, 2, 3, 4, 5, 6, 7

	all_moves     uint8 = 0
	only_captures uint8 = 1
)

var char_to_piece map[byte]uint8 = map[byte]uint8{
	'P': white_pawn,
	'N': white_knight,
	'B': white_bishop,
	'R': white_rook,
	'Q': white_queen,
	'K': white_king,
	'p': black_pawn,
	'n': black_knight,
	'b': black_bishop,
	'r': black_rook,
	'q': black_queen,
	'k': black_king,
}

var piece_to_char map[byte]uint8 = map[byte]uint8{
	white_pawn: 'P', 
	white_knight: 'N', 
	white_bishop: 'B', 
	white_rook: 'R', 
	white_queen: 'Q', 
	white_king: 'K', 
	black_pawn: 'p',
	black_knight: 'n',
	black_bishop: 'b',
	black_rook: 'r',
	black_queen: 'q',
	black_king: 'k',
}

var promoted_pieces map[uint8]byte = map[uint8]byte{
	white_queen: 'q',
	white_rook: 'r',
	white_bishop: 'b',
	white_knight: 'n',
	black_queen: 'q',
	black_rook: 'r',
	black_bishop: 'b',
	black_knight: 'n',
}

var move_to_uci = []string{
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
}

// castling rights update constants
var square_to_castling_rights = [64]uint8 {
   13, 15, 15, 15, 12, 15, 15, 14,
   15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15,
    7, 15, 15, 15,  3, 15, 15, 11,
}

