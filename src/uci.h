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

#include <sstream>
#include <string>
#include <thread>
#include "chess.h"
#include "search.h"

// class for UCI Protocol
class UCIInterface {
public:
    Search search;

    std::istringstream iss;
    std::thread threads;

public:
    void beginThread(int depth);
    void stopThread();
    bool isSearching();

public:
    void UCILoop();
    void bootEngine();
    void parseGoCommand();
    Move parseMove(std::string move);
};

