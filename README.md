## Overview:
Golpher is a UCI compatible chess engine written in Golang. Available to play against on lichess [here](https://lichess.org/@/AlphaGooo).
## Installation:
Only builds for Windows are available in this repository. To build locally on a different operating system navigate to the engine directory and run `go build`.
## Basic usage:
Alpha Go supports the [UCI Protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html). When running the engine, the output should be:
```
>>> id name Golpher
>>> id name OliveriQ
>>> uciok

```
## Features:
- Move Generation:
    - [Bitboards](https://www.chessprogramming.org/Bitboards)
    - [Hyperbola Quintessence](https://www.chessprogramming.org/Hyperbola_Quintessence)
    - [Integer move encoding](https://www.chessprogramming.org/Encoding_Moves)
- Search:
    - [Negamax search](https://www.chessprogramming.org/Negamax)
    - [Quiescence search](https://www.chessprogramming.org/Quiescence_Search)
    - [Alpha-beta pruning](https://www.chessprogramming.org/Alpha-Beta)
    - [PV/killer/history move ordering](https://www.chessprogramming.org/Move_Ordering)
    - [PVS (Principal Variation Search)](https://www.chessprogramming.org/Principal_Variation_Search)
    - [LMR (Late Move Pruning)](https://www.chessprogramming.org/Late_Move_Reductions)
    - [NMP (Null Move Pruning)](https://www.chessprogramming.org/Null_Move_Pruning)
    - [Aspiration windows](https://www.chessprogramming.org/Aspiration_Windows)
- Evaluation:
    - [Material](https://www.chessprogramming.org/Material)
    - [Piece-square tables](https://www.chessprogramming.org/Piece-Square_Tables)

