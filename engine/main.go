package main

func main() {
	initialize_lookup_tables()

	// UCI testing
	var uci UCIInterface
	uci.UCILoop()
}
