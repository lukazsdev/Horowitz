package main

import "fmt"

func main() {
	// UCI testing
	var uci UCIInterface
	//uci.UCILoop()

	// Debugging
	uci.boot_engine()
	var pos Position
	//var perft Perft
	var search Search
	pos.parse_fen(start_position)
	print_board(pos)

	// TT debugging
	search.TT.init()

	search.TT.store(pos.hash_key, 1, hash_flag_beta, 45, 0)

	score := search.TT.read(pos.hash_key, 20, 30, 0, 1)

	fmt.Println("score: ", score)
}

