/**
 * CrazyBallItem.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "CrazyBallItem.h"

const char* CrazyBallItem::CRAZY_BALL_ITEM_NAME       = "CrazyBall";
const double CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS  = 28.0;

CrazyBallItem::CrazyBallItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(CRAZY_BALL_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Neutral) {
}

CrazyBallItem::~CrazyBallItem() {
}

double CrazyBallItem::Activate() {
	this->isActive = true;

    // Kill other crazy ball timers
    std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
        GameItemTimer* currTimer = *iter;
        if (currTimer->GetTimerItemType() == GameItem::CrazyBallItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
        }
        else {
            ++iter;
        }
    }

	// Make all the balls crazy!
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
        GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
    	
	    // Make the ball(s) crazy
	    affectedBall->AddBallType(GameBall::CrazyBall);
    }

	GameItem::Activate();
	return CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS;
}

void CrazyBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

    // Make each ball not-so crazy again
    std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
        GameBall* currBall = *ballIter;
        assert(currBall != NULL);	
        currBall->RemoveBallType(GameBall::CrazyBall);
    }

	this->isActive = false;
	GameItem::Deactivate();
}