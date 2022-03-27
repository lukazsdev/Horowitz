package main

//import "fmt"

// FEN dedug positions
const empty_board string = "8/8/8/8/8/8/8/8 b - - "
const start_position string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
const tricky_position string = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
const killer_position string = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
const cmk_position string = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
const repetitions string = "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

// parse FEN string
func parse_fen(fen string, ptr int) {

	for i := range BITBOARDS {
		BITBOARDS[i] = 0
	}
	for i := range OCCUPANCIES {
		OCCUPANCIES[i] = 0
	}

	SIDE = 0
	ENPASSANT = NO_SQ
	CASTLE = 0

	for rank := 7; rank >= 0; rank-- {
		for file := 0; file < 8; file++ {
			square := rank*8 + file

			if (fen[ptr]>=97 && fen[ptr]<=122)||(fen[ptr]>=65 && fen[ptr]<=90) {
				piece := char_pieces[fen[ptr]]
				BITBOARDS[piece] = set_bit(BITBOARDS[piece], square)
				ptr = ptr + 1
			}

			if fen[ptr] >= '0' && fen[ptr] <= '9' {
				offset := int(fen[ptr]) - 48
				
				var piece int = -1

				for bb_piece := P; bb_piece <= k; bb_piece++ {
					if get_bit(BITBOARDS[bb_piece], square) > 0 {
						piece = bb_piece
					}
				}

				if piece == -1 {
					file = file - 1
				}
				

				file += offset
				ptr = ptr + 1
			}

			if fen[ptr] == '/' {
				ptr = ptr + 1
			}
		}
	}

	ptr = ptr + 1
	if fen[ptr] == 'w' {
		SIDE = WHITE
	} else {
		SIDE = BLACK
	}

	ptr = ptr + 2

	for fen[ptr] != ' ' {
		switch (fen[ptr]) {
		case 'K':
			CASTLE = CASTLE | WK
		case 'Q':
			CASTLE = CASTLE | WQ
		case 'k':
			CASTLE = CASTLE | BK
		case 'q':
			CASTLE = CASTLE | BQ
		}
		ptr = ptr + 1
	}

	ptr = ptr + 1

	if fen[ptr] != '-' {
		file := int(fen[ptr]) - 97
		rank := int(fen[ptr+1]) - 49
		ENPASSANT = rank*8 + file
	} else {
		ENPASSANT = NO_SQ
	}
}