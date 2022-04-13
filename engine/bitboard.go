package main

import (
    "math/bits"
)

// typedef "Bitboard" as unsigned 64-bit integer
type Bitboard uint64

// set bit at given square (set to 1)
func (bitboard *Bitboard) set_bit(square int) {
    *bitboard |= SQUARE_BB[square]
}

// return bit at given square
func (bitboard *Bitboard) get_bit(square int) uint64 {
    return uint64(*bitboard & SQUARE_BB[square])
}

// pop bit at given square (set to 0)
func (bitboard *Bitboard) pop_bit(square int) {
    if bitboard.get_bit(square) > 0 {
        *bitboard ^= SQUARE_BB[square]
    }
}

// count bits within bitboard
func (bitboard Bitboard) count_bits() int {
    return bits.OnesCount64(uint64(bitboard))
}

// returns index of least significant bit (bit scan forward)
func (bitboard Bitboard) bsf() int {
    return bits.TrailingZeros64(uint64(bitboard))
}

// returns index of least significant bit and removes that bit from bitboard
func (bitboard *Bitboard) pop_lsb() int {
    lsb := bitboard.bsf()
    *bitboard &= *bitboard - 1;
    return lsb
}

// returns other side (e.g f(white) => black)
func other_side(side uint8) uint8 {
    return 1 ^ side
}


//returns reversed bitboard (rotate 180 degrees)
func reverse(bb Bitboard) Bitboard {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555)
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333)
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f)
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff)

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48)
}


// file masks
var MASK_FILE = [8]Bitboard{
    0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
    0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
}

// rank masks
var MASK_RANK = [8]Bitboard{
    0xff, 0xff00, 0xff0000, 0xff000000,
    0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000,	
}

// diagonal masks
var MASK_DIAGONAL = [15]Bitboard{
    0x80, 0x8040, 0x804020,
    0x80402010, 0x8040201008, 0x804020100804,
    0x80402010080402, 0x8040201008040201, 0x4020100804020100,
    0x2010080402010000, 0x1008040201000000, 0x804020100000000,
    0x402010000000000, 0x201000000000000, 0x100000000000000,
}

// anti diagonal masks
var MASK_ANTI_DIAGONAL = [15]Bitboard{
    0x1, 0x102, 0x10204,
    0x1020408, 0x102040810, 0x10204081020,
    0x1020408102040, 0x102040810204080, 0x204081020408000,
    0x408102040800000, 0x810204080000000, 0x1020408000000000,
    0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
}

//array containing bitboard for each square (1 << square)
var SQUARE_BB = [64]Bitboard{
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x100000000, 0x200000000, 0x400000000, 0x800000000,
    0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
    0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
    0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
    0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
    0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
    0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
    0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
};
