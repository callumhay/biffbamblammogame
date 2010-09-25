/**
 * UberBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "UberBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double UberBallItem::UBER_BALL_TIMER_IN_SECS	= 10.0;
const char* UberBallItem::UBER_BALL_ITEM_NAME       = "UberBall";

UberBallItem::UberBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(UBER_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
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
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);
	affectedBall->AddBallType(GameBall::UberBall);

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