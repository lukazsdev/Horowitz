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


#include "see.h"

// Peform a static exchange evaluation on target square of the move given,
// and return a score of the move from the perspective of the side to move
int See(Position& pos, Move move) {
    Square toSq       = move.target();
    Square frSq       = move.source();
    PieceType target  = piece_type(pos.board[toSq]);
    PieceType attacker = piece_type(pos.board[frSq]);

    int gain[32];
    int depth = 0;
    Color sideToMove = ~pos.sideToMove;

    memset(gain, 0, sizeof(gain));
    Bitboard seenBB = 0ULL;
    Bitboard occupiedBB = pos.allPieces<White>() | pos.allPieces<Black>();
    Bitboard attackerBB = SQUARE_BB[frSq];

    Bitboard attadef = allAttackers(pos, toSq, occupiedBB);
    Bitboard maxXray = occupiedBB & ~(pos.Knights<White>() | pos.Kings<White>() |
                        pos.Knights<Black>() | pos.Kings<Black>());

    gain[depth] = PieceValue[target];
    for (bool ok = true; ok; ok = attackerBB != 0) {
        depth++;
        gain[depth] = PieceValue[attacker] - gain[depth - 1];
        if (std::max(-gain[depth - 1], gain[depth]) < 0) {
            break;
        }
        attadef    &= ~attackerBB;
        occupiedBB &= ~attackerBB;
        seenBB     |= attackerBB;
        if ((attackerBB & maxXray) != 0) {
            attadef |= considerXRays(pos, toSq, occupiedBB) & ~seenBB;
        }
        attackerBB = minAttacker(pos, attadef, sideToMove, attacker);
        sideToMove = ~sideToMove;
    }

    for (depth--; depth > 0; depth--) {
        gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);
    }

    return gain[0];
}

Bitboard minAttacker(Position& pos, Bitboard attadef, Color color, PieceType& attacker) {
    for (attacker = Pawn; attacker <= King; attacker = PieceType(attacker + 1)) {
        Bitboard subset = attadef & pos.PiecesBB[makePiece(color, attacker)];
        if (subset != 0) return (subset & -subset);
    }
    return 0ULL;
}

Bitboard considerXRays(Position &pos, Square sq, Bitboard occupiedBB) {
    Bitboard attackingBishops = pos.Bishops<White>() | pos.Bishops<Black>();
    Bitboard attackingRooks   = pos.Rooks<White>() | pos.Rooks<Black>();
    Bitboard attackingQueens  = pos.Queens<White>() | pos.Queens<Black>();

    Bitboard interCardinalRays = pos.GetBishopAttacks(sq, occupiedBB);
    Bitboard cardinalRaysRays  = pos.GetRookAttacks(sq, occupiedBB);

    Bitboard attackers = interCardinalRays & (attackingBishops | attackingQueens);
    attackers |= cardinalRaysRays & (attackingRooks | attackingQueens);
	return attackers;
}

Bitboard allAttackers(Position& pos, Square sq, Bitboard occupiedBB) {
    return attackersForSide(pos, White, sq, occupiedBB) | 
                attackersForSide(pos, Black, sq, occupiedBB);
}

Bitboard attackersForSide(Position& pos, Color attackerColor, Square sq, Bitboard occupiedBB) {
    Bitboard attackingBishops = pos.PiecesBB[makePiece(attackerColor, Bishop)];
    Bitboard attackingRooks    = pos.PiecesBB[makePiece(attackerColor, Rook)];
    Bitboard attackingQueens  = pos.PiecesBB[makePiece(attackerColor, Queen)];
    Bitboard attackingKnights = pos.PiecesBB[makePiece(attackerColor, Knight)];
    Bitboard attackingKing    = pos.PiecesBB[makePiece(attackerColor, King)];
    Bitboard attackingPawns   = pos.PiecesBB[makePiece(attackerColor, Pawn)];

    Bitboard interCardinalRays = pos.GetBishopAttacks(sq, occupiedBB);
    Bitboard cardinalRaysRays  = pos.GetRookAttacks(sq, occupiedBB);

    Bitboard attackers = interCardinalRays & (attackingBishops | attackingQueens);
    attackers |= cardinalRaysRays & (attackingRooks | attackingQueens);
	attackers |= pos.GetKnightAttacks(sq) & attackingKnights;
	attackers |= pos.GetKingAttacks(sq) & attackingKing;
	attackers |= pos.GetPawnAttacks(~attackerColor, sq) & attackingPawns;
	return attackers;
} 
