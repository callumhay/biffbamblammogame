/**
 * MultiBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MultiBallItem.h"
#include "GameModel.h"

const std::string MultiBallItem::MULTI3_BALL_ITEM_NAME	= "MultiBall3";
const std::string MultiBallItem::MULTI5_BALL_ITEM_NAME	= "MultiBall5";

const double MultiBallItem::MULTI_BALL_TIMER_IN_SECS	= 0.0;
const double MultiBallItem::BALL_COLLISIONS_DISABLED_DURATION = 0.5;

// Warning: if you make MAX_NUM_SPLITBALLS too big then the deviation from the center will be huge to keep the balls
// from colliding when they are initially spawned and could result in collision issues
const float MultiBallItem::MIN_SPLIT_DEGREES = 360.0f / static_cast<float>(MAX_NUM_SPLITBALLS + 1);

MultiBallItem::MultiBallItem(const Point2D &spawnOrigin, GameModel *gameModel, NumMultiBalls numBalls) : 
GameItem(MultiBallItem::NameFromNumBalls(numBalls), spawnOrigin, gameModel, GameItem::Good), numNewSpawnedBalls(static_cast<unsigned int>(numBalls)-1) {
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
	GameBall::BallSpeed ballSpd = affectedBall->GetSpeed();
	if (ballSpd == GameBall::ZeroSpeed) {
		assert(false);
		ballSpd = GameBall::NormalSpeed;
	}

	// Create all the copies of the current ball
	for (unsigned int copyNum = 0; copyNum < this->numNewSpawnedBalls; copyNum++) {
		GameBall* newBall = new GameBall(*affectedBall);

		// We need to change the moving direction of the copied ball to something random but also reasonable:
		currRotationInDegs += MultiBallItem::MIN_SPLIT_DEGREES;
		newBall->SetVelocity(ballSpd, Rotate(currRotationInDegs, ballDir));

		newBalls.push_back(newBall);
	}


	// Now add all the newly created balls into the game model's list of balls
	for (std::vector<GameBall*>::iterator iter = newBalls.begin(); iter != newBalls.end(); iter++) {
		gameBalls.push_back(*iter);
	}

	// Go through all the game balls and disable their ball-ball collisions for
	// a brief amount of time until they are all seperated
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); iter++) {
		GameBall* currBall = *iter;
		if (currBall->CanCollide()) {
			currBall->SetBallCollisionsDisabled(MultiBallItem::BALL_COLLISIONS_DISABLED_DURATION);
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