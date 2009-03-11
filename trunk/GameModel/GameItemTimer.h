#ifndef __GAMEITEMTIMER_H__
#define __GAMEITEMTIMER_H__

#include <string>
#include <map>

#include "GameItem.h"

/**
 * Represents a timer for a game item, that is, it represents
 * some tracked length of time before the effects of a specific
 * item expire.
 */
class GameItemTimer {

private:
	GameItem* assocGameItem;	// The game item associated with this timer
	double timeLengthInSecs;	// Total length of the timer in seconds
	double timeElapsedInSecs;	// Amount of time elapsed on the timer so far

public:
	static const float ZERO_TIME_TIMER_IN_SECS;

	GameItemTimer(GameItem* gameItem);
	~GameItemTimer();

	void Tick(double seconds);

	/**
	 * Returns whether or not this timer has expired.
	 * Returns: true on expiration, false otherwise.
	 */
	bool HasExpired() const {
		return (this->timeElapsedInSecs >= this->timeLengthInSecs);
	}

	/**
	 * For obtaining the type of timer of this.
	 */
	std::string GetTimerItemName() const {
		return this->assocGameItem->GetName();
	}

	GameItem::ItemType GetTimerDisposition() const {
		return this->assocGameItem->GetItemType();
	}

	/**
	 * Stops the timer and the item effect associated with it.
	 */
	void StopTimer() {
		this->assocGameItem->Deactivate();
		this->timeElapsedInSecs = this->timeLengthInSecs;
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