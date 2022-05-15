#include "evaluate.h"


namespace Eval {

// define isolated pawn masks
Bitboard isolatedPawnMasks[8];

void init() {
	// initialize isolated pawn masks
	isolatedPawnMasks[FILE_A] = MASK_FILE[FILE_B];
	isolatedPawnMasks[FILE_H] = MASK_FILE[FILE_G];
	for (int file = 1; file < 7; file++) {
		isolatedPawnMasks[file] = MASK_FILE[file - 1] | MASK_FILE[file + 1];
	}
}

// mop up evaluation functions
float endgamePhaseWeight(int materialWithoutPawns) {
	float multiplier = 1 / materialEndgameStart;
	return 1 - std::min((float)1, (float)materialWithoutPawns * multiplier);
}

// return whether given pawn is backwards or not
bool isBackwardsPawn(Square sq, Bitboard ourPawnsBB, Color c) {
    if (c == White) {
        uint8_t midRank   = rank_of(sq);
        uint8_t leftRank  = rank_of(bsf((ourPawnsBB & MASK_FILE[file_of(sq)-1])));
        uint8_t rightRank = rank_of(bsf((ourPawnsBB & MASK_FILE[file_of(sq)+1])));
        if (midRank < leftRank && midRank < rightRank) 
            return true;
        
    } else {
        uint8_t midRank = rank_of(sq);
        uint8_t leftRank  = rank_of(bsr((ourPawnsBB & MASK_FILE[file_of(sq)-1])));
        uint8_t rightRank = rank_of(bsr((ourPawnsBB & MASK_FILE[file_of(sq)+1])));
        if (midRank > leftRank && midRank > rightRank) 
            return true;
    } 

    return false;
}

}; // end of evaluation namespace