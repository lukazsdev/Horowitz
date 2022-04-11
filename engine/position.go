package main

//import "fmt"

// struct for representing chess position
type Position struct {
	// zobrist hash of position
	hash_key uint64

	// piece and occupied bitboards
	bitboards [12]Bitboard
	occupied   [3]Bitboard

	// piece on given square
	board     [64]uint8

	// main info of position
	side_to_move     uint8
	castling_rights  uint8
	enpassant_square   int

	// store previous state
	store_info       State
}

// struct for copying/storing previous states
type State struct {
	hash_key_copy uint64

	bitboards_copy [12]Bitboard
	occupied_copy   [3]Bitboard

	// piece on given square
	board_copy     [64]uint8

	side_to_move_copy     uint8
	castling_rights_copy  uint8
	enpassant_square_copy   int
}

// puts a piece on a given square
func (pos *Position) place_piece(piece uint8, square int) {
	pos.board[square] = piece
	pos.bitboards[piece] |= SQUARE_BB[square]
}

// remove a piece from given square
func (pos *Position) remove_piece(piece uint8, square int) {
	pos.board[square] = no_piece
	pos.bitboards[piece] &= ^SQUARE_BB[square]; 
}

// move a piece from one square to another
func (pos *Position) move_piece(piece uint8, source_square, target_square int) {
	pos.bitboards[piece] ^= (SQUARE_BB[source_square] | SQUARE_BB[target_square])
	pos.board[target_square] = pos.board[source_square]
	pos.board[source_square] = no_piece
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
		pos.move_piece(piece, source_square, target_square)

		// hash piece  
	    pos.hash_key ^= Zob.piece_keys[piece][source_square]
        pos.hash_key ^= Zob.piece_keys[piece][target_square]

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

					// remove the piece from hash key
                    pos.hash_key ^= Zob.piece_keys[bb_piece][target_square]
					break
				}
			}
		}

		// handle promotions
		if promoted_piece > 0 {
			// remove pawn from target square
			pos.bitboards[6 * our_side + white_pawn].pop_bit(target_square)

			// remove promoted piece from hash key
		    pos.hash_key ^= Zob.piece_keys[6 * our_side + white_pawn][target_square]

			// place promoted piece
			pos.bitboards[promoted_piece].set_bit(target_square)

			// add promoted piece into the hash key
            pos.hash_key ^= Zob.piece_keys[promoted_piece][target_square]
		}

		// handle enpassant captures
		if enpass > 0 {
			if our_side == white {
				// erase pawn that made double push
				pos.bitboards[black_pawn].pop_bit(target_square - 8)

				// remove pawn from hash key
                pos.hash_key ^= Zob.piece_keys[black_pawn][target_square - 8]

			} else {
				// erase pawn that made double push
				pos.bitboards[white_pawn].pop_bit(target_square + 8)

				// remove pawn from hash key
                pos.hash_key ^= Zob.piece_keys[white_pawn][target_square + 8]
			}
		}

		// hash enpassant if available (remove enpassant square from hash key )
        if pos.enpassant_square != NO_SQ {
		    pos.hash_key ^= Zob.enpassant_keys[pos.enpassant_square]
		}

		// reset enpassant square
		pos.enpassant_square = NO_SQ

		// handle double pawn push and set enpassant squares
		if double_push > 0 {
			if our_side == white {
				// set enpassant square
				pos.enpassant_square = target_square - 8

				// hash enpassant
                pos.hash_key ^= Zob.enpassant_keys[target_square - 8]
			} else {
				// set enpassant square
				pos.enpassant_square = target_square + 8

				// hash enpassant
                pos.hash_key ^= Zob.enpassant_keys[target_square + 8]
			}
		}

		// handle castling
		if castling > 0 {
			switch target_square {
			// white kingside castle
			case SQ_G1:
			    pos.move_piece(white_rook, SQ_H1, SQ_F1)

				// hash rook
			    pos.hash_key ^= Zob.piece_keys[white_rook][SQ_H1]
                pos.hash_key ^= Zob.piece_keys[white_rook][SQ_F1]
				break
			
			// white queenside castle
			case SQ_C1:
			    pos.move_piece(white_rook, SQ_A1, SQ_D1)

				// hash rook
			    pos.hash_key ^= Zob.piece_keys[white_rook][SQ_A1]
                pos.hash_key ^= Zob.piece_keys[white_rook][SQ_D1]
				break
			
			// black kingside castle
			case SQ_G8:
				pos.move_piece(black_rook, SQ_H8, SQ_F8)

				// hash rook
			    pos.hash_key ^= Zob.piece_keys[black_rook][SQ_H8]
                pos.hash_key ^= Zob.piece_keys[black_rook][SQ_F8]
				break
			
			// black queenside castle
			case SQ_C8:
				pos.move_piece(black_rook, SQ_A8, SQ_D8)

				// hash rook
			    pos.hash_key ^= Zob.piece_keys[black_rook][SQ_A8]
                pos.hash_key ^= Zob.piece_keys[black_rook][SQ_D8]
				break
			}
		}

		// hash castling
        pos.hash_key ^= Zob.castle_keys[pos.castling_rights]

		// update castling rights
		pos.castling_rights &= square_to_castling_rights[source_square]
		pos.castling_rights &= square_to_castling_rights[target_square]

		// hash castling
        pos.hash_key ^= Zob.castle_keys[pos.castling_rights]

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

		// hash side
		pos.hash_key ^= Zob.side_key

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
			return pos.make_move(move, all_moves)

		} else {
			return false
		}
	}
}


// copy board
func (pos *Position) copy_board() {
	pos.store_info = State{
		hash_key_copy:         pos.hash_key,
		bitboards_copy:        pos.bitboards,
		occupied_copy:         pos.occupied,
		board_copy:            pos.board, 


		side_to_move_copy:     pos.side_to_move,
		castling_rights_copy:  pos.castling_rights,
		enpassant_square_copy: pos.enpassant_square,
	}
}

// take back
func (pos *Position) take_back() {
	*pos = Position{
		hash_key:         pos.store_info.hash_key_copy,
		bitboards:        pos.store_info.bitboards_copy,
		occupied:         pos.store_info.occupied_copy,
		board:            pos.store_info.board_copy,

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

	// reset board
	for i := range pos.board { pos.board[i] = no_piece }

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
				pos.place_piece(piece, square)
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

	// initialize position hash key
    pos.hash_key = Zob.generate_hash_key(*pos)
}