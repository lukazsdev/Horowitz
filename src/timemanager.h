#include <chrono>
#include "chess.h"

/*
const (
	NoValue      int64 = 0
	InfiniteTime int64 = -1
)

// A struct which holds data for a timer for Blunder's time mangement.
type TimeManager struct {
	TimeLeft  int64
	Increment int64
	MovesToGo int64
	Stop      bool

	stopTime        time.Time
	startTime       time.Time
	hardTimeForMove int64
	SoftTimeForMove int64
}
*/

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