package main

import "fmt"

func main() {
	initialize_lookup_tables()

	fmt.Print("Go chess engine\n\n")

	var occ uint64 = SQUARE_BB[SQ_C2] | SQUARE_BB[SQ_B7] | SQUARE_BB[SQ_D4] 
	print_bitboard(get_bishop_attacks(SQ_E4, occ))
	print_bitboard(get_rook_attacks(SQ_E4, occ))
}