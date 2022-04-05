package main

import "fmt"

type Search struct {
	ply      int
	nodes uint64

	best_move Move
}

const (
	infinity   int = 50000
	mate_value int = 49000
	mate_score int = 48000
)

func (search *Search) quiescence(pos Position, alpha, beta int) int {
	// evaluation
	evaluation := evaluate(pos)

	// increment nodes
	search.nodes++

	// fail-hard beta cutoff
	if evaluation >= beta {
		// node (move) fails high
		return beta 
	}

	// found better move
	if evaluation > alpha {
		// PV node (move)
		alpha = evaluation
	}

	// move list
	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {

		// preserve board state
		pos.copy_board()

		// increment half move counter
		search.ply++

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], only_captures) {
			search.ply--
			continue
		} 

		// recursively call quiescence
		score := -search.quiescence(pos, -beta, -alpha)

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
		}

	}

	// node fails low
	return alpha
}

func (search *Search) negamax(pos Position, alpha, beta, depth int) int {
	if depth == 0 {
		// search only captures
		return search.quiescence(pos, alpha, beta)
	}

	// current side to move and opposite side
	var our_side, their_side = pos.side_to_move, other_side(pos.side_to_move)

	// is king in check
	king_square := pos.bitboards[get_piece_type(King, our_side)].bsf()
	in_check := is_square_attacked(king_square, their_side, pos)

	// increment nodes
	search.nodes++

	// old value of alpha
	old_alpha := alpha

	// best move so far
	var best_so_far Move

	// legal moves counter
	legal_moves := 0

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

		// increment legal moves
		legal_moves++

		// recursively call negamax
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

	// no legal moves in current position
	if legal_moves == 0 {
		// king is in check
		if in_check == true {
			return -mate_value + search.ply
		}
		// if not, then statelmate
		return 0
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

	fmt.Println("init search")

	score := search.negamax(pos, -infinity, infinity, depth)

	fmt.Println("end the search")

	print_move(search.best_move)
	if search.best_move > 0 {
		fmt.Println("info score cp", score, "depth", depth, "nodes", search.nodes)

		fmt.Print("bestmove ")
		print_move(search.best_move)
	}
}