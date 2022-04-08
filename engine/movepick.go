package main

//import "fmt"

// MVV LVA [attacker][victim]
var MVV_LVA = [12][12]int {
 	{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

	{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},
}


// score move 
func (search *Search) score_move(pos Position, move Move) int {
	// assign score to move
	move_score := 0

	// check if move is a capture
	if move.get_move_capture() > 0 {
		var attacker, victim uint8
		// check if move is enpassant capture
		if move.get_move_enpassant() > 0 {
			attacker = get_piece_type(Pawn, pos.side_to_move)
			victim   = get_piece_type(Pawn, other_side(pos.side_to_move))
		} else {
			attacker = move.get_move_piece()
			victim   = pos.board[move.get_move_target()]
		}

		move_score += MVV_LVA[attacker][victim] + 10000
	} else {
		// score first killer move
		if search.killers[0][search.ply] == move {
			move_score += 9000
		// score second killer move
		} else if search.killers[1][search.ply] == move {
			move_score += 8000
		// score history move
		} else {
			move_score += int(search.history[move.get_move_piece()][move.get_move_target()])
		}
	}

	return move_score
}

func (search *Search) sort_moves(pos Position, moves *MoveList) {
	// move scores
	move_scores := make([]int, moves.count)

	// score all moves in move list
	for count := 0; count < moves.count; count++ {
		move_scores[count] = search.score_move(pos, moves.list[count])
	}

	// sort moves based on score
	for current_move := 0; current_move < moves.count; current_move++ {
		for next_move := current_move + 1; next_move < moves.count; next_move++ {
			// compare current and next move scores 
			if move_scores[current_move] < move_scores[next_move] {
				// swap scores
				temp_score := move_scores[current_move]
				move_scores[current_move] = move_scores[next_move]
				move_scores[next_move] = temp_score

				// swap moves
				temp_move := moves.list[current_move]
				moves.list[current_move] = moves.list[next_move]
				moves.list[next_move] = temp_move
			}
		}
	}
}