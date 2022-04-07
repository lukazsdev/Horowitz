package main

import (
	"fmt"
	"strings"
	"strconv"
	"bufio"
	"os"
)

// struct for UCI protocol
type UCIInterface struct {
	pos    Position
	search Search
}

func (uci *UCIInterface) UCILoop() {
	reader := bufio.NewReader(os.Stdin)

	uci.print_engine_info()
	initialize_lookup_tables()

	// main loop
	for {
		command, _ := reader.ReadString('\n')
		command = strings.Replace(command, "\r\n", "\n", -1)

		if command == "uci\n" {
			uci.print_engine_info()

		} else if command == "isready\n" {
			fmt.Printf("readyok\n")
		} else if strings.HasPrefix(command, "setoption") {
			// do stuff
		} else if strings.HasPrefix(command, "ucinewgame") {
			uci.parse_position("position startpos");
		} else if strings.HasPrefix(command, "position") {
			uci.parse_position(command)
		} else if strings.HasPrefix(command, "go") {
			uci.parse_go(command) 
		} else if strings.HasPrefix(command, "perft") {
			uci.parse_perft(command)
		} else if strings.HasPrefix(command, "board") {
			print_board(uci.pos)
		} else if command == "quit\n" {
			break
		}
	}
}

func (uci *UCIInterface) print_engine_info() {
	fmt.Print("id name AlphaGo\n")
	fmt.Print("id name OliveriQ\n")
	fmt.Print("uciok\n")
}


// parse UCI "position" command (e.g position startpos e2e4)
func (uci *UCIInterface) parse_position(command string) {
	// skip to next token (after "position")
	command = command[9:len(command)]

	// general purpose pointer in command string
	ptr := command

	// parse UCI "startpos" command
	if strings.Compare(command[0:8], "startpos") == 0 {
		// initialize board to start position
		uci.pos.parse_fen(start_position)

	} else {
		// go to "fen" token
		fen_index := strings.Index(command, "fen")

		// if "fen" is not a substr of command
		if fen_index == -1 {
			uci.pos.parse_fen(start_position)

		} else {
			ptr = command[fen_index+4:len(command)]
			uci.pos.parse_fen(ptr[0:len(ptr)])
		}
	}

	// go to "moves" token
	moves_index := strings.Index(command, "moves")

	// if "moves" is available
	if moves_index != -1 {
		moves := strings.Fields(command[moves_index+6:len(command)])
		// loop over all moves
		for i := 0; i < len(moves); i++ {
			move := uci.parse_move(moves[i])

			if move == 0 {
				break
			}

			uci.pos.make_move(move, all_moves)
		}
	}
}


// parse move input from GUI 
func (uci *UCIInterface) parse_move(move_string string) Move {
	moves := uci.pos.generate_moves()

	// parse from and to squares
	source_square := (int(move_string[1])-49)*8 + (int(move_string[0])-97)
	target_square := (int(move_string[3])-49)*8 + (int(move_string[2])-97)

	for i := 0; i < moves.count; i++ {
		// initialize move
		move := moves.list[i]

		// check if move is available within move list
		if source_square == move.get_move_source() && target_square == move.get_move_target() {
			// initialize promoted piece
			promoted_piece := move.get_move_promoted()

			// check if promoted piece is available
			if promoted_piece > 0 {
				// loop over all possible promotions
				for piece := white_knight; piece <= white_queen; piece++ {
					if (promoted_piece==piece || promoted_piece==piece+6) && move_string[4]==promoted_pieces[piece] {
						// return legal move
						return move
					}
				}

				// continue loop on possibly wrong promotions
				continue
			}

			// return legal move
			return move
		}
	} 

	// return ilegal move
	return 0
}

// parse UCI "go" command
func (uci *UCIInterface) parse_go(command string) {
	command = strings.TrimPrefix(command, "go")
	command = strings.TrimPrefix(command, " ")
	fields := strings.Fields(command)

	depth := 4

	for index, field := range fields {
		if field == "depth" {
			depth, _ = strconv.Atoi(fields[index+1])
		}
	}
	

	uci.search.position(uci.pos, depth)
}

func (uci *UCIInterface) parse_perft(command string) {
	command = strings.TrimPrefix(command, "perft")
	command = strings.TrimPrefix(command, " ")
	fields := strings.Fields(command)

	depth := 1

	for index, field := range fields {
		if field == "depth" {
			depth, _ = strconv.Atoi(fields[index+1])
		}
	}
	

	uci.perft_test(depth)
}

func (uci *UCIInterface) perft_test(depth int) {
	var perft Perft 
	perft.test(uci.pos, depth)
}