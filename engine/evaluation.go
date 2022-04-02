package main

// piece value constants
var material_score = map[uint8]int{
	white_pawn: 100, 
	white_knight: 300, 
	white_bishop: 350, 
	white_rook: 500, 
	white_queen: 1000, 
	white_king: 10000, 
	black_pawn: -100,
	black_knight: -300,
	black_bishop: -350,
	black_rook: -500,
	black_queen: -1000,
	black_king: -10000,
}

// pawn piece square table
var PAWN_SQUARE_TABLE = [64]int {
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0, -10, -10,   0,   0,   0,
	0,   0,   0,   5,   5,   0,   0,   0,
	5,   5,  10,  20,  20,   5,   5,   5,
	10,  10,  10,  20,  20,  10,  10,  10,
	20,  20,  20,  30,  30,  30,  20,  20,
	30,  30,  30,  40,  40,  30,  30,  30,
	90,  90,  90,  90,  90,  90,  90,  90,
}

// knight piece square table
var KNIGHT_SQUARE_TABLE = [64]int {
	-5, -10,   0,   0,   0,   0, -10,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   5,  20,  10,  10,  20,   5,  -5,
	-5,  10,  20,  30,  30,  20,  10,  -5,
	-5,  10,  20,  30,  30,  20,  10,  -5,
	-5,   5,  20,  20,  20,  20,   5,  -5,
	-5,   0,   0,  10,  10,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
}

// bishop piece square table
var BISHOP_SQUARE_TABLE = [64]int {
	 0,   0, -10,   0,   0, -10,   0,   0,
	 0,  30,   0,   0,   0,   0,  30,   0,
	 0,  10,   0,   0,   0,   0,  10,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,  20,   0,  10,  10,   0,  20,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,
}

// rook piece square table
var ROOK_SQUARE_TABLE = [64]int {
	 0,   0,   0,  20,  20,   0,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 0,   0,  10,  20,  20,  10,   0,   0,
	 50,  50,  50,  50,  50,  50,  50,  50,
	 50,  50,  50,  50,  50,  50,  50,  50,

}

// king piece square table
var KING_SQUARE_TABLE = [64]int {
	 0,   0,   5,   0, -15,   0,  10,   0,
	 0,   5,   5,  -5,  -5,   0,   5,   0,
	 0,   0,   5,  10,  10,   5,   0,   0,
	 0,   5,  10,  20,  20,  10,   5,   0,
	 0,   5,  10,  20,  20,  10,   5,   0,
	 0,   5,   5,  10,  10,   5,   5,   0,
	 0,   0,   5,   5,   5,   5,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,
}

// mirror positional score tables for black
var mirror = [64]int {
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
}


// position evaluation
func evaluate(pos Position) int {
	score := 0

	score += pawns_eval(pos, white)
	score += knights_eval(pos, white)
	score += bishops_eval(pos, white)
	score += rooks_eval(pos, white)
	score += queens_eval(pos, white)
	score += king_eval(pos, white)

	score += pawns_eval(pos, black)
	score += knights_eval(pos, black)
	score += bishops_eval(pos, black)
	score += rooks_eval(pos, black)
	score += queens_eval(pos, black)
	score += king_eval(pos, black)

	// white => score, black => -score
	return perspective(score, pos.side_to_move)
}

// pawns evaluation
func pawns_eval(pos Position, side uint8) int {

	piece := get_piece_type(Pawn, side)
	score := material_score[piece]

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white: 
			score += PAWN_SQUARE_TABLE[square]
		case black: 
			score -= PAWN_SQUARE_TABLE[mirror[square]]
		}
	}

	return score
}

// knights evaluation
func knights_eval(pos Position, side uint8) int {
	
	piece := get_piece_type(Knight, side)
	score := material_score[piece]

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white: 
			score += KNIGHT_SQUARE_TABLE[square]
		case black: 
			score -= KNIGHT_SQUARE_TABLE[mirror[square]]
		}
	}

	return score
}

// bishops evaluation
func bishops_eval(pos Position, side uint8) int {
	
	piece := get_piece_type(Bishop, side)
	score := material_score[piece]

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white: 
			score += BISHOP_SQUARE_TABLE[square]
			score += get_bishop_attacks(square, pos.occupied[both]).count_bits()
		case black: 
			score -= BISHOP_SQUARE_TABLE[mirror[square]]
			score -= get_bishop_attacks(square, pos.occupied[both]).count_bits()
		}
	}

	return score
}

// rooks evaluation
func rooks_eval(pos Position, side uint8) int {
	
	piece := get_piece_type(Rook, side)
	score := material_score[piece]

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white: 
			score += ROOK_SQUARE_TABLE[square]
		case black: 
			score -= ROOK_SQUARE_TABLE[mirror[square]]
		}
	}

	return score
}

// queens evaluation
func queens_eval(pos Position, side uint8) int {
	
	piece := get_piece_type(Pawn, side)
	score := material_score[piece]

	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white:
			score += get_queen_attacks(square, pos.occupied[both]).count_bits()
		case black:
			score -= get_queen_attacks(square, pos.occupied[both]).count_bits()
		}
	}

	return score
}

// king evaluation
func king_eval(pos Position, side uint8) int {
	
	piece := get_piece_type(King, side)
	score := material_score[piece]

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		switch (side) {
		case white: 
			score += KING_SQUARE_TABLE[square]
		case black: 
			score -= KING_SQUARE_TABLE[mirror[square]]
		}
	}

	return score
}
