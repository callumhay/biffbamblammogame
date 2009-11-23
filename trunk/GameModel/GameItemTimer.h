#ifndef __GAMEITEMTIMER_H__
#define __GAMEITEMTIMER_H__

#include <string>
#include <map>

#include "GameItem.h"
#include "GameEventManager.h"

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
	bool wasStopped;

public:
	static const float ZERO_TIME_TIMER_IN_SECS;

	GameItemTimer(GameItem* gameItem);
	~GameItemTimer();

	void Tick(double seconds);

	/**
	 * Returns whether or not this timer has expired.
	 * Returns: true on expiration, false otherwise.
	 */
	inline bool HasExpired() const {
		return (this->timeElapsedInSecs >= this->timeLengthInSecs);
	}

	inline std::string GetTimerItemName() const {
		return this->assocGameItem->GetName();
	}

	inline GameItem::ItemType GetTimerItemType() const {
		return this->assocGameItem->GetItemType();
	}

	inline GameItem::ItemDisposition GetTimerDisposition() const {
		return this->assocGameItem->GetItemDisposition();
	}

	/**
	 * Stops the timer and the item effect associated with it.
	 */
	inline void StopTimer() {
		// In the case of items that have no time associated with their effect, we
		// don't affect the item or set off any events
		if ((this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
			this->assocGameItem->Deactivate();
			GameEventManager::Instance()->ActionItemTimerStopped(*this);
		}

		this->timeElapsedInSecs = this->timeLengthInSecs;
		this->wasStopped = true;
	}

	/**
	 * Returns a the decimal percentage of elapsed time on the timer.
	 * Returns: a number in [0,1] reflecting the amount of elapsed time.
	 */
	inline double GetPercentTimeElapsed() const {
		if (this->timeLengthInSecs == 0) { 
			return 1.0f; 
		}

		double decPercent = this->timeElapsedInSecs / this->timeLengthInSecs;
		assert(decPercent >= 0 && decPercent <= 1.0f);
		return decPercent;
	}

	friend std::ostream& operator <<(std::ostream& os, const GameItemTimer& itemTimer);
};


inline std::ostream& operator <<(std::ostream& os, const GameItemTimer& itemTimer) {
	return os << "Item Timer (Item: " << *itemTimer.assocGameItem << ", Duration: " << itemTimer.timeLengthInSecs << ")";
};

#endif