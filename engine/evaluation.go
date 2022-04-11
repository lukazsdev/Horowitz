package main

//import "fmt"

// piece value constants
var material_score = map[uint8]int{
	white_pawn: 100, 
	white_knight: 300, 
	white_bishop: 350, 
	white_rook: 500, 
	white_queen: 1000, 
	black_pawn: -100,
	black_knight: -300,
	black_bishop: -350,
	black_rook: -500,
	black_queen: -1000,
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
	score := 0
	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
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
	score := 0
	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
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
	score := 0

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
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
	score := 0

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
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
	
	piece := get_piece_type(Queen, side)
	score := 0

	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
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
	score := 0

	
	bitboard := pos.bitboards[piece]
	for bitboard > 0 {
		square := bitboard.pop_lsb()
		score += material_score[piece]
		switch (side) {
		case white: 
			score += KING_SQUARE_TABLE[square]
		case black: 
			score -= KING_SQUARE_TABLE[mirror[square]]
		}
	}

	return score
}
