#include "chess.h"

#pragma once

int See                   (Position& pos, Move move);
Bitboard minAttacker      (Position& pos, Bitboard attadef, Color color, PieceType& attacker);
Bitboard considerXRays    (Position &pos, Square sq, Bitboard occupiedBB);
Bitboard allAttackers     (Position& pos, Square sq, Bitboard occupiedBB);
Bitboard attackersForSide (Position& pos, Color attackerColor, Square sq, Bitboard occupiedBB); 