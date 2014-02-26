/**
 * OmniLaserBallItem.cpp
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

#include "OmniLaserBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const char* OmniLaserBallItem::OMNI_LASER_BALL_ITEM_NAME      = "OmniLaserBall";
const double OmniLaserBallItem::OMNI_LASER_BALL_TIMER_IN_SECS = 16.0;

OmniLaserBallItem::OmniLaserBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(OMNI_LASER_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

OmniLaserBallItem::~OmniLaserBallItem() {
}

double OmniLaserBallItem::Activate() {
	this->isActive = true;

	// Kill other omni laser ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::OmniLaserBallItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
		}
        else {
            ++iter;
        }
	}

	// Make the ball have omni-laser shooting abilities
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
	    GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
        affectedBall->AddBallType(GameBall::OmniLaserBulletBall);
    }

	GameItem::Activate();
	return OmniLaserBallItem::OMNI_LASER_BALL_TIMER_IN_SECS;
}

void OmniLaserBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make each ball normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::OmniLaserBulletBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}