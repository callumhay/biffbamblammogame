/**
 * BallSpeedItem.cpp
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

#include "BallSpeedItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double BallSpeedItem::BALL_SPEED_TIMER_IN_SECS	= 20.0;
const char* BallSpeedItem::SLOW_BALL_ITEM_NAME        = "SlowBall";
const char* BallSpeedItem::FAST_BALL_ITEM_NAME        = "FastBall";

BallSpeedItem::BallSpeedItem(const BallSpeedType type, const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem((type==SlowBall)?SLOW_BALL_ITEM_NAME:FAST_BALL_ITEM_NAME, 
         spawnOrigin, dropDir, gameModel, (type==SlowBall) ? GameItem::Good : GameItem::Bad), spdType(type) {
}

BallSpeedItem::~BallSpeedItem() {
}

void BallSpeedItem::SwitchSpeed(BallSpeedType newSpd) {
	this->spdType = newSpd;
	switch (newSpd) {
		case FastBall:
			this->disposition = GameItem::Bad;
			this->name = BallSpeedItem::FAST_BALL_ITEM_NAME;
			break;
		case SlowBall:
			this->disposition = GameItem::Good;
			this->name = BallSpeedItem::SLOW_BALL_ITEM_NAME;
			break;
		default:
			assert(false);
			break;
	}
}

/**
 * Called to activate this item - the effect will be to slow down
 * the game ball and start a timer for this effect to ware out.
 * Returns: A new timer associated with the effect.
 * NOTE: This function CREATES a new heap object and is not responsible
 * for its destruction, BECAREFUL!
 */
double BallSpeedItem::Activate() {
	this->isActive = true;

	bool foundSlowDownActive = false;
	bool foundSpeedUpActive  = false;

	// Kill other ball speed timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		bool foundSlowDown = currTimer->GetTimerItemType() == GameItem::BallSlowDownItem;
		bool foundSpeedUp  = currTimer->GetTimerItemType() == GameItem::BallSpeedUpItem;
		if (foundSlowDown || foundSpeedUp) {
			removeTimers.push_back(currTimer);
			foundSlowDownActive |= foundSlowDown;
			foundSpeedUpActive |= foundSpeedUp;
		}
	}

	// Remove the ball speed related timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
		GameItemTimer* currTimer = removeTimers[i];
		activeTimers.remove(currTimer);
		delete currTimer;
		currTimer = NULL;
	}

	// Should never be able to find both active simulataneously
	assert(!(foundSlowDownActive && foundSpeedUpActive));

	// Check for the cases where this effect simply cancels out another
	if (this->spdType == SlowBall && foundSpeedUpActive || this->spdType == FastBall && foundSlowDownActive) {
		return GameItemTimer::ZERO_TIME_TIMER_IN_SECS;
	}

	// Activate the actual effect of this speed-ball item for the last ball to hit the paddle
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
	    GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);

	    // Figure out how the item will effect the ball's speed
	    switch (this->spdType) {
		    case FastBall:
			    affectedBall->IncreaseSpeed();
                affectedBall->AddBallType(GameBall::FastBall);
			    break;
		    case SlowBall:
			    affectedBall->DecreaseSpeed();
                affectedBall->AddBallType(GameBall::SlowBall);
			    break;
		    default:
			    assert(false);
			    break;
	    }
    }

	GameItem::Activate();
	return BallSpeedItem::BALL_SPEED_TIMER_IN_SECS;
}

/**
 * Called to deactivate the effect of this timer.
 */
void BallSpeedItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Shut off the item for each of the balls
    GameBall::BallType removeType = this->spdType == FastBall ? GameBall::FastBall : GameBall::SlowBall;
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	
    for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
        currBall->TurnOffBoost();
        currBall->TurnOffImpulse();
		currBall->SetSpeed(GameBall::GetNormalSpeed());
        currBall->RemoveBallType(removeType);
	}

	this->isActive = false;
	GameItem::Deactivate();
}