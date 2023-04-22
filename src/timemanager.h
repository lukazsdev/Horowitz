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