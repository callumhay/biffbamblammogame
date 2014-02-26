/**
 * GameItemTimer.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
	friend std::ostream& operator <<(std::ostream& os, const GameItemTimer& itemTimer);
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

	inline GameItem::ItemType GetTimerItemType() const {
		return this->assocGameItem->GetItemType();
	}

	inline const GameItem::ItemDisposition& GetTimerDisposition() const {
		return this->assocGameItem->GetItemDisposition();
	}

	/**
	 * Allows control over whether the "deactivate" method will be called
	 * on the game item in this when the timer stops.
	 */
	inline void SetDeactivateItemOnStop(bool deactivate) {
		this->deactivateItemOnStop = deactivate;
	}

	/**
	 * Stops the timer and the item effect associated with it.
	 */
	inline void StopTimer(bool didExpire) {
		// In the case of items that have no time associated with their effect, we
		// don't affect the item or set off any events
		if ((this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
			if (this->deactivateItemOnStop) {
				this->assocGameItem->Deactivate();
			}
			GameEventManager::Instance()->ActionItemTimerStopped(*this, didExpire);
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

	inline double GetTimeLeft() const {
		double timeLeft = this->timeLengthInSecs - this->timeElapsedInSecs;
		assert(timeLeft >= 0);
		return timeLeft;
	}

    inline const std::set<const GameBall*>& GetAssociatedBalls() const {
        return this->assocGameBalls;
    }
    inline void AddAssociatedBall(const GameBall* ball) {
        this->assocGameBalls.insert(ball);
    }
    inline void RemoveAssociatedBall(const GameBall* ball) {
        this->assocGameBalls.erase(ball);
    }

private:
	GameItem* assocGameItem;	// The game item associated with this timer
	double timeLengthInSecs;	// Total length of the timer in seconds
	double timeElapsedInSecs;	// Amount of time elapsed on the timer so far
	bool wasStopped;

	bool deactivateItemOnStop;

    std::set<const GameBall*> assocGameBalls; // The game balls that this item timer is associated with
                                              // if the timer does not affect a ball then this set will be empty

    DISALLOW_COPY_AND_ASSIGN(GameItemTimer);
};


inline std::ostream& operator <<(std::ostream& os, const GameItemTimer& itemTimer) {
	return os << "Item Timer (Item: " << *itemTimer.assocGameItem << ", Duration: " << itemTimer.timeLengthInSecs << ")";
};

#endif