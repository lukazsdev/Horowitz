package main

import "fmt"


func main() {
	fmt.Print("Go chess engine\n\n")
	initialize_lookup_tables()

	parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ", 0)
	print_board()

	var move_list Moves

	generate_moves(&move_list)
	print_move_list(move_list)
}