package main

import (
    "fmt"
    "time"
)

// structure for engine search
type Search struct {
    timer TimeManager
    TT    TranspositionTable

    nodes uint64
    ply      int

    killers  [2][max_ply]Move
    history      [12][64]Move

    pv_length [max_ply]int
    pv_table  [max_ply][max_ply]Move

    follow_pv, score_pv  uint8

    repetitions_table [1000]uint64
    repetitions_index int
}

const (
    // maximum ply
    max_ply    int = 64

    // constant values for search
    infinity   int = 10000
    checkmate  int =  9000

    // search pruning parameters
    //static_nmp_margin int = 120
    full_depth_moves  int = 4
    reduction_limit   int = 3
    window_size       int = 50

    // no move (null move)
    null_move Move = 0
)

//var futility_margins = [9]int{0, 100, 160, 220, 280, 340, 400, 460, 520}
//var late_move_pruning_margins = [4]int{0, 8, 12, 24}

func (search *Search) quiescence(pos Position, alpha, beta int) int {
    // evaluation
    evaluation := evaluate(pos)

    // increment nodes
    search.nodes++

    // prevent index out of bounds of array
    if search.ply > max_ply - 1 {
        return evaluation
    }

    // every 2048 nodes, check if time is up
    if (search.nodes & 2047) == 0 {
        search.timer.check()
    }

    // stop search if time is up
    if search.timer.stop == true {
        return 0
    }

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

    // sort move list
    search.sort_moves(pos, &moves)

    for i := 0; i < moves.count; i++ {
        // get current move in move list
        move := moves.list[i]

        // preserve board state
        pos.copy_board()

        // increment half move counter
        search.ply++

        // increment repetition index and store current hash key
        search.repetitions_index++
        search.repetitions_table[search.repetitions_index] = pos.hash_key

        // skip if move is ilegal
        if !pos.make_move(move, only_captures) {
            search.repetitions_index--
            search.ply--
            continue
        } 

        // recursively call quiescence
        score := -search.quiescence(pos, -beta, -alpha)

        // decrement ply
        search.ply--

        // decrement repetitions index
        search.repetitions_index--

        // take back move
        pos.take_back()

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
    // increment nodes
    search.nodes++

    // initialize PV length
    search.pv_length[search.ply] = search.ply

    // prevent index out of bounds of array
    if search.ply > max_ply - 1 {
        return evaluate(pos)
    }

    // every 2048 nodes, check if time is up
    if (search.nodes & 2047) == 0 {
        search.timer.check()
    }

    // stop search if time is up
    if search.timer.stop == true {
        return 0
    }

    // score of current position
    score := 0

    // define hash flag
    hash_flag := hash_flag_alpha

    // current side to move and opposite side
    var our_side, their_side = pos.side_to_move, other_side(pos.side_to_move)

    // is king in check
    king_square := pos.bitboards[get_piece_type(King, our_side)].bsf()
    in_check := is_square_attacked(king_square, their_side, pos)
    has_non_pawn_material := pos.non_pawn_material()

    // check if current node is PV node or not
    is_pv_node := beta - alpha > 1
    //can_futility_prune := false

    // increase depth if king in check
    if in_check == true {
        depth++
    }

    if depth == 0 {
        // search only captures
        return search.quiescence(pos, alpha, beta)
    }

    // if position repetition occurs
    if search.ply > 0 && search.is_repetition(pos) {
        // return draw score
        return 0
    }

    // read hash entry if we're not in a root ply and hash entry is available
    // and current node is not a PV node
    score = search.TT.read(pos.hash_key, alpha, beta, search.ply, uint8(depth))
    if search.ply > 0 && score != no_hash_entry && is_pv_node == false {
        // if the move has already been searched (hence has a value)
        // we just return the score for this move without searching it
        return score
    }
    
    
    /*
    // static null move pruning
    if in_check == false && is_pv_node == false && abs(beta) < checkmate {
        // if current material - score margin is still good, prune branch
        static_score := evaluate(pos)
        score_margin := static_nmp_margin * depth

        if static_score - score_margin >= beta {
            return beta
        }
    }
    */
    

    // null move pruning (only done if we don't have non pawn material)
    if depth >= 3 && in_check == false && search.ply > 0 && has_non_pawn_material == true {
        // preserve board state
        pos.copy_board()

        // increment ply
        search.ply++;

        // increment repetition index and store current hash key
        search.repetitions_index++
        search.repetitions_table[search.repetitions_index] = pos.hash_key

        // hash enpassant if available
        if pos.enpassant_square != NO_SQ {
            pos.hash_key ^= Zob.enpassant_keys[pos.enpassant_square]
        }

         // reset enpassant square
        pos.enpassant_square = NO_SQ

        // switch the side (give opponent extra move)
        pos.side_to_move = other_side(pos.side_to_move)

        // hash the side
        pos.hash_key ^= Zob.side_key

        // search moves with reduced depth to find beta cutoffs 
        score = -search.negamax(pos, -beta, -beta + 1, depth - 1 - 2)

        // decrement ply
        search.ply--

        // decrement repetitions index
        search.repetitions_index--
            
        // restore board state
        pos.take_back()

        // stop search if time is up
        if search.timer.stop == true {
            return 0
        }

        // fail-hard beta cutoff
        if (score >= beta) {
            // node (position) fails high
            return beta
        }
    }

    /*
    // futility pruning
    if depth <= 8 && is_pv_node == false && in_check == false && alpha < checkmate {
		static_score := evaluate(pos)
		if static_score + futility_margins[depth] <= alpha {
			can_futility_prune = true
		}
	}
    */
    

    // legal moves counter
    legal_moves := 0
    
    // move list
    moves := pos.generate_moves()

    // if we are following PV line
    if search.follow_pv == 1 {
        // enable PV move scoring
        search.enable_pv_scoring(moves)
    }

    // sort move list
    search.sort_moves(pos, &moves)

    // number of moves searched in move list
    moves_searched := 0

    for i := 0; i < moves.count; i++ {
        // get current move in move list
        move := moves.list[i]

        // preserve board state
        pos.copy_board()

        // increment half move counter
        search.ply++

        // increment repetitions index and store current hash key
        search.repetitions_index++
        search.repetitions_table[search.repetitions_index] = pos.hash_key
        
        // skip if move is ilegal
        if !pos.make_move(move, all_moves) {
            search.repetitions_index--
            search.ply--
            continue
        } 

        // increment legal moves
        legal_moves++

        
        /*
        // late move pruning
        if depth <= 3 && !is_pv_node && !in_check && legal_moves > late_move_pruning_margins[depth] {
			tactical := in_check || (move.get_move_promoted() > 0)
			if !tactical {
				pos.take_back()
				search.repetitions_index--
                search.ply--
                continue
			}
		}
        */
        

        /*
        // futility pruning
        if can_futility_prune && legal_moves > 1 {
			tactical := in_check || (move.get_move_capture() > 0) || (move.get_move_promoted() > 0)
			if !tactical {
                pos.take_back()
				search.repetitions_index--
                search.ply--
                continue
			}
		}
        */
        

        // full depth search
        if moves_searched == 0 {
            // recursively call negamax normally
            score = -search.negamax(pos, -beta, -alpha, depth - 1)
        } else {
            // condition to consider LMR
            if moves_searched >= full_depth_moves && 
                depth >= reduction_limit && in_check == false &&
                 move.get_move_capture() == 0 && move.get_move_promoted() == 0 {
                // search current move with reduced depth
                score = -search.negamax(pos, -alpha - 1, -alpha, depth - 2)
            } else {
                // hack to ensure that full-depth search is done
                score = alpha + 1
            }

            // PV search
            if score > alpha {
                score = -search.negamax(pos, -alpha - 1, -alpha, depth - 1)
                // check for failure
                if (score > alpha) && (score < beta) {
                    score = -search.negamax(pos, -beta, -alpha, depth - 1)
                }
            }
        }

        // decrement ply
        search.ply--

        // decrement repetitions index
        search.repetitions_index--

        // take back move
        pos.take_back()

        // increment moves searched
        moves_searched++

        // fail-hard beta cutoff
        if score >= beta {
            // store hash entry with the score equal to beta
            search.TT.store(pos.hash_key, uint8(depth), hash_flag_beta, beta, search.ply)

            // only quiet moves
            if move.get_move_capture() == 0 {
                // store killer moves
                search.killers[1][search.ply] = search.killers[0][search.ply]
                search.killers[0][search.ply] = move
            }

            // node (move) fails high
            return beta 
        }

        // found better move
        if score > alpha {
            // switch hash flag from storing score for fail-low node
            // to the one storing score for PV node
            hash_flag = hash_flag_exact
            
            // only quiet movesgo
            if move.get_move_capture() == 0 {
                // store history moves
                search.history[move.get_move_piece()][move.get_move_target()] += Move(depth)
            }

            // PV node (move)
            alpha = score

            // write PV move to table
            search.pv_table[search.ply][search.ply] = move

            // copy move from deeper ply into current ply's line
            for next_ply := search.ply + 1; next_ply < search.pv_length[search.ply + 1]; next_ply++ {
                search.pv_table[search.ply][next_ply] = search.pv_table[search.ply + 1][next_ply]
            }

            // adjust PV length
            search.pv_length[search.ply] = search.pv_length[search.ply + 1]
        }
    }

    // no legal moves in current position
    if legal_moves == 0 {
        // king is in check
        if in_check == true {
            return -checkmate + search.ply
        }
        // if not, then statelmate
        return 0
    }

    // store hash entry with the score equal to alpha
    search.TT.store(pos.hash_key, uint8(depth), hash_flag, alpha, search.ply)

    // node (move) fails low
    return alpha
}

func (search *Search) position(pos Position, depth int) {
    // start a search timer
    start_time := time.Now()

    // initialize best move to no move
    best_move := null_move

    // last iteration score
    prev_score := 0

    // start search timer
    search.timer.start()

    // reset search info
    search.reset_info()

    // initial alpha, beta bounds
    alpha := -infinity
    beta  :=  infinity

    // iterative deepening
    for current_depth := 1; current_depth <= depth; current_depth++ {
        // enable follow PV flag
        search.follow_pv = 1

        // find best move within position
        score := search.negamax(pos, alpha, beta, current_depth)

        // end a search timer
        end_time := time.Since(start_time)

        if search.timer.stop == true {
            if best_move == null_move && current_depth == 1 {
                best_move = search.pv_table[0][0]
            }
            break
        }

        // adjust aspiration window technique
        if (score <= alpha) || (score >= beta) {
            alpha = -infinity    
            beta  = infinity      
            continue;
        }

        alpha = score - window_size
        beta = score + window_size

        // If the score between this current iteration and the last iteration drops,
        // take more time on the current search to make sure we find the best move.
        if current_depth > 1 && prev_score > score && prev_score-score >= 30 {
            search.timer.set_soft_time_for_move(search.timer.soft_time_for_move * 13 / 10)
        }

        // save current best move
        best_move = search.pv_table[0][0]


        // if PV is available
        if search.pv_length[0] > 0 {
            // print search info
            if score > -checkmate && score < -(checkmate-100) {
                fmt.Print("info score mate ", -(score + checkmate) / 2 - 1, " depth ", current_depth)
                fmt.Print(" nodes ", search.nodes, " time ", end_time.Milliseconds(), " pv ")
            } else if score > (checkmate-100) && score < checkmate {
                fmt.Print("info score mate ", (checkmate - score) / 2 + 1, " depth ", current_depth)
                fmt.Print(" nodes ", search.nodes, " time ", end_time.Milliseconds(), " pv ")
            } else {
                fmt.Print("info score cp ", score, " depth ", current_depth)
                fmt.Print(" nodes ", search.nodes, " time ", end_time.Milliseconds(), " pv ")
            }
        }

        // loop over moves within PV line
        for count := 0; count < search.pv_length[0]; count++ {
            // print PV move
            print_move(search.pv_table[0][count])
            fmt.Print(" ")
        }

        fmt.Print("\n")

        // set previous score to current score
        prev_score = score
    }

    fmt.Print("bestmove ")
    print_move(best_move)
    fmt.Print("\n")
}

func (search *Search) reset_info() {
    // reset search info
    search.ply       = 0
    search.nodes     = 0
    search.follow_pv = 0
    search.score_pv  = 0

    // reset killers array
    for i := 0; i < 2; i++ {
        for j := 0; j < 64; j++ {
            search.killers[i][j] = Move(0)
        }
    }

    // reset history array
    for i := 0; i < 12; i++ {
        for j := 0; j < 64; j++ {
            search.history[i][j] = Move(0)
        }
    }

    // reset PV length array
    for i := 0; i < 64; i++ {
        search.pv_length[i] = 0
    }

    // reset PV table array
    for i := 0; i < 64; i++ {
        for j := 0; j < 64; j++ {
            search.pv_table[i][j] = Move(0)
        }
    }
    
}

func (search *Search) is_repetition(pos Position) bool {
    // loop over repetitions array
    for idx := 0; idx < search.repetitions_index; idx++ {
        // if current repetitions hash key equals pos hash key
        if search.repetitions_table[idx] == pos.hash_key {
            // found a repetition
            return true
        }
    }

    // no repetition
    return false
}