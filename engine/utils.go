package main

import "fmt"

// returns file of given square
func rank_of(square int) int {
	return square >> 3
}

// returns file of given square
func file_of(square int) int {
	return square & 7
}

// returns diagonal of given square
func diagonal_of(square int) int {
	return 7 + rank_of(square) - file_of(square)
}

// returns anti diagonal of given square
func anti_diagonal_of(square int) int {
	return rank_of(square) + file_of(square)
}

// print move (for UCI purposes)
func print_move(move Move) {
	if move.get_move_promoted() > 0 {
	fmt.Print(square_to_coordinates[move.get_move_source()],
			  square_to_coordinates[move.get_move_target()])
	fmt.Printf("%c", promoted_pieces[move.get_move_promoted()])
	} else {
		fmt.Print(square_to_coordinates[move.get_move_source()],
				  square_to_coordinates[move.get_move_target()])
	}
	fmt.Print("\n")
}

// print move list (for debuggin purposes)
func print_move_list(moves MoveList) {
	if moves.count == 0 {
		fmt.Print("\nNo moves in move list\n")
		return
	} else {
		fmt.Print("\n     move    piece     capture   double    enpass    castling\n\n")
		for move_count := 0; move_count < moves.count; move_count++ {
			move := moves.list[move_count]

			fmt.Print("     ", square_to_coordinates[move.get_move_source()], square_to_coordinates[move.get_move_target()])
			if move.get_move_promoted() > 0 { fmt.Printf("%c", promoted_pieces[move.get_move_promoted()]) } else { fmt.Print(" ") }
			fmt.Printf("     %c", piece_to_char[move.get_move_piece()])
			if move.get_move_capture() > 0 { fmt.Print("          1") } else { fmt.Print("          0") }
			if move.get_move_double() > 0 { fmt.Print("        1") } else { fmt.Print("        0") }
			if move.get_move_enpassant() > 0 { fmt.Print("         1") } else { fmt.Print("         0") }
			if move.get_move_castling() > 0 { fmt.Print("          1") } else { fmt.Print("          0") }
			fmt.Print("\n")
		}
	}

	fmt.Print("\n\n     Total number of moves: ", moves.count, "\n\n")
}

// print given bitboard
func print_bitboard(bitboard Bitboard) {
	fmt.Print("\n")
	for rank := 7; rank >= 0; rank-- {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if file == 0 {
				fmt.Print(" ", rank + 1, " ")
			}

			bit := bitboard.get_bit(square)
			if bit > 0 {
				fmt.Print(" ", 1)
			} else {
				fmt.Print(" ", 0)
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("\n    a b c d e f g h\n\n")
}

// print board
func print_board(pos Position) {
	fmt.Print("\n")
	for rank := 7; rank >= 0; rank-- {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if file == 0 {
				fmt.Print(" ", rank + 1, " ")
			}

			var piece uint8 = no_piece

			for bb_piece := white_pawn; bb_piece <= black_king; bb_piece++ {
				if pos.bitboards[bb_piece].get_bit(square) > 0 {
					piece = bb_piece
				}
			}


			if piece == no_piece {
				fmt.Print(" .")
			} else {
				fmt.Printf(" %c", piece_to_char[piece])
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("\n    a b c d e f g h\n\n")

	if pos.side_to_move == white {
		fmt.Print("   Side:    white\n")
	} else {
		fmt.Print("   Side:    black\n")
	}

	if pos.enpassant_square != NO_SQ {
		fmt.Print("   Enpass:    ", square_to_coordinates[pos.enpassant_square], "\n")
	} else {
		fmt.Print("   Enpass:     no\n")
	}

	fmt.Print("   Castling:  ")
	if (pos.castling_rights & white_kingside_castle) > 0 {
		fmt.Print("K")
	} else {
		fmt.Print("-")
	}
	if (pos.castling_rights & white_queenside_castle) > 0 {
		fmt.Print("Q")
	} else {
		fmt.Print("-")
	}
	if (pos.castling_rights & black_kingside_castle) > 0 {
		fmt.Print("k")
	} else {
		fmt.Print("-")
	}
	if (pos.castling_rights & black_queenside_castle) > 0 {
		fmt.Print("q")
	} else {
		fmt.Print("-")
	}
	fmt.Print("\n")
}



