package main

// number of positions reached during testing
var nodes uint64

// perft driver
func perft_driver(pos Position, depth int) {
	if depth == 0 {
		nodes++
		return
	}

	moves := pos.generate_moves()
	for i := 0; i < moves.count; i++ {
		// preserve board state
		pos.copy_board()

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], all_moves) {
			continue
		} 
	     
		// call perft recursively
		perft_driver(pos, depth - 1)

		// return to prev board state
		pos.take_back()

	}
}