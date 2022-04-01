package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Print("Go Chess engine\n")

	initialize_lookup_tables()

	pos := Position{
		side_to_move:         0,
		castling_rights:      0,
		enpassant_square: NO_SQ,
	}

	pos.parse_fen(start_position, 0)
	print_board(pos)

	start := time.Now()

	//perft_driver(pos, 6)

	fmt.Println("Nodes: ", nodes)
	fmt.Println("Time: ", time.Since(start).Milliseconds(), "ms")
}