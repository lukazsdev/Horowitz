package main

//import "fmt"

// struct for representing chess position
type Position struct {
	// piece and occupied bitboards
	bitboards [12]Bitboard
	occupied   [3]Bitboard

	// main info of position
	side_to_move     uint8
	castling_rights  uint8
	enpassant_square   int

	// store previous state
	store_info       State
}

// struct for copying/storing previous states
type State struct {
	bitboards_copy [12]Bitboard
	occupied_copy   [3]Bitboard

	side_to_move_copy     uint8
	castling_rights_copy  uint8
	enpassant_square_copy   int
}

// make move
func (pos *Position) make_move(move Move, move_flag uint8) bool {
	// all moves
	if move_flag == all_moves {
		// preserve board state
		pos.copy_board()

		// extract info from move
		source_square  := move.get_move_source()
		target_square  := move.get_move_target()
		piece          := move.get_move_piece()
		promoted_piece := move.get_move_promoted()
		capture        := move.get_move_capture()
		double_push    := move.get_move_double()
		enpass         := move.get_move_enpassant()
		castling       := move.get_move_castling()
		
		// make the move
		pos.bitboards[piece].pop_bit(source_square)
		pos.bitboards[piece].set_bit(target_square)

		// extract sides from position
		var our_side, their_side uint8 = pos.side_to_move, other_side(pos.side_to_move)

		// handle captures
		if capture > 0 {
			// loop over bitboards of the opposite side
			for bb_piece := 6 * their_side; bb_piece <= 6 * their_side + white_king; bb_piece++ {
				// check if there is a piece on the target square
				if pos.bitboards[bb_piece].get_bit(target_square) > 0 {
					// remove the piece from target square
					pos.bitboards[bb_piece].pop_bit(target_square)
					break
				}
			}
		}

		// handle promotions
		if promoted_piece > 0 {
			// remove pawn from target square
			pos.bitboards[6 * our_side + white_pawn].pop_bit(target_square)

			// place promoted piece
			pos.bitboards[promoted_piece].set_bit(target_square)
		}

		// handle enpassant captures
		if enpass > 0 {
			if our_side == white {
				// erase pawn that made double push
				pos.bitboards[black_pawn].pop_bit(target_square - 8)
			} else {
				// erase pawn that made double push
				pos.bitboards[white_pawn].pop_bit(target_square + 8)
			}
		}

		// reset enpassant square
		pos.enpassant_square = NO_SQ

		// handle double pawn push and set enpassant squares
		if double_push > 0 {
			if our_side == white {
				pos.enpassant_square = target_square - 8
			} else {
				pos.enpassant_square = target_square + 8
			}
		}

		// handle castling
		if castling > 0 {
			switch target_square {
			// white kingside castle
			case SQ_G1:
				pos.bitboards[white_rook].pop_bit(SQ_H1)
				pos.bitboards[white_rook].set_bit(SQ_F1)
				break
			
			// white queenside castle
			case SQ_C1:
				pos.bitboards[white_rook].pop_bit(SQ_A1)
				pos.bitboards[white_rook].set_bit(SQ_D1)
				break
			
			// black kingside castle
			case SQ_G8:
				pos.bitboards[black_rook].pop_bit(SQ_H8)
				pos.bitboards[black_rook].set_bit(SQ_F8)
				break
			
			// black queenside castle
			case SQ_C8:
				pos.bitboards[black_rook].pop_bit(SQ_A8)
				pos.bitboards[black_rook].set_bit(SQ_D8)
				break
			}
		}

		// update castling rights
		pos.castling_rights &= square_to_castling_rights[source_square]
		pos.castling_rights &= square_to_castling_rights[target_square]

		// reset occupancies
		for i := range pos.occupied { pos.occupied[i] = 0 }

		for bb_piece := white_pawn; bb_piece <= white_king; bb_piece++ {
			pos.occupied[white] |= pos.bitboards[bb_piece]
		}
		for bb_piece := black_pawn; bb_piece <= black_king; bb_piece++ {
			pos.occupied[black] |= pos.bitboards[bb_piece]
		}

		pos.occupied[both] |= pos.occupied[white] | pos.occupied[black]

		// switch side
		pos.side_to_move = their_side

		// bug with bsf() => returns 64 when empty board

		// make sure king has not been exposed to a check
		if is_square_attacked(pos.bitboards[6 * our_side + white_king].bsf(), their_side, *pos) {
			// go back to original board state
			pos.take_back()

			// return ilegal move
			return false
		} else {
			// return legal move
			return true
		}

	// capture moves
	} else {

		if move.get_move_capture() > 0 {
			pos.make_move(move, all_moves)

		} else {
			// bug does reach this part
			return false
		}
	}

	return true

}


// copy board
func (pos *Position) copy_board() {
	pos.store_info = State{
		bitboards_copy:        pos.bitboards,
		occupied_copy:         pos.occupied,

		side_to_move_copy:     pos.side_to_move,
		castling_rights_copy:  pos.castling_rights,
		enpassant_square_copy: pos.enpassant_square,
	}
}

// take back
func (pos *Position) take_back() {
	*pos = Position{
		bitboards:        pos.store_info.bitboards_copy,
		occupied:         pos.store_info.occupied_copy,

		side_to_move:     pos.store_info.side_to_move_copy,
		castling_rights:  pos.store_info.castling_rights_copy,
		enpassant_square: pos.store_info.enpassant_square_copy,
	}
}

// parse FEN string
func (pos *Position) parse_fen(fen string) {
	// general purpose pointer to FEN string
	var ptr int = 0

	// reset bitboards
	for i := range pos.bitboards { pos.bitboards[i] = 0 }
	for i := range pos.occupied { pos.occupied[i] = 0 }

	// reset position info
	pos.side_to_move = 0
	pos.enpassant_square = NO_SQ
	pos.castling_rights = 0

	// parsing FEN and overwriting bitboards
	for rank := 7; rank >= 0; rank-- {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if (fen[ptr]>=97 && fen[ptr]<=122)||(fen[ptr]>=65 && fen[ptr]<=90) {
				piece := char_to_piece[fen[ptr]]
				pos.bitboards[piece].set_bit(square)
				ptr++
			}

			if fen[ptr] >= '0' && fen[ptr] <= '9' {
				offset := int(fen[ptr]) - 48
				
				var piece uint8 = no_piece

				for bb_piece := white_pawn; bb_piece <= black_king; bb_piece++ {
					if pos.bitboards[bb_piece].get_bit(square) > 0 {
						piece = bb_piece
					}
				}

				if piece == no_piece {
					file = file - 1
				}
				

				file += offset
				ptr++
			}

			if fen[ptr] == '/' {
				ptr++
			}
		}
	}

	ptr++
	if fen[ptr] == 'w' {
		pos.side_to_move = white
	} else {
		pos.side_to_move = black
	}

	ptr += 2

	for fen[ptr] != ' ' {
		switch (fen[ptr]) {
		case 'K':
			pos.castling_rights |= white_kingside_castle
		case 'Q':
			pos.castling_rights |= white_queenside_castle
		case 'k':
			pos.castling_rights |= black_kingside_castle
		case 'q':
			pos.castling_rights |= black_queenside_castle
		}
		ptr++
	}

	ptr++

	if fen[ptr] != '-' {
		file := int(fen[ptr]) - 97
		rank := int(fen[ptr+1]) - 49
		pos.enpassant_square = rank*8 + file
	} else {
		pos.enpassant_square = NO_SQ
	}

	for piece := white_pawn; piece <= white_king; piece++ {
		pos.occupied[white] = pos.occupied[white] | pos.bitboards[piece]
	}
	for piece := black_pawn; piece <= black_king; piece++ {
		pos.occupied[black] = pos.occupied[black] | pos.bitboards[piece]
	}

	pos.occupied[both] = pos.occupied[white] | pos.occupied[black]
}