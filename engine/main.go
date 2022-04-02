package main

import (
	"fmt"
)

func main() {
	fmt.Print("Go Chess engine\n")

	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.parse_position("position startpos")
	print_board(uci.pos)
}
