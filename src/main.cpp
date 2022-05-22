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

#include "uci.h"

int main() {
    UCIInterface uci;
    uci.bootEngine();
    //uci.UCILoop();

    uci.search.pos.parseFEN("1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - - 0 1");
    uci.search.pos.print();
    std::cout << See(uci.search.pos, Move(SQ_E1, SQ_E5, Rook, 0)) << std::endl;

    uci.search.pos.parseFEN("1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - - 0 1");
    uci.search.pos.print();
    std::cout << See(uci.search.pos, Move(SQ_D3, SQ_E5, Knight, 0)) << std::endl;

    uci.search.pos.parseFEN("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
    uci.search.pos.print();
    std::cout << See(uci.search.pos, Move(SQ_H7, SQ_E4, Bishop, 0)) << std::endl;
}

