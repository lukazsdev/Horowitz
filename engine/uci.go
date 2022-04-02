package main

//import "fmt"

// struct for UCI protocol
type UCIInterface struct {
	pos Position
}

// parse move input from GUI 
func (uci *UCIInterface) parse_move(move_string string) Move {
	moves := uci.pos.generate_moves()

	// parse from and to squares
	source_square := (int(move_string[1])-49)*8 + (int(move_string[0])-97)
	target_square := (int(move_string[3])-49)*8 + (int(move_string[2])-97)

	for i := 0; i < moves.count; i++ {
		// initialize move
		move := moves.list[i]

		// check if move is available within move list
		if source_square == move.get_move_source() && target_square == move.get_move_target() {
			// initialize promoted piece
			promoted_piece := move.get_move_promoted()

			// check if promoted piece is available
			if promoted_piece > 0 {
				// loop over all possible promotions
				for piece := white_knight; piece <= white_queen; piece++ {
					if (promoted_piece==piece || promoted_piece==piece+6) && move_string[4]==promoted_pieces[piece] {
						// return legal move
						return move
					}
				}

				// continue loop on possibly wrong promotions
				continue
			}

			// return legal move
			return move
		}
	} 

	// return ilegal move
	return 0
}

