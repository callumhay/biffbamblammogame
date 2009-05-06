#include "BallSafetyNetItem.h"
#include "GameLevel.h"
#include "GameEventManager.h"

const std::string BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME	= "Ball Safety Net (power-up)";
const double BallSafetyNetItem::BALL_SAFETY_NET_TIMER_IN_SECS		= 0.0;

BallSafetyNetItem::BallSafetyNetItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

BallSafetyNetItem::~BallSafetyNetItem() {
}

double BallSafetyNetItem::Activate() {
	this->isActive = true;

	GameLevel* currLevel = this->gameModel->GetCurrentLevel();
	bool safetyNetActiveAlready = currLevel->IsBallSafetyNetActive();
	
	// Add the ball safety net to the level
	currLevel->ToggleBallSafetyNet(true);
	if (!safetyNetActiveAlready) {
		// EVENT: We just created a brand new ball safety net...
		GameEventManager::Instance()->ActionBallSafetyNetCreated();

		// We activated the game item (wasn't active before)
		GameItem::Activate();
	}
	return BallSafetyNetItem::BALL_SAFETY_NET_TIMER_IN_SECS;
}

void BallSafetyNetItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}