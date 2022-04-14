package main

//import "fmt"

const (
    // game phase scores
    opening_phase_score = 6192
    endgame_phase_score = 518

    // game phases
    opening, endgame, middlegame uint8 = 0, 1, 2
)

var material_score = [2][6]int {
    // opening material scores
    { 82, 337, 365, 477, 1025, 12000 }, 

    // endgame material scores
    {  94, 281, 297, 512,  936, 12000 },
}

type Evaluation struct {
    // opening and endgame scores
    OPScores [2]int
    EGScores [2]int

    // total score of position
    score int 
}

// evaluate given position
func evaluate(pos Position) int {
    // evaluation instance
    var eval Evaluation

    // get game phase score
    game_phase_score := get_game_phase_score(pos)	

    // get current game phase
    game_phase := get_game_phase(game_phase_score)

    // loop over piece bitboards
    for bb_piece := white_pawn; bb_piece <= black_king; bb_piece++ {
        // initialize bitboard to current piece
        bitboard := pos.bitboards[bb_piece]

        // loop over pieces within bitboard
        for bitboard > 0 {
            // initialize current piece
            piece := bb_piece

            // initialize current piece color
            color := piece / 6

            // initialize square and pop LSB
            square := bitboard.pop_lsb()

            eval.OPScores[color] += material_score[opening][piece%6]
            eval.EGScores[color] += material_score[endgame][piece%6]

            // add positional piece scores
            switch bb_piece % 6 {

            // evaluate white pawns
            case Pawn:
                evaluate_pawn(pos, color, square, &eval)

            // evaluate white knights
            case Knight:
                evaluate_knight(pos, color, square, &eval)
            
            // evaluate white bishops
            case Bishop:
                evaluate_bishop(pos, color, square, &eval)

            // evaluate white rooks
            case Rook:
                evaluate_rook(pos, color, square, &eval)

            // evaluate white queens
            case Queen:
                evaluate_queen(pos, color, square, &eval)

            // evaluate white king
            case King:
                evaluate_king(pos, color, square, &eval)
            }
        }
    }

    op_score := eval.OPScores[white] - eval.OPScores[black]
    eg_score := eval.EGScores[white] - eval.EGScores[black]

    // score for middlegame phase
    if game_phase == middlegame {
        eval.score = (op_score * game_phase_score + eg_score * 
                (opening_phase_score - game_phase_score)) / opening_phase_score
    
    // score for opening phase
    } else if game_phase == opening {
        eval.score = op_score
    
    // score for endgame phase
    } else if game_phase == endgame {
        eval.score = eg_score
    }

    return perspective(eval.score, pos.side_to_move)
}

// evaluate score of a pawn
func evaluate_pawn(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][Pawn][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][Pawn][FLIP_SQ[color][square]]
}

// evaluate score of a knight
func evaluate_knight(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][Knight][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][Knight][FLIP_SQ[color][square]]
}

// evaluate score of a bishop
func evaluate_bishop(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][Bishop][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][Bishop][FLIP_SQ[color][square]]
}

// evaluate score of a rook
func evaluate_rook(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][Rook][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][Rook][FLIP_SQ[color][square]]
}

// evaluate score of a queen
func evaluate_queen(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][Queen][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][Queen][FLIP_SQ[color][square]]
}

// evaluate score of a king
func evaluate_king(pos Position, color uint8, square int, eval *Evaluation) {
    eval.OPScores[color] += PSQT[opening][King][FLIP_SQ[color][square]]
    eval.EGScores[color] += PSQT[endgame][King][FLIP_SQ[color][square]]
}


// get game phase score
func get_game_phase_score(pos Position) int {
    // white and black game phase scores
    var white_piece_scores, black_piece_scores int

    // loop over white pieces
    for piece := white_knight; piece <= white_queen; piece++ {
        white_piece_scores += pos.bitboards[piece].count_bits() * material_score[opening][piece%6]
    }

    // loop over black pieces
    for piece := black_knight; piece <= black_queen; piece++ {
        black_piece_scores += pos.bitboards[piece].count_bits() * material_score[opening][piece%6]
    }

    // return game phase score
    return white_piece_scores + black_piece_scores
}

// returns game phase given the game phase score
func get_game_phase(game_phase_score int) uint8 {
    // return opening phase if score > opening phase
    if game_phase_score > opening_phase_score {
        return opening

    // return endgame phase if score > endgame phase
    } else if game_phase_score < endgame_phase_score {
        return endgame
    }

    // else, return middlegame phase
    return middlegame
}