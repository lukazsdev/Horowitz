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

	pos.parse_fen("4k2r/P4q1p/P2p2p1/Q1pP4/2P5/P7/6PP/R3K3 b Qk - 0 1", 0)
	print_board(pos)
}