package main

import "fmt"

type Search struct {
	ply   uint8
	nodes uint64

	best_move Move
}

func (search *Search) negamax(pos Position, alpha, beta, depth int) int {
	if depth == 0 {
		eval := evaluate(pos)
		return eval
	}

	// best move so far
	var best_so_far Move

	// increment nodes
	search.nodes++

	// old value of alpha
	old_alpha := alpha

	// move list
	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {
		// preserve board state
		pos.copy_board()

		// increment half move counter
		search.ply++

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], all_moves) {
			search.ply--
			continue
		} 

		score := -search.negamax(pos, -beta, -alpha, depth - 1)

		// take back move
		pos.take_back()

		// decrement ply
		search.ply--

		// fail-hard beta cutoff
		if score >= beta {
			// node (move) fails high
			return beta 
		}

		// found better move
		if score > alpha {
			// PV node (move)
			alpha = score

			// if root move
			if search.ply == 0 {
				best_so_far = moves.list[i]
			}
		}

	}

	if old_alpha != alpha {
		search.best_move = best_so_far
	}

	// node (move) fails low
	return alpha
}

func (search *Search) position(pos Position, depth int) {
	search.ply = 0
	search.nodes = 0

	search.negamax(pos, -50000, 50000, depth)

	fmt.Print("bestmove ")
	print_move(search.best_move)
}