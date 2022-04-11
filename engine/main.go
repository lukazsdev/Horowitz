package main

//import "fmt"

func main() {
	// UCI testing
	var uci UCIInterface
	//uci.UCILoop()

	// Debugging
	uci.boot_engine()
	var pos Position
	var perft Perft
	// var search Search
	pos.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ")
	print_board(pos)

	perft.test(pos, 4)
}

