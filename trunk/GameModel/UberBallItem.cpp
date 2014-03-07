/**
 * UberBallItem.cpp
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

#include "UberBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double UberBallItem::UBER_BALL_TIMER_IN_SECS	= 10.0;
const char* UberBallItem::UBER_BALL_ITEM_NAME       = "UberBall";

UberBallItem::UberBallItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(UBER_BALL_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

UberBallItem::~UberBallItem() {
}

double UberBallItem::Activate() {
	this->isActive = true;

	// Kill other uber ball timers
	// TODO: cancel out weak ball timers...

	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::UberBallItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the uberball timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
	    GameItemTimer* currTimer = removeTimers[i];
	    activeTimers.remove(currTimer);
	    delete currTimer;
	    currTimer = NULL;
	}

	// Make the last ball to hit the paddle das uber ball, ja!
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
	    GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
	    affectedBall->AddBallType(GameBall::UberBall);
    }

	GameItem::Activate();
	return UberBallItem::UBER_BALL_TIMER_IN_SECS;
}

void UberBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make all the balls normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->RemoveBallType(GameBall::UberBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}