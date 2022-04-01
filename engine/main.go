package main

import (
	"fmt"
)

func main() {
	fmt.Print("Go Chess engine\n")

	initialize_lookup_tables()

	pos := Position{
		side_to_move:         0,
		castling_rights:      0,
		enpassant_square: NO_SQ,
	}

	pos.parse_fen(tricky_position, 0)
	print_board(pos)

	var move_list Moves

	pos.generate_moves(&move_list)

	print_move_list(move_list)
}