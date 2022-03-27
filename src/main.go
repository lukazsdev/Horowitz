package main

import (
	"fmt"
)

func main() {
	fmt.Print("Go chess engine\n\n")
	initialize_lookup_tables()

	parse_fen(start_position, 0)
	print_board()

	// test bitboard
	var tt uint64 = BITBOARDS[b]
	lsb := pop_lsb(&tt)
	print_bitboard(tt)
	fmt.Println(lsb)

}

/*
- test queen attacks move gen
- implement bit count function
- implement get lsb function
*/