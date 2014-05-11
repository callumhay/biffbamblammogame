/**
 * MultiBallItem.cpp
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

#include "MultiBallItem.h"
#include "GameModel.h"

const char* MultiBallItem::MULTI3_BALL_ITEM_NAME	= "MultiBall3";
const char* MultiBallItem::MULTI5_BALL_ITEM_NAME	= "MultiBall5";

const double MultiBallItem::MULTI_BALL_TIMER_IN_SECS	= 0.0;
const double MultiBallItem::BALL_COLLISIONS_DISABLED_DURATION = 0.5;

// Warning: if you make MAX_NUM_SPLITBALLS too big then the deviation from the center will be huge to keep the balls
// from colliding when they are initially spawned and could result in collision issues
const float MultiBallItem::MIN_SPLIT_DEGREES = 360.0f / static_cast<float>(MAX_NUM_SPLITBALLS + 1);

MultiBallItem::MultiBallItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel, NumMultiBalls numBalls) : 
GameItem(MultiBallItem::NameFromNumBalls(numBalls), spawnOrigin, dropDir, gameModel, GameItem::Good), 
numNewSpawnedBalls(static_cast<unsigned int>(numBalls)-1), numBalls(numBalls) {
	assert(numNewSpawnedBalls >= MIN_NUM_SPLITBALLS && numNewSpawnedBalls <= MAX_NUM_SPLITBALLS);
}

MultiBallItem::~MultiBallItem() {
}

/**
 * Activate the multiball item - this will split the ball up into this
 * multiball item's number of balls
 */
double MultiBallItem::Activate() {
	this->isActive = true;
	float currRotationInDegs = 0.0f;

	// Go through the first game ball making 'numBalls' copies of it and setting that copy with the appropriate characteristics
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    std::list<GameBall*>::iterator ballIter = gameBalls.begin();

	GameBall* affectedBall = *ballIter;
	assert(affectedBall != NULL);
    ++ballIter;

    // Make sure we pick a ball that isn't below the paddle...
    const Point2D& paddlePos = this->gameModel->GetPlayerPaddle()->GetCenterPosition();
    while (ballIter != gameBalls.end() && (affectedBall->GetCenterPosition2D()[1] + affectedBall->GetBounds().Radius()) < paddlePos[1]) {
        affectedBall = *ballIter;
        ++ballIter;
    }

	std::vector<GameBall*> newBalls;
	newBalls.reserve(this->numNewSpawnedBalls);

	// Get the ball direction for the affected ball, if its a null vector then just make it up (no pun intended)
	Vector2D ballDir = affectedBall->GetDirection();
	if (ballDir == Vector2D(0, 0)) {
		ballDir = Vector2D(0, 1);
	}
	float ballSpd = affectedBall->GetSpeed();
	if (ballSpd == GameBall::GetZeroSpeed()) {
		assert(false);
		ballSpd = GameBall::GetNormalSpeed();
	}

    // When we copy the ball we also copy all of the item effects that have been applied to that ball,
    // therefore all timers with effects associated with the ball being copied need to be informed of
    // all the new associations...
    std::list<GameItemTimer*> relevantTimers;
    const std::list<GameItemTimer*>& activeItemTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::const_iterator iter = activeItemTimers.begin(); iter != activeItemTimers.end(); ++iter) {
        GameItemTimer* itemTimer = *iter;
        assert(itemTimer != NULL);
        if (itemTimer->GetAssociatedBalls().find(affectedBall) != itemTimer->GetAssociatedBalls().end()) {
            relevantTimers.push_back(itemTimer);
        }
    }

	// Create all the copies of the current ball
	for (unsigned int copyNum = 0; copyNum < this->numNewSpawnedBalls; copyNum++) {
		GameBall* newBall = new GameBall(*affectedBall);

		// We need to change the moving direction of the copied ball to something random but also reasonable:
		currRotationInDegs += MultiBallItem::MIN_SPLIT_DEGREES;
		newBall->SetVelocity(ballSpd, Vector2D::Rotate(currRotationInDegs, ballDir));
		newBalls.push_back(newBall);

        // The new ball requires association with all the relevant, active timers whose effects are now
        // being applied to this new ball...
        for (std::list<GameItemTimer*>::const_iterator iter = relevantTimers.begin(); iter != relevantTimers.end(); ++iter) {
            GameItemTimer* itemTimer = *iter;
            itemTimer->AddAssociatedBall(newBall);
        }
	}

	// Now add all the newly created balls into the game model's list of balls
	for (std::vector<GameBall*>::iterator iter = newBalls.begin(); iter != newBalls.end(); ++iter) {
		gameBalls.push_back(*iter);
	}

	// Go through all the game balls and disable their ball-ball collisions for
	// a brief amount of time until they are all seperated
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
		GameBall* currBall = *iter;
		if (currBall->CanCollideWithOtherBalls()) {
			currBall->SetBallBallCollisionsDisabled(MultiBallItem::BALL_COLLISIONS_DISABLED_DURATION);
		}
	}

	GameItem::Activate();
	return MultiBallItem::MULTI_BALL_TIMER_IN_SECS;
}

/**
 * Does nothing since the multiball has no timer or way of being cancelled out.
 */
void MultiBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}
