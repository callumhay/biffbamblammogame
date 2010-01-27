/**
 * InvisiBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "InvisiBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double InvisiBallItem::INVISI_BALL_TIMER_IN_SECS	= 12.0;
const std::string InvisiBallItem::INVISI_BALL_ITEM_NAME = "InvisiBall";

InvisiBallItem::InvisiBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(INVISI_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

InvisiBallItem::~InvisiBallItem() {
}

double InvisiBallItem::Activate() {
	this->isActive = true;

	// Kill all other invisiball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::InvisiBallItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the invisiball timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Make the last ball to hit the paddle invisible!
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);
	affectedBall->AddBallType(GameBall::InvisiBall);

	GameItem::Activate();
	return InvisiBallItem::INVISI_BALL_TIMER_IN_SECS;
}

void InvisiBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Take away all of the balls' invisi-clocks of doom!?
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->RemoveBallType(GameBall::InvisiBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}