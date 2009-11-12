/**
 * GhostBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GhostBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double GhostBallItem::GHOST_BALL_TIMER_IN_SECS	= 15.0;
const std::string GhostBallItem::GHOST_BALL_ITEM_NAME = "GhostBall";

GhostBallItem::GhostBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(GHOST_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

GhostBallItem::~GhostBallItem() {
}

double GhostBallItem::Activate() {
	this->isActive = true;

	// Kill other ghost ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::GhostBallItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the ghostball timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Make the last ball to hit the paddle into a ghost ball
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);
	affectedBall->AddBallType(GameBall::GhostBall);

	GameItem::Activate();
	return GhostBallItem::GHOST_BALL_TIMER_IN_SECS;
}

void GhostBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make each ball normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::GhostBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}