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

	pos.parse_fen(p6, 0)
	print_board(pos)

	perft_test(pos, 1)
	perft_test(pos, 2)
	perft_test(pos, 3)
	perft_test(pos, 4)
	perft_test(pos, 5)
}