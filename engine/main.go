package main

import (
	"fmt"
)

func main() {
	fmt.Print("Go Chess engine\n")

	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.pos.parse_fen("r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ", 0)

	move := uci.parse_move("b7a8q")

	print_move(move)
	if move > 0 {
		uci.pos.make_move(move, all_moves)
		print_board(uci.pos)
	} else {
		fmt.Println("Ilegal move")
	}
}
