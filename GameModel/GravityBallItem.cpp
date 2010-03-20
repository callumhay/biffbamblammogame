#include "GravityBallItem.h"

const char* GravityBallItem::GRAVITY_BALL_ITEM_NAME				= "GravityBall";
const double GravityBallItem::GRAVITY_BALL_TIMER_IN_SECS	= 18.0;

GravityBallItem::GravityBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(GRAVITY_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

GravityBallItem::~GravityBallItem() {
}

double GravityBallItem::Activate() {
	this->isActive = true;

	// Kill other ghost ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;

		// Remove the gravity ball timers from the list of active timers
		if (currTimer->GetTimerItemType() == GameItem::GravityBallItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			++iter;
		}
	}

	// Make the last ball to hit the paddle into a gravity ball
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);
	
	affectedBall->AddBallType(GameBall::GraviBall);

	GameItem::Activate();
	return GravityBallItem::GRAVITY_BALL_TIMER_IN_SECS;
}

void GravityBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Remove the item effect from the balls
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::GraviBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}