package main

import (
	"fmt"
	"time"
)

const (
	// FEN dedug positions (perft testing)
	empty_board = "8/8/8/8/8/8/8/8 b - - "
	start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
	tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
	killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
	cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
	repetitions = "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

	// more positions from https://www.chessprogramming.org/Perft_Results
	p3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "
	p4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
	p5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  " 
	p6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "

	// mate in 6 queen sacrifice position
	p7 = "2kr2nr/1pp2ppp/3b4/1P3q2/2Pp1B2/5Q1P/RP3PP1/R5K1 w - - 5 18"
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