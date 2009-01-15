#include "UberBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double UberBallItem::UBER_BALL_TIMER_IN_SECS	= 10.0;
const std::string UberBallItem::UBER_BALL_ITEM_NAME = "Uber Ball (Power-up)";

UberBallItem::UberBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(UBER_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

UberBallItem::~UberBallItem() {
}

double UberBallItem::Activate() {
	this->isActive = true;
	GameBall* ball = this->gameModel->GetGameBall();
	assert(ball != NULL);

	// Kill other uber ball timers
	// TODO: cancel out weak ball timers...

	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == UberBallItem::UBER_BALL_ITEM_NAME) {
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

	// Make the ball das uber ball, ja!
	ball->AddBallType(GameBall::UberBall);

	GameItem::Activate();
	return UberBallItem::UBER_BALL_TIMER_IN_SECS;
}

void UberBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	GameBall* ball = this->gameModel->GetGameBall();
	assert(ball != NULL);
	
	// Make the ball normal again
	ball->RemoveBallType(GameBall::UberBall);
	this->isActive = false;
	GameItem::Deactivate();
}