package main

import "fmt"

func search_position(pos Position, depth int) {

	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {
		// preserve board state
		pos.copy_board()

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], all_moves) {
			continue
		} 
	     
		fmt.Print("bestmove ")
		print_move(moves.list[i])

		break
	}
}