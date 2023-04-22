// https://github.com/algerbrex/blunder/blob/main/engine/timemanager.go

#include "timemanager.h"

void TimeManager::Start() {
    // Reset the flag time's up flag to false for a new search
    Stop = false;

    // If we're given infinite time, we're done calculating the time for the
    // current move.
    if (TimeLeft == InfiniteTime) {
        return;
    }

    // If we're given a hard time limit, we're also done calculating, since we've
    // been told already how much time should be spent on the current search.
    if (hardTimeForMove != NoValue) {
        return;
    }

    // Calculate the time we can allocate for the search about to start.
    int timeForMove;

    if (MovesToGo != NoValue) {
        // If we have a certian amount of moves to go before the time we have left
        // is reset, use that value to divide the time currently left.
        timeForMove = TimeLeft / MovesToGo;
    } else {
        // Otherwise get 2.5% of the current time left and use that.
        timeForMove = TimeLeft / 40;
    }

    // Give an bonus from the increment
    timeForMove += Increment / 2;

    // If the increment bonus puts us outside of the actual time we
    // have left, use the time we have left minus 500ms.
    if (timeForMove >= TimeLeft) {
        timeForMove = TimeLeft - 500;
    }

    // If taking away 500ms puts us below zero, use 100ms
    // to just get a move to return.
    if (timeForMove <= 0) {
        timeForMove = 100;
    }

    // Calculate the time from now when we need to stop searching, based on the
    // time are allowed to spend on the current search.
    startTime = std::chrono::system_clock::now();
    stopTime = startTime + std::chrono::milliseconds(timeForMove);
    softTimeForMove = timeForMove;
    hardTimeForMove = NoValue;
    Stop = false;
}

void TimeManager::setHardTimeForMove(int newTime) {
    hardTimeForMove = newTime;
    stopTime = std::chrono::system_clock::now() + std::chrono::milliseconds(newTime);
}

void TimeManager::setSoftTimeForMove(int newTime) {
    // To avoid losing on time, we do enforce a rule that
    // any update to the soft time limit must not exceeded
    // more than 1/8th of the total time left for our side.
    if (newTime > TimeLeft / 8) {
        newTime = TimeLeft / 8;
    }

    // If the hard time limit for this move has already been set, only update the
    // time limit if the hard time limit has not been set, or it is still greater
    // than or equal to the new soft time limit.
    if (newTime != NoValue && (hardTimeForMove == NoValue || hardTimeForMove >= newTime)) {
        softTimeForMove = newTime;
        stopTime = std::chrono::system_clock::now() + std::chrono::milliseconds(newTime);
    }
}

void TimeManager::Check() {
    // If we have infinite time, tm.Stop is set to false unless we've already
    // been told to stop.
    if (!Stop && TimeLeft == InfiniteTime) {
        Stop = false;
        return;
    }

    // Otherwise figure out if our allocated time for this move is up.
    if (std::chrono::system_clock::now() >= stopTime) {
        Stop = true;
    }
}
