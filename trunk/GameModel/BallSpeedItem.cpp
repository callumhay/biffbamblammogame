/**
 * BallSpeedItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BallSpeedItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double BallSpeedItem::BALL_SPEED_TIMER_IN_SECS	= 20.0;
const std::string BallSpeedItem::SLOW_BALL_ITEM_NAME = "SlowBall";
const std::string BallSpeedItem::FAST_BALL_ITEM_NAME = "FastBall";

BallSpeedItem::BallSpeedItem(const BallSpeedType type, const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem((type==SlowBall)?SLOW_BALL_ITEM_NAME:FAST_BALL_ITEM_NAME, spawnOrigin, gameModel, (type==SlowBall)?GameItem::Good:GameItem::Bad), spdType(type) {
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

	// Kill other ball speed timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::BallSlowDownItem || currTimer->GetTimerItemType() == GameItem::BallSpeedUpItem) {
			removeTimers.push_back(currTimer);
		}
	}

	// Remove the ball speed related timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
		GameItemTimer* currTimer = removeTimers[i];
		activeTimers.remove(currTimer);
		delete currTimer;
		currTimer = NULL;
	}

	// Activate the actual effect of this speed-ball item for the last ball to hit the paddle
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);

	// Figure out how the item will effect the ball's speed
	switch (this->spdType) {
		case FastBall:
			affectedBall->IncreaseSpeed();
			break;
		case SlowBall:
			affectedBall->DecreaseSpeed();
			break;
		default:
			assert(false);
			break;
	}

	// If all the balls are normal speed then all previous power-ups/downs involving
	// ball speed have been cancelled out an no item is active
	if (affectedBall->GetSpeed() == GameBall::NormalSpeed) {
		return GameItemTimer::ZERO_TIME_TIMER_IN_SECS;
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
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->SetSpeed(GameBall::NormalSpeed);
	}

	this->isActive = false;
	GameItem::Deactivate();
}