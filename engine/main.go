package main

import (
	"fmt"
)

func main() {
	fmt.Print("Go Chess engine\n")

	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.parse_position("position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4 d7d5 e4d5")
	print_board(uci.pos)
}
