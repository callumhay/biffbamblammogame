#ifndef __GAMEITEMTIMER_H__
#define __GAMEITEMTIMER_H__

#include <string>

#include "GameItem.h"

/**
 * Represents a timer for a game item, that is, it represents
 * some tracked length of time before the effects of a specific
 * item expire.
 */
class GameItemTimer {

public:
	// Timer type enumerator:
	// - NoTimer : This is an immediate timer with ZERO totalLengthInSecs, this is just to ensure that
	//             the timer architecture is universal and there are no exceptions for cases where a timer is not used.
	// - SlowBallTimer : A timer related to an item that makes the ball go slower
	// - FastBallTimer : A timer related to an item that makes the ball go faster
	enum TimerType { NoTimer, SlowBallTimer, FastBallTimer };

private:
	GameItem* assocGameItem;	// The game item associated with this timer
	double timeLengthInSecs;	// Total length of the timer in seconds
	double timeElapsedInSecs;	// Amount of time elapsed on the timer so far
	TimerType type;						// The type of timer that this represents

public:
	GameItemTimer(GameItem* gameItem, const TimerType type, const double totalLengthInSecs);
	~GameItemTimer();

	void Tick(double seconds);
	
	/**
	 * Returns whether or not this timer has expired.
	 * Returns: true on expiration, false otherwise.
	 */
	bool HasExpired() const {
		return (type == NoTimer) || (this->timeElapsedInSecs >= this->timeLengthInSecs);
	}

	/**
	 * For obtaining the type of timer of this.
	 */
	TimerType GetTimerType() const {
		return this->type;
	}

	/**
	 * Returns a the decimal percentage of elapsed time on the timer.
	 * Returns: a number in [0,1] reflecting the amount of elapsed time.
	 */
	float GetPercentTimeElapsed() const {
		if (this->timeLengthInSecs == 0) { 
			return 1.0f; 
		}

		float decPercent = this->timeElapsedInSecs / this->timeLengthInSecs;
		assert(decPercent >= 0 && decPercent <= 1.0f);
		return decPercent;
	}

};
#endif