/**
 * MultiBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

MultiBallItem::MultiBallItem(const Point2D &spawnOrigin, GameModel *gameModel, NumMultiBalls numBalls) : 
GameItem(MultiBallItem::NameFromNumBalls(numBalls), spawnOrigin, gameModel, GameItem::Good), 
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
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);

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