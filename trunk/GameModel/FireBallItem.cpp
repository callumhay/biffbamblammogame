/**
 * FireBallItem.cpp
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

#include "FireBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double FireBallItem::FIRE_BALL_TIMER_IN_SECS	= 22.0;
const char* FireBallItem::FIRE_BALL_ITEM_NAME			  = "FireBall";

FireBallItem::FireBallItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(FIRE_BALL_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Neutral) {
}

FireBallItem::~FireBallItem() {
}

double FireBallItem::Activate() {
	this->isActive = true;

	// Kill other fire ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;

		// Remove the fire ball timers from the list of active timers
		if (currTimer->GetTimerItemType() == GameItem::FireBallItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else if (currTimer->GetTimerItemType() == GameItem::IceBallItem) {

            // EVENT: Iceball(s) are being cancelled by a fireball item
            const std::set<const GameBall*>& affectedBalls = currTimer->GetAssociatedBalls();
            for (std::set<const GameBall*>::const_iterator ballIter = affectedBalls.begin();
                 ballIter != affectedBalls.end(); ++ballIter) {

                const GameBall* currBall = *ballIter;
                assert(currBall != NULL);
                GameEventManager::Instance()->ActionIceBallCanceledByFireBall(*currBall);
            }

			// If there's an ice ball item going right now then the effects just cancel each other out
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
			return 0.0;
		}
		else {
			++iter;
		}
	}

	// Make the last ball to hit the paddle into a fire ball
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
	    GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
	    affectedBall->AddBallType(GameBall::FireBall);
    }

	GameItem::Activate();
	return FireBallItem::FIRE_BALL_TIMER_IN_SECS;
}

void FireBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make each ball normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::FireBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}