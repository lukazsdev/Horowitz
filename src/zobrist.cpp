/*
  Horowitz, a UCI compatible chess engine. 
  Copyright (C) 2022 by OliveriQ.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "zobrist.h"

// zobrist hashing instance
Zobrist zobrist;

// generate almost unique hash for given position
void Zobrist::initRandomKeys() {
    // update pseudo random number state
    randomState = 1804289383;

    // loop over pieces
    for (int piece = 0; piece < 12; piece++) {
        for (int square = 0; square < 64; square++) {
            pieceKeys[piece][square] = random64();
        }
    }

    // loop over board squares
    for (int square = 0; square < 64; square++) {
        // initialize random castle keys
        enpassantKeys[square] = random64();
    }

    // loop over castle rights
    for (int index = 0; index < 16; index++) {
        castleKeys[index] = random64();
    }

    // initialize random side key
    sideKey = random64();
}

uint64_t Zobrist::random64() {
    // define 4 random numbers
    uint64_t n1, n2, n3, n4;
    
    // init random numbers slicing 16 bits from MS1B side
    n1 = (uint64_t)(random32()) & 0xFFFF;
    n2 = (uint64_t)(random32()) & 0xFFFF;
    n3 = (uint64_t)(random32()) & 0xFFFF;
    n4 = (uint64_t)(random32()) & 0xFFFF;
    
    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
} 

uint32_t Zobrist::random32() {
    // get current state
    uint32_t number = randomState;
    
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    // update random number state
    randomState = number;
    
    // return random number
    return number;
} 