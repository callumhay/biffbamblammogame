/**
 * FireBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "FireBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double FireBallItem::FIRE_BALL_TIMER_IN_SECS	= 22.0;
const char* FireBallItem::FIRE_BALL_ITEM_NAME			  = "FireBall";

FireBallItem::FireBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(FIRE_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

FireBallItem::~FireBallItem() {
}

double FireBallItem::Activate() {
	this->isActive = true;

	// Kill other fire ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;

		// Remove the gravity ball timers from the list of active timers
		if (currTimer->GetTimerItemType() == GameItem::FireBallItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			++iter;
		}
	}

	// Make the last ball to hit the paddle into a fire ball
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);
	affectedBall->AddBallType(GameBall::FireBall);

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