#include "GhostBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double GhostBallItem::GHOST_BALL_TIMER_IN_SECS	= 15.0;
const std::string GhostBallItem::GHOST_BALL_ITEM_NAME = "Ghost Ball (Power-neutral)";

GhostBallItem::GhostBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(GHOST_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

GhostBallItem::~GhostBallItem() {
}

double GhostBallItem::Activate() {
	this->isActive = true;
	GameBall* ball = this->gameModel->GetGameBall();
	assert(ball != NULL);

	// Kill other ghost ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == GhostBallItem::GHOST_BALL_ITEM_NAME) {
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

	// Make the ball das uber ball, ja!
	ball->AddBallType(GameBall::GhostBall);

	GameItem::Activate();
	return GhostBallItem::GHOST_BALL_TIMER_IN_SECS;
}

void GhostBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	GameBall* ball = this->gameModel->GetGameBall();
	assert(ball != NULL);
	
	// Make the ball normal again
	ball->RemoveBallType(GameBall::GhostBall);
	this->isActive = false;
	GameItem::Deactivate();
}