package main

// structure for representing chess position
type Position struct {
	// piece and occupied bitboards
	bitboards [12]Bitboard
	occupied   [3]Bitboard

	// main info of position
	side_to_move     uint8
	castling_rights  uint8
	enpassant_square int
}

// make move


// parse fen
// parse FEN string
func (pos *Position) parse_fen(fen string, ptr int) {
	// reset bitboards
	for i := range pos.bitboards {
		pos.bitboards[i] = 0
	}
	for i := range pos.occupied {
		pos.occupied[i] = 0
	}

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