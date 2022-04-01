package main

//import "fmt"


// lookup table for knight attacks
var KNIGHT_ATTACKS_TABLE = [64]Bitboard {
    0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200,
    0x0000000000284400, 0x0000000000508800, 0x0000000000A01000, 0x0000000000402000,
    0x0000000002040004, 0x0000000005080008, 0x000000000A110011, 0x0000000014220022,
    0x0000000028440044, 0x0000000050880088, 0x00000000A0100010, 0x0000000040200020,
    0x0000000204000402, 0x0000000508000805, 0x0000000A1100110A, 0x0000001422002214,
    0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040,
    0x0000020400040200, 0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400,
    0x0000284400442800, 0x0000508800885000, 0x0000A0100010A000, 0x0000402000204000,
    0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000, 0x0014220022140000,
    0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000,
    0x0204000402000000, 0x0508000805000000, 0x0A1100110A000000, 0x1422002214000000,
    0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,
    0x0400040200000000, 0x0800080500000000, 0x1100110A00000000, 0x2200221400000000,
    0x4400442800000000, 0x8800885000000000, 0x100010A000000000, 0x2000204000000000,
    0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000,
}

// lookup table for king attacks
var KING_ATTACKS_TABLE = [64]Bitboard {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14,
    0x0000000000003828, 0x0000000000007050, 0x000000000000E0A0, 0x000000000000C040,
    0x0000000000030203, 0x0000000000070507, 0x00000000000E0A0E, 0x00000000001C141C,
    0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0, 0x0000000000C040C0,
    0x0000000003020300, 0x0000000007050700, 0x000000000E0A0E00, 0x000000001C141C00,
    0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000,
    0x0000000302030000, 0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000,
    0x0000003828380000, 0x0000007050700000, 0x000000E0A0E00000, 0x000000C040C00000,
    0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000, 0x00001C141C000000,
    0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000,
    0x0003020300000000, 0x0007050700000000, 0x000E0A0E00000000, 0x001C141C00000000,
    0x0038283800000000, 0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000,
    0x0302030000000000, 0x0705070000000000, 0x0E0A0E0000000000, 0x1C141C0000000000,
    0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000, 0xC040C00000000000,
    0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,
    0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000,
}

// lookup table for white pawn attacks
var white_PAWN_ATTACKS = [64]Bitboard {
	0x200, 0x500, 0xa00, 0x1400,
	0x2800, 0x5000, 0xa000, 0x4000,
	0x20000, 0x50000, 0xa0000, 0x140000,
	0x280000, 0x500000, 0xa00000, 0x400000,
	0x2000000, 0x5000000, 0xa000000, 0x14000000,
	0x28000000, 0x50000000, 0xa0000000, 0x40000000,
	0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
	0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
	0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
	0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
	0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
	0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
	0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000,
	0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
}

// lookup table for black pawn attacks
var black_PAWN_ATTACKS = [64]Bitboard {
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x2, 0x5, 0xa, 0x14,
	0x28, 0x50, 0xa0, 0x40,
	0x200, 0x500, 0xa00, 0x1400,
	0x2800, 0x5000, 0xa000, 0x4000,
	0x20000, 0x50000, 0xa0000, 0x140000,
	0x280000, 0x500000, 0xa00000, 0x400000,
	0x2000000, 0x5000000, 0xa000000, 0x14000000,
	0x28000000, 0x50000000, 0xa0000000, 0x40000000,
	0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
	0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
	0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
	0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
	0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
	0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
}

// lookup table for pawn attacks
var PAWN_ATTACKS_TABLE [2][64]Bitboard

// initialize pawn attacks lookup table
func initialize_lookup_tables() {
	for square := 0; square < 64; square++ {
		PAWN_ATTACKS_TABLE[white][square] = white_PAWN_ATTACKS[square]
		PAWN_ATTACKS_TABLE[black][square] = black_PAWN_ATTACKS[square]
	}
}

// slider pseudo move generation (hyperbola quintessence)
func hyp_quint(square int, occ Bitboard, mask Bitboard) Bitboard {
	return (((mask & occ) - SQUARE_BB[square] * 2) ^
        reverse(reverse(mask & occ) - reverse(SQUARE_BB[square]) * 2)) & mask;
}

// returns rook attacks from given square and occupied bitboard
func get_rook_attacks(square int, occ Bitboard) Bitboard {
	return hyp_quint(square, occ, MASK_FILE[file_of(square)]) |
        hyp_quint(square, occ, MASK_RANK[rank_of(square)])
}

// returns bishop attacks from given square and occupied bitboard
func get_bishop_attacks(square int, occ Bitboard) Bitboard {
	return hyp_quint(square, occ, MASK_DIAGONAL[diagonal_of(square)]) |
        hyp_quint(square, occ, MASK_ANTI_DIAGONAL[anti_diagonal_of(square)])
}

// returns queen attacks from given square and occupied bitboard
func get_queen_attacks(square int, occ Bitboard) Bitboard {
	return get_rook_attacks(square, occ) | get_bishop_attacks(square, occ)
}




func (pos *Position) generate_moves() MoveList {
	var moves MoveList
	// current piece bitboard, and its attacks
	var bitboard, attacks Bitboard

	// "from" square and "to" square for current move
	var source_square, target_square int

	// current side and opposite side
	var our_side, their_side uint8 = pos.side_to_move, other_side(pos.side_to_move)

	// loop over all piece bitboards
	for piece := white_pawn; piece <= black_king; piece++ {
		// assign current piece to bitboard
		bitboard = pos.bitboards[piece]

		// generate pawns and king castling moves
		if (our_side == white && piece == white_pawn) || (our_side == black && piece == black_pawn) {
			// loop over pawns within bitboard
			for bitboard > 0 {
				// initialize source square
				source_square = bitboard.pop_lsb()

				// generate white pawn moves
				if our_side == white {
					// initialize target square
					target_square = source_square + 8

					// if target square does not contain any piece
					if pos.occupied[both].get_bit(target_square) == 0 {
						// pawn promotion
						if (source_square>=SQ_A7) && (source_square <= SQ_H7) {
							moves.add(encode_move(source_square, target_square, piece, white_queen, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_rook, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_bishop, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_knight, 0, 0, 0, 0))
						} else {
							// single push
							moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))
							// double push
							if (source_square>=SQ_A2) && (source_square<=SQ_H2) && pos.occupied[both].get_bit(target_square + 8) == 0 {
								moves.add(encode_move(source_square, target_square + 8, piece, 0, 0, 1, 0, 0))
							}
						}
					}

					// initialize pawn attacks bitboard
					attacks = PAWN_ATTACKS_TABLE[our_side][source_square] & pos.occupied[black]

					// generate pawn captures
					for attacks > 0 {
						// initialize target square
						target_square = attacks.pop_lsb()

						// pawn promotion
						if (source_square>=SQ_A7) && (source_square <= SQ_H7) {
							moves.add(encode_move(source_square, target_square, piece, white_queen, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_rook, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_bishop, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, white_knight, 1, 0, 0, 0))
						} else {
							// normal capture
							moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
						}
					}

					// generate enpassant captures
					if pos.enpassant_square != NO_SQ {
						var enpassant_capture Bitboard = PAWN_ATTACKS_TABLE[our_side][source_square] & SQUARE_BB[pos.enpassant_square]

						// check if enpassant capture is available
						if enpassant_capture > 0 {
							// initialize enpassant target square
							var target_enpassant int = enpassant_capture.bsf()
							moves.add(encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0))
						}
					}
				// generate black pawn moves
				} else {
					// initialize target square
					target_square = source_square - 8

					// if target square does not contain any piece
					if pos.occupied[both].get_bit(target_square) == 0 {
						// pawn promotion
						if (source_square>=SQ_A2) && (source_square <= SQ_H2) {
							moves.add(encode_move(source_square, target_square, piece, black_queen, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_rook, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_bishop, 0, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_knight, 0, 0, 0, 0))
						} else {
							// single push
							moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))
							// double push
							if (source_square>=SQ_A7) && (source_square<=SQ_H7) && pos.occupied[both].get_bit(target_square - 8) == 0 {
								moves.add(encode_move(source_square, target_square - 8, piece, 0, 0, 1, 0, 0))
							}
						}
					}

					// initialize pawn attacks bitboard
					attacks = PAWN_ATTACKS_TABLE[our_side][source_square] & pos.occupied[white]

					// generate pawn captures
					for attacks > 0 {
						// initialize target square
						target_square = attacks.pop_lsb()

						// pawn promotion
						if (source_square>=SQ_A2) && (source_square <= SQ_H2) {
							moves.add(encode_move(source_square, target_square, piece, black_queen, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_rook, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_bishop, 1, 0, 0, 0))
						        moves.add(encode_move(source_square, target_square, piece, black_knight, 1, 0, 0, 0))
						} else {
							// normal capture
							moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
						}
					}

					// generate enpassant captures
					if pos.enpassant_square != NO_SQ {
						var enpassant_capture Bitboard = PAWN_ATTACKS_TABLE[our_side][source_square] & SQUARE_BB[pos.enpassant_square]

						// check if enpassant capture is available
						if enpassant_capture > 0 {
							// initialize enpassant target square
							var target_enpassant int = enpassant_capture.bsf()
							moves.add(encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0))
						}
					}
				}
			}
		}

		// castling moves for white
		if our_side == white && piece == white_king {
			// check if king side castling is available
			if (pos.castling_rights & white_kingside_castle) > 0 {
				// check if squares between king and rook are empty
				if (pos.occupied[both].get_bit(SQ_F1)==0) && (pos.occupied[both].get_bit(SQ_G1)==0) {
					// check if king and f1 square are not under attack
					if is_square_attacked(SQ_E1, black, *pos)==false && is_square_attacked(SQ_F1, black, *pos)==false {
						moves.add(encode_move(SQ_E1, SQ_G1, piece, 0, 0, 0, 0, 1))
					}
				}
			}

			// check if queen side castling is available
			if (pos.castling_rights & white_queenside_castle) > 0 {
				// check if squares between king and rook are empty
				if (pos.occupied[both].get_bit(SQ_D1)==0) && (pos.occupied[both].get_bit(SQ_C1)==0) && (pos.occupied[both].get_bit(SQ_B1)==0) {
					// check if king and d1 square are not under attack
					if is_square_attacked(SQ_E1, black, *pos)==false && is_square_attacked(SQ_D1, black, *pos)==false {
						moves.add(encode_move(SQ_E1, SQ_C1, piece, 0, 0, 0, 0, 1))
					}
				}
			}
		}

		// castling moves for black
		if our_side == black && piece == black_king {
			// check if king side castling is available
			if (pos.castling_rights & black_kingside_castle) > 0 {
				// check if squares between king and rook are empty
				if (pos.occupied[both].get_bit(SQ_F8)==0) && (pos.occupied[both].get_bit(SQ_G8)==0) {
					// check if king and f1 square are not under attack
					if is_square_attacked(SQ_E8, white, *pos)==false && is_square_attacked(SQ_F8, white, *pos)==false {
						
						moves.add(encode_move(SQ_E8, SQ_G8, piece, 0, 0, 0, 0, 1))
					}
				}
			}

			// check if queen side castling is available
			if (pos.castling_rights & black_queenside_castle) > 0 {
				// check if squares between king and rook are empty
				if (pos.occupied[both].get_bit(SQ_D8)==0) && (pos.occupied[both].get_bit(SQ_C8)==0) && (pos.occupied[both].get_bit(SQ_B8)==0) {
					// check if king and d1 square are not under attack
					if is_square_attacked(SQ_E8, white, *pos)==false && is_square_attacked(SQ_D8, white, *pos)==false {
						moves.add(encode_move(SQ_E8, SQ_C8, piece, 0, 0, 0, 0, 1))
					}
				}
			}
		}		

		// generate knight moves
		
		if (our_side == white && piece == white_knight) || (our_side == black && piece == black_knight) {
			for bitboard > 0 {
				source_square = bitboard.pop_lsb()
				attacks = KNIGHT_ATTACKS_TABLE[source_square] & (^pos.occupied[our_side])
				for attacks > 0 {
					// initialize target square and pop LSB
					target_square = attacks.pop_lsb()
					// quiet move
					if pos.occupied[their_side].get_bit(target_square) == 0 {
						moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))

					// capture move
					} else {
						moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
					}
				}
			}
		} 
		
		// generate bishop moves
		if (our_side == white && piece == white_bishop) || (our_side == black && piece == black_bishop) {
			for bitboard > 0 {
				source_square = bitboard.pop_lsb()
				attacks = get_bishop_attacks(source_square, pos.occupied[both]) & (^pos.occupied[our_side])
				for attacks > 0 {
					// initialize target square and pop LSB
					target_square = attacks.pop_lsb()
					// quiet move
					if pos.occupied[their_side].get_bit(target_square) == 0 {
						moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))

					// capture move
					} else {
						moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
					}
				}
			}
		} 
		// generate rook moves
		if (our_side == white && piece == white_rook) || (our_side == black && piece == black_rook) {
			for bitboard > 0 {
				source_square = bitboard.pop_lsb()
				attacks = get_rook_attacks(source_square, pos.occupied[both]) & (^pos.occupied[our_side])
				for attacks > 0 {
					// initialize target square and pop LSB
					target_square = attacks.pop_lsb()
					// quiet move
					if pos.occupied[their_side].get_bit(target_square) == 0 {
						moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))

					// capture move
					} else {
						moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
					}
				}
			}
		} 
		// generate queen moves
		if (our_side == white && piece == white_queen) || (our_side == black && piece == black_queen) {
			for bitboard > 0 {
				source_square = bitboard.pop_lsb()
				attacks = get_queen_attacks(source_square, pos.occupied[both]) & (^pos.occupied[our_side])
				for attacks > 0 {
					// initialize target square and pop LSB
					target_square = attacks.pop_lsb()
					// quiet move
					if pos.occupied[their_side].get_bit(target_square) == 0 {
						moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))

					// capture move
					} else {
						moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
					}
				}
			}
		} 
		// generate king moves
		if (our_side == white && piece == white_king) || (our_side == black && piece == black_king) {
			for bitboard > 0 {
				source_square = bitboard.pop_lsb()
				attacks = KING_ATTACKS_TABLE[source_square] & (^pos.occupied[our_side])
				for attacks > 0 {
					// initialize target square and pop LSB
					target_square = attacks.pop_lsb()
					// quiet move
					if pos.occupied[their_side].get_bit(target_square) == 0 {
						moves.add(encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0))

					// capture move
					} else {
						moves.add(encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0))
					}
				}
			}
		}
	}

	return moves
}


// checks if square is attacked by the given side
func is_square_attacked(square int, side uint8, pos Position) bool {
	if square != NO_SQ {
		if side == white && ((PAWN_ATTACKS_TABLE[black][square] & pos.bitboards[white_pawn]) > 0) { return true }
		if side == black && ((PAWN_ATTACKS_TABLE[white][square] & pos.bitboards[black_pawn]) > 0) { return true }

		if (KNIGHT_ATTACKS_TABLE[square] & pos.bitboards[6 * side + white_knight]) > 0 { return true }
		if (KING_ATTACKS_TABLE[square] & pos.bitboards[6 * side + white_king])     > 0 { return true }

		if (get_bishop_attacks(square, pos.occupied[both]) & pos.bitboards[6 * side + white_bishop]) > 0 { return true }
		if (get_rook_attacks(square, pos.occupied[both])   & pos.bitboards[6 * side + white_rook])   > 0 { return true }
		if (get_queen_attacks(square, pos.occupied[both])  & pos.bitboards[6 * side + white_queen])  > 0 { return true }
	}

	return false
}
