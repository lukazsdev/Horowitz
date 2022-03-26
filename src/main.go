package main

import "fmt"

func main() {
	initialize_leaper_attacks()
	for i := 0; i < 64; i++ {
		print_bitboard(king_attacks[i])
		fmt.Print("\n\n")
	}
}
