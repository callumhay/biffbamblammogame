/**
 * GameItemTimer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameItemTimer.h"

const float GameItemTimer::ZERO_TIME_TIMER_IN_SECS = 0.0f;

GameItemTimer::GameItemTimer(GameItem* gameItem) : assocGameItem(gameItem), timeElapsedInSecs(0.0), wasStopped(false) {
	assert(gameItem != NULL);
	this->timeLengthInSecs = gameItem->Activate();
	if ((this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
		GameEventManager::Instance()->ActionItemTimerStarted(*this);
	}
}

GameItemTimer::~GameItemTimer() {
	// Make sure the timer gets a stop event called when appropriate
	if (!this->wasStopped && (this->timeLengthInSecs - GameItemTimer::ZERO_TIME_TIMER_IN_SECS) > EPSILON) {
		GameEventManager::Instance()->ActionItemTimerStopped(*this);
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
		this->StopTimer();
	}
}