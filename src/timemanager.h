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
#pragma once

#include <chrono>
#include "chess.h"

static constexpr int NoValue = 0;
static constexpr int InfiniteTime = -1;

// time management class
class TimeManager {
public:
    int TimeLeft;
    int Increment;
    int MovesToGo;
    bool Stop;

    std::chrono::time_point<std::chrono::system_clock> stopTime;
    std::chrono::time_point<std::chrono::system_clock> startTime;
    int hardTimeForMove;
    int softTimeForMove;

public:
    void Start();
    void setHardTimeForMove(int newTime);
    void setSoftTimeForMove(int newTime);
    void Check();
};