package main

//import "fmt"

func main() {
	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.UCILoop()


	// Debugging
	/*
	var pos Position
	pos.parse_fen("8/8/8/8/8/8/5PpP/6KR b - - 0 1")
	print_board(pos)

	moves := pos.generate_moves()
	print_move_list(moves)
	*/
}

