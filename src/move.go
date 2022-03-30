package main

import "fmt"

// Move list structure
type Moves struct {
	// move list
	moves [256]int

	// pointer to current move
	count int
}

// function for encoding a move
func encode_move(source, target, piece, promoted, capture, double, enpassant, castling int) int {
	return ((source)          |          
	        (target << 6)     |     
	        (piece << 12)     |     
	        (promoted << 16)  |  
	        (capture << 20)   |   
	        (double << 21)    |    
	        (enpassant << 22) | 
	        (castling << 23))
}

// extract source square
func get_move_source(move int) int {
	return move & 0x3f
}

// extract target square
func get_move_target(move int) int {
	return (move & 0xfc0) >> 6
}

// extract piece 
func get_move_piece(move int) int {
	return (move & 0xf000) >> 12
}

// extract promoted piece
func get_move_promoted(move int) int {
	return (move & 0xf0000) >> 16
} 

// extract capture flag
func get_move_capture(move int) int {
	return move & 0x100000
}

// extract double pawn push flag
func get_move_double(move int) int {
	return move & 0x200000
}

// extract enpassant flag
func get_move_enpassant(move int) int {
	return move & 0x400000
}

// extract castling flag
func get_move_castling(move int) int {
	return move & 0x800000
}

// add move to move list
func add_move(move_list *Moves, move int) {
	// store move
	move_list.moves[move_list.count] = move

	// increment pointer
	move_list.count++
}

// print move (for UCI purposes)
func print_move(move int) {
	if get_move_promoted(move) > 0 {
	fmt.Print(square_to_coordinates[get_move_source(move)],
			  square_to_coordinates[get_move_target(move)],
		      promoted_pieces[get_move_promoted(move)])
	} else {
		fmt.Print(square_to_coordinates[get_move_source(move)],
				  square_to_coordinates[get_move_target(move)])
	}
}

// print move list (for debuggin purposes)
func print_move_list(move_list Moves) {
	if move_list.count == 0 {
		fmt.Print("\nNo moves in move list\n")
		return
	} else {
		fmt.Print("\n     move    piece     capture   double    enpass    castling\n\n")
		for move_count := 0; move_count < move_list.count; move_count++ {
			var move int = move_list.moves[move_count]

			fmt.Print("     ", square_to_coordinates[get_move_source(move)], square_to_coordinates[get_move_target(move)])
			if get_move_promoted(move) > 0 { fmt.Printf("%c", promoted_pieces[get_move_promoted(move)]) } else { fmt.Print(" ") }
			fmt.Printf("     %c", ascii_pieces[get_move_piece(move)])
			if get_move_capture(move) > 0 { fmt.Print("          1") } else { fmt.Print("          0") }
			if get_move_double(move) > 0 { fmt.Print("        1") } else { fmt.Print("        0") }
			if get_move_enpassant(move) > 0 { fmt.Print("         1") } else { fmt.Print("         0") }
			if get_move_castling(move) > 0 { fmt.Print("          1") } else { fmt.Print("          0") }
			fmt.Print("\n")
		}
	}

	fmt.Print("\n\n     Total number of moves: ", move_list.count, "\n\n")
}
