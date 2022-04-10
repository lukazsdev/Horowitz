package main

// Reference: https://github.com/algerbrex/blunder/blob/main/engine/timemanager.go

import (
	"time"
)

const (
	no_value      int64 = 0
	infinite_time int64 = -1
)

// A struct which holds data for a timer for Blunder's time mangement.
type TimeManager struct {
	time_left   int64
	increment   int64
	moves_to_go int64
	stop        bool

	stop_time          time.Time
	start_time         time.Time
	hard_time_for_move int64
	soft_time_for_move int64
}

// Start the timer, setting up the internal state.
func (tm *TimeManager) start() {
	// Reset the flag time's up flag to false for a new search
	tm.stop = false

	// If we're given infinite time, we're done calculating the time for the
	// current move.
	if tm.time_left == infinite_time {
		return
	}

	// If we're given a hard time limit, we're also done calculating, since we've
	// been told already how much time should be spent on the current search.
	if tm.hard_time_for_move != no_value {
		return
	}

	// Calculate the time we can allocate for the search about to start.
	var time_for_move int64

	if tm.moves_to_go != no_value {
		// If we have a certian amount of moves to go before the time we have left
		// is reset, use that value to divide the time currently left.
		time_for_move = tm.time_left / tm.moves_to_go
	} else {
		// Otherwise get 2.5% of the current time left and use that.
		time_for_move = tm.time_left / 40
	}

	// Give an bonus from the increment
	time_for_move += tm.increment / 2

	// If the increment bonus puts us outside of the actual time we
	// have left, use the time we have left minus 500ms.
	if time_for_move >= tm.time_left {
		time_for_move = tm.time_left - 500
	}

	// If taking away 500ms puts us below zero, use 100ms
	// to just get a move to return.
	if time_for_move <= 0 {
		time_for_move = 100
	}

	// Calculate the time from now when we need to stop searching, based on the
	// time are allowed to spend on the current search.
	tm.start_time = time.Now()
	tm.stop_time = tm.start_time.Add(time.Duration(time_for_move) * time.Millisecond)
	tm.soft_time_for_move = time_for_move
	tm.hard_time_for_move = no_value
	tm.stop = false
}

// Set a hard limit for the maximum amount of time the current
// search can use. Normally this value is set automatically to
// allow the search to use however much time it needs, but there
// are cases where we want to enforce a strict time limit.
//
// This method should not be called after TimeManger.Start has been called,
// since both methods act as the intializers of the current search, and one
// would conflict with the other. So either TimeManager.Start or
// TimeManeger.Sethard_time_for_move should be called to intialize the current
// search time logic, but not both.
func (tm *TimeManager) set_hard_time_for_move(newTime int64) {
	tm.hard_time_for_move = newTime
	tm.stop_time = time.Now().Add(time.Duration(newTime) * time.Millisecond)
}

// Set the soft time limit for current search. The soft time limit
// is a reccomendation for how long the search should continue, but
// can be changed by dynamic factors during the search.
func (tm *TimeManager) set_soft_time_for_move(newTime int64) {
	// To avoid losing on time, we do enforce a rule that
	// any update to the soft time limit must not exceeded
	// more than 1/8th of the total time left for our side.
	if newTime > tm.time_left/8 {
		newTime = tm.time_left / 8
	}

	// If the hard time limit for this move has already been set, only update the
	// time limit if the hard time limit has not been set, or it is still greater
	// than or equal to the new soft time limit.
	if newTime != no_value && (tm.hard_time_for_move == no_value || tm.hard_time_for_move >= newTime) {
		tm.soft_time_for_move = newTime
		tm.stop_time = tm.start_time.Add(time.Duration(newTime) * time.Millisecond)
	}
}

// Check if the time we alloted for picking this move has expired.
func (tm *TimeManager) check() {
	// If we have infinite time, tm.stop is set to false unless we've already
	// been told to stop.
	if !tm.stop && tm.time_left == infinite_time {
		tm.stop = false
		return
	} 

	// Otherwise figure out if our allocated time for this move is up.
	if time.Now().After(tm.stop_time) {
		tm.stop = true
	}
}