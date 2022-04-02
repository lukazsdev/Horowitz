package main

import "fmt"

func main() {
	initialize_lookup_tables()

	// UCI testing
	//var uci UCIInterface
	//uci.UCILoop()

	var pos Position
	pos.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ")
	print_board(pos)
}
