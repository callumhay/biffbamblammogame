/**
 * GameItemTimer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameItemTimer.h"

const float GameItemTimer::ZERO_TIME_TIMER_IN_SECS = 0.0f;

GameItemTimer::GameItemTimer(GameItem* gameItem) : 
assocGameItem(gameItem), timeElapsedInSecs(0.0) , wasStopped(false), deactivateItemOnStop(true) {
	assert(gameItem != NULL);
	this->timeLengthInSecs = gameItem->Activate();
	if ((this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
		GameEventManager::Instance()->ActionItemTimerStarted(*this);
	}

    // If there are balls associated with the item then add it to the set of associated balls
    this->assocGameBalls = gameItem->GetBallsAffected();
}

GameItemTimer::~GameItemTimer() {
	// Make sure the timer gets a stop event called when appropriate
	//if (!this->wasStopped && (this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
	//	GameEventManager::Instance()->ActionItemTimerStopped(*this);
	//}
	if (!this->wasStopped) {
		this->StopTimer(false);
	}

	if (this->assocGameItem != NULL) {
		delete this->assocGameItem;
		this->assocGameItem = NULL;
	}
}

/**
 * This will update the timer, i.e., make it tick away until all
 * its time has elapsed.
 */
void GameItemTimer::Tick(double seconds) {
	// Do nothing if time has already expired... 
	if (this->HasExpired()) {
		return;
	}

	this->timeElapsedInSecs += seconds;

	// This should only happen once - obviously when a timer expires
	// so must its associated game item's effect!
	if (this->HasExpired()) {
		this->StopTimer(true);
	}
}