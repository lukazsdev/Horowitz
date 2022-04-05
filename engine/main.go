package main

//import "fmt"

func main() {
	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.UCILoop()

	/*

	var pos Position
	pos.parse_fen("3nq3/8/8/1pP5/8/8/8/4K3 w - b6 0 1")
	print_board(pos)

	


	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {
		move := moves.list[i]

		is_legal := pos.make_move(move, all_moves)

		if is_legal == false {
			continue
		}

		print_move(move)

		//print_board(pos)
		
		pos.take_back()
	}

	print_board(pos)

	*/


}
