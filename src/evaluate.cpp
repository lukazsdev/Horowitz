#include "evaluate.h"

namespace Eval {

// defin pawn hash table
PawnHashTable PT;

// evaluation masks
Bitboard whitePassedMasks[64];
Bitboard blackPassedMasks[64];
  
// mop up evaluation functions
float endgamePhaseWeight(int materialWithoutPawns) {
  float multiplier = 1 / materialEndgameStart;
  return 1 - std::min((float)1, (float)materialWithoutPawns * multiplier);
}

void init() {
  for (Square sq = Square(0); sq < Square(64); sq = Square(sq + 1)) {
    uint8_t file = file_of(sq);
    uint8_t rank = rank_of(sq);
    Bitboard fileMask = (file == FILE_A) ? MASK_FILE[FILE_A] | MASK_FILE[FILE_B] :
                        (file == FILE_H) ? MASK_FILE[FILE_H] | MASK_FILE[FILE_G] :
                        MASK_FILE[file - 1] | MASK_FILE[file] | MASK_FILE[file + 1];
    

    Bitboard whitePassedMask = 0ULL;
    for (int currRank = (int)rank + 1; currRank <= (int)RANK_8; currRank++) {
      whitePassedMask |= fileMask & MASK_RANK[currRank];
    }

    Bitboard blackPassedMask = 0ULL;
    for (int currRank = (int)rank - 1; currRank >= (int)RANK_1; currRank--) {
      blackPassedMask |= fileMask & MASK_RANK[currRank];
    }

    whitePassedMasks[sq] = whitePassedMask;
    blackPassedMasks[sq] = blackPassedMask;
  }
}

} // end of namespace eval