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


#include "chess.h"

#pragma once

int See(Position& pos, Move move);
Bitboard minAttacker(Position& pos, Bitboard attadef, Color color, PieceType& attacker);
Bitboard considerXRays(Position &pos, Square sq, Bitboard occupiedBB);
Bitboard allAttackers(Position& pos, Square sq, Bitboard occupiedBB);
Bitboard attackersForSide(Position& pos, Color attackerColor, Square sq, Bitboard occupiedBB); 