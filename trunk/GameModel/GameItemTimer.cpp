/**
 * GameItemTimer.cpp
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