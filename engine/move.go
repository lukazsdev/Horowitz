package main

//import "fmt"

// Move list structure
type Moves struct {
	// move list
	moves [256]Move

	// pointer to current move
	count int
}

// add move to move list
func (move_list *Moves) add_move(move Move) {
	// store move
	move_list.moves[move_list.count] = move

	// increment pointer
	move_list.count++
}

type Move uint32

func encode_move(source, target int, piece, promoted, capture, double, enpassant, castling uint8) Move {
	return Move(uint32(source)          |          
	            uint32(target) << 6     |     
		    uint32(piece) << 12     |     
		    uint32(promoted) << 16  |  
		    uint32(capture) << 20   |   
		    uint32(double) << 21    |    
		    uint32(enpassant) << 22 | 
		    uint32(castling) << 23)
}

// extract source square
func (move Move) get_move_source() int {
	return int(move & 0x3f)
}

// extract target square
func (move Move) get_move_target() int {
	return int((move & 0xfc0) >> 6)
}

// extract piece 
func (move Move) get_move_piece() uint8 {
	return uint8((move & 0xf000) >> 12)
}

// extract promoted piece
func (move Move) get_move_promoted() uint8 {
	return uint8((move & 0xf0000) >> 16)
} 

// extract capture flag
func (move Move) get_move_capture() uint8 {
	return uint8((move & 0x100000) >> 20)
}

// extract double pawn push flag
func (move Move) get_move_double() uint8 {
	return uint8((move & 0x200000)>> 21)
}

// extract enpassant flag
func (move Move) get_move_enpassant() uint8 {
	return uint8((move & 0x400000) >> 22)
}

// extract castling flag
func (move Move) get_move_castling() uint8 {
	return uint8((move & 0x800000) >> 23)
}
