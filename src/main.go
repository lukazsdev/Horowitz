package main

import (
	"fmt"
)

type Test struct {
	count int
}

func main() {
	fmt.Print("Go chess engine\n\n")
	initialize_lookup_tables()

	parse_fen(tricky_position, 0)
	print_board()

	var move_list Moves

	var move int = encode_move(SQ_D7, SQ_E8, P, Q, 1, 0, 0, 0)

	add_move(&move_list, move)
	print_move_list(move_list)
}