package main

//import "fmt"

func main() {
	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.UCILoop()


	// Debugging
	//var pos Position
	//pos.parse_fen(start_position)
	//print_board(pos)
}

