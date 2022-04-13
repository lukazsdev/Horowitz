package main

import (
	"fmt"
	"time"
)

type Perft struct {
	nodes      uint64
	captures   uint64
	promotions uint64
	enpass     uint64
	castles    uint64
}

// perft driver
func (perft *Perft) driver(pos Position, depth int) {
	if depth == 0 {
		perft.nodes++
		return
	}

	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {
		// preserve board state
		pos.copy_board()

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], all_moves) {
			continue
		} 
		
		// increment perft flags
		perft.update_flags(moves.list[i])
	     
		// call perft recursively
		perft.driver(pos, depth - 1)

		// return to prev board state
		pos.take_back()
	}
}

func (perft *Perft) test(pos Position, depth int) {
	// start timer
	start_timer := time.Now()

	// reset perft info
	perft.nodes      = 0
	perft.captures   = 0
	perft.promotions = 0
	perft.enpass     = 0
	perft.castles    = 0

	// generate peudo legal moves
	moves := pos.generate_moves()

	for i := 0; i < moves.count; i++ {
		// preserve board state
		pos.copy_board()

		// skip if move is ilegal
		if !pos.make_move(moves.list[i], all_moves) {
			continue
		} 

		// increment perft flags
		perft.update_flags(moves.list[i])
	     
		// call perft recursively
		perft.driver(pos, depth - 1)

		// return to prev board state
		pos.take_back()
	}

	fmt.Print("     Depth: ", depth)
	fmt.Print("\n     Nodes: ", perft.nodes)
	fmt.Print("\n     Time: ", time.Since(start_timer))
	fmt.Print("\n\n     Captures:   ", perft.captures)
	fmt.Print("\n     Promotions: ", perft.promotions)
	fmt.Print("\n     Enpassants: ", perft.enpass)
	fmt.Print("\n     Castles:    ", perft.castles)
	fmt.Print("\n")
}

func (perft *Perft) update_flags(move Move) {
	if move.get_move_capture() > 0 {
		perft.captures++
	}
	if move.get_move_promoted() > 0 {
		perft.promotions++
	}
	if move.get_move_enpassant() > 0 {
		perft.enpass++
	}
	if move.get_move_castling() > 0 {
		perft.castles++
	}
}