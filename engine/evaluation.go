package main

//import "fmt"

const (
    // game phase scores
    opening_phase_score = 6192
    endgame_phase_score = 518

    // game phases
    opening, endgame, middlegame uint8 = 0, 1, 2
)

// positional piece scores [game phase][piece][square]
var positional_score = [2][6][64]int {
    
    // opening positional piece scores // 
    {

        // pawn
        {
        0,   0,   0,   0,   0,   0,  0,   0,
        98, 134,  61,  95,  68, 126, 34, -11,
        -6,   7,  26,  31,  65,  56, 25, -20,
        -14,  13,   6,  21,  23,  12, 17, -23,
        -27,  -2,  -5,  12,  17,   6, 10, -25,
        -26,  -4,  -4, -10,   3,   3, 33, -12,
        -35,  -1, -20, -23, -15,  24, 38, -22,
        0,   0,   0,   0,   0,   0,  0,   0,
        },
        
        // knight
        {
        -167, -89, -34, -49,  61, -97, -15, -107,
        -73, -41,  72,  36,  23,  62,   7,  -17,
        -47,  60,  37,  65,  84, 129,  73,   44,
        -9,  17,  19,  53,  37,  69,  18,   22,
        -13,   4,  16,  13,  28,  19,  21,   -8,
        -23,  -9,  12,  10,  19,  17,  25,  -16,
        -29, -53, -12,  -3,  -1,  18, -14,  -19,
        -105, -21, -58, -33, -17, -28, -19,  -23,
        },
        
        // bishop
        {
        -29,   4, -82, -37, -25, -42,   7,  -8,
        -26,  16, -18, -13,  30,  59,  18, -47,
        -16,  37,  43,  40,  35,  50,  37,  -2,
        -4,   5,  19,  50,  37,  37,   7,  -2,
        -6,  13,  13,  26,  34,  12,  10,   4,
        0,  15,  15,  15,  14,  27,  18,  10,
        4,  15,  16,   0,   7,  21,  33,   1,
        -33,  -3, -14, -21, -13, -12, -39, -21,
        },
        
        // rook
        {
        32,  42,  32,  51, 63,  9,  31,  43,
        27,  32,  58,  62, 80, 67,  26,  44,
        -5,  19,  26,  36, 17, 45,  61,  16,
        -24, -11,   7,  26, 24, 35,  -8, -20,
        -36, -26, -12,  -1,  9, -7,   6, -23,
        -45, -25, -16, -17,  3,  0,  -5, -33,
        -44, -16, -20,  -9, -1, 11,  -6, -71,
        -19, -13,   1,  17, 16,  7, -37, -26,
        },
        
        // queen
        {
        -28,   0,  29,  12,  59,  44,  43,  45,
        -24, -39,  -5,   1, -16,  57,  28,  54,
        -13, -17,   7,   8,  29,  56,  47,  57,
        -27, -27, -16, -16,  -1,  17,  -2,   1,
        -9, -26,  -9, -10,  -2,  -4,   3,  -3,
        -14,   2, -11,  -2,  -5,   2,  14,   5,
        -35,  -8,  11,   2,   8,  15,  -3,   1,
        -1, -18,  -9,  10, -15, -25, -31, -50,
        },
        
        // king
        {
        -65,  23,  16, -15, -56, -34,   2,  13,
        29,  -1, -20,  -7,  -8,  -4, -38, -29,
        -9,  24,   2, -16, -20,   6,  22, -22,
        -17, -20, -12, -27, -30, -25, -14, -36,
        -49,  -1, -27, -39, -46, -44, -33, -51,
        -14, -14, -22, -46, -44, -30, -15, -27,
        1,   7,  -8, -64, -43, -16,   9,   8,
        -15,  36,  12, -54,   8, -28,  24,  14,
        },
    },

    // Endgame positional piece scores //
    {

        //pawn
        {
        0,   0,   0,   0,   0,   0,   0,   0,
        178, 173, 158, 134, 147, 132, 165, 187,
        94, 100,  85,  67,  56,  53,  82,  84,
        32,  24,  13,   5,  -2,   4,  17,  17,
        13,   9,  -3,  -7,  -7,  -8,   3,  -1,
        4,   7,  -6,   1,   0,  -5,  -1,  -8,
        13,   8,   8,  10,  13,   0,   2,  -7,
        0,   0,   0,   0,   0,   0,   0,   0,
        },
        
        // knight
        {
        -58, -38, -13, -28, -31, -27, -63, -99,
        -25,  -8, -25,  -2,  -9, -25, -24, -52,
        -24, -20,  10,   9,  -1,  -9, -19, -41,
        -17,   3,  22,  22,  22,  11,   8, -18,
        -18,  -6,  16,  25,  16,  17,   4, -18,
        -23,  -3,  -1,  15,  10,  -3, -20, -22,
        -42, -20, -10,  -5,  -2, -20, -23, -44,
        -29, -51, -23, -15, -22, -18, -50, -64,
        },
        
        // bishop
        {
        -14, -21, -11,  -8, -7,  -9, -17, -24,
        -8,  -4,   7, -12, -3, -13,  -4, -14,
        2,  -8,   0,  -1, -2,   6,   0,   4,
        -3,   9,  12,   9, 14,  10,   3,   2,
        -6,   3,  13,  19,  7,  10,  -3,  -9,
        -12,  -3,   8,  10, 13,   3,  -7, -15,
        -14, -18,  -7,  -1,  4,  -9, -15, -27,
        -23,  -9, -23,  -5, -9, -16,  -5, -17,
        },
        
        // rook
        {
        13, 10, 18, 15, 12,  12,   8,   5,
        11, 13, 13, 11, -3,   3,   8,   3,
        7,  7,  7,  5,  4,  -3,  -5,  -3,
        4,  3, 13,  1,  2,   1,  -1,   2,
        3,  5,  8,  4, -5,  -6,  -8, -11,
        -4,  0, -5, -1, -7, -12,  -8, -16,
        -6, -6,  0,  2, -9,  -9, -11,  -3,
        -9,  2,  3, -1, -5, -13,   4, -20,
        },
        
        // queen
        {
        -9,  22,  22,  27,  27,  19,  10,  20,
        -17,  20,  32,  41,  58,  25,  30,   0,
        -20,   6,   9,  49,  47,  35,  19,   9,
        3,  22,  24,  45,  57,  40,  57,  36,
        -18,  28,  19,  47,  31,  34,  39,  23,
        -16, -27,  15,   6,   9,  17,  10,   5,
        -22, -23, -30, -16, -16, -23, -36, -32,
        -33, -28, -22, -43,  -5, -32, -20, -41,
        },
        
        // king
        {
        -74, -35, -18, -18, -11,  15,   4, -17,
        -12,  17,  14,  17,  17,  38,  23,  11,
        10,  17,  23,  15,  20,  45,  44,  13,
        -8,  22,  24,  27,  26,  33,  26,   3,
        -18,  -4,  21,  24,  27,  23,   9, -11,
        -19,  -3,  11,  21,  23,  16,   7,  -9,
        -27, -11,   4,  13,  14,   4,  -5, -17,
        -53, -34, -21, -11, -28, -14, -24, -43,
        },
    },
}

var material_score = [2][12]int {
    // opening material scores
    { 82, 337, 365, 477, 1025, 12000, -82, -337, -365, -477, -1025, -12000, }, 

    // endgame material scores
    {  94, 281, 297, 512,  936, 12000, -94, -281, -297, -512,  -936, -12000, },
}

// evaluate given position
func evaluate(pos Position) int {
    // static evaluation scores
    var score, opening_score, endgame_score int

    // general purpose bitboard
    var bitboard Bitboard

    // general purpose piece 
    var piece uint8

    // general purpose square
    var square int

    // get game phase score
    game_phase_score := get_game_phase_score(pos)	

    // get current game phase
    game_phase := get_game_phase(game_phase_score)

    // loop over piece bitboards
    for bb_piece := white_pawn; bb_piece <= black_king; bb_piece++ {
        // initialize bitboard to current piece
        bitboard = pos.bitboards[bb_piece]

        // loop over pieces within bitboard
        for bitboard > 0 {
            // initialize piece
            piece = bb_piece

            // initialize square and pop LSB
            square = bitboard.pop_lsb()

            // add opening and endgame material scores
            opening_score += material_score[opening][piece]
            endgame_score += material_score[endgame][piece]

            // add positional piece scores
            switch piece {


                            //=========================//
                            // white pieces evaluation //
                            //=========================//


            // evaluate white pawns
            case white_pawn:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][Pawn][mirror[square]]
                endgame_score += positional_score[endgame][Pawn][mirror[square]]
                break

            // evaluate white knights
            case white_knight:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][Knight][mirror[square]]
                endgame_score += positional_score[endgame][Knight][mirror[square]]
                break
            
            // evaluate white bishops
            case white_bishop:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][Bishop][mirror[square]]
                endgame_score += positional_score[endgame][Bishop][mirror[square]]
                break

            // evaluate white rooks
            case white_rook:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][Rook][mirror[square]]
                endgame_score += positional_score[endgame][Rook][mirror[square]]
                break

            // evaluate white queens
            case white_queen:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][Queen][mirror[square]]
                endgame_score += positional_score[endgame][Queen][mirror[square]]
                break

            // evaluate white king
            case white_king:
                // add opening and endgame positional scores
                opening_score += positional_score[opening][King][mirror[square]]
                endgame_score += positional_score[endgame][King][mirror[square]]
                break


                            //=========================//
                            // black pieces evaluation //
                            //=========================//

            
            // evaluate black pawns
            case black_pawn:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][Pawn][square]
                endgame_score -= positional_score[endgame][Pawn][square]
                break

            // evaluate black knights
            case black_knight:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][Knight][square]
                endgame_score -= positional_score[endgame][Knight][square]
                break

            // evaluate black bishops
            case black_bishop:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][Bishop][square]
                endgame_score -= positional_score[endgame][Bishop][square]
                break

            // evaluate black rooks
            case black_rook:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][Rook][square]
                endgame_score -= positional_score[endgame][Rook][square]
                break

            // evaluate black queens
            case black_queen:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][Queen][square]
                endgame_score -= positional_score[endgame][Queen][square]
                break

            // evaluate black king
            case black_king:
                // add opening and endgame positional scores
                opening_score -= positional_score[opening][King][square]
                endgame_score -= positional_score[endgame][King][square]
                break

            }
        }
    }

    //====================================================//
    //           Formula for middlegame score:            //
    // (opening_score * game_phase_score + endgame_score   //
    // * (opening_phase_score - game_phase_score)) /    //
    //               opening_phase_score                  //
    //====================================================//

    // score for middlegame phase
    if game_phase == middlegame {
        score = (opening_score * game_phase_score + endgame_score * 
                (opening_phase_score - game_phase_score)) / opening_phase_score
    
    // score for opening phase
    } else if game_phase == opening {
        score = opening_score
    
    // score for endgame phase
    } else if game_phase == endgame {
        score = endgame_score
    }

    return perspective(score, pos.side_to_move)
}


// get game phase score
func get_game_phase_score(pos Position) int {
    // white and black game phase scores
    var white_piece_scores, black_piece_scores int

    // loop over white pieces
    for piece := white_knight; piece <= white_queen; piece++ {
        white_piece_scores += pos.bitboards[piece].count_bits() * material_score[opening][piece]
    }

    // loop over black pieces
    for piece := black_knight; piece <= black_queen; piece++ {
        black_piece_scores += pos.bitboards[piece].count_bits() * -material_score[opening][piece]
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