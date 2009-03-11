#include "InvisiBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double InvisiBallItem::INVISI_BALL_TIMER_IN_SECS	= 12.0;
const std::string InvisiBallItem::INVISI_BALL_ITEM_NAME = "InvisiBall (Power-down)";

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

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == InvisiBallItem::INVISI_BALL_ITEM_NAME) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the uber/weak ball timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Make all the balls invisible!
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->AddBallType(GameBall::InvisiBall);
	}

	GameItem::Activate();
	return InvisiBallItem::INVISI_BALL_TIMER_IN_SECS;
}

void InvisiBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Take away all of the balls' invisi-clocks of doom!?
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->RemoveBallType(GameBall::InvisiBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}