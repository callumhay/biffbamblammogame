#include "LaserPaddleItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS	= 10.0;
const std::string LaserPaddleItem::LASER_PADDLE_ITEM_NAME = "Laser Paddle (Power-up)";

LaserPaddleItem::LaserPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(LASER_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

LaserPaddleItem::~LaserPaddleItem() {
}

double LaserPaddleItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other laser paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == LaserPaddleItem::LASER_PADDLE_ITEM_NAME) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the laser paddle timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Make the paddle have laser shooting abilities
	paddle->AddPaddleType(PlayerPaddle::LaserPaddle);

	GameItem::Activate();
	return LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS;
}

void LaserPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	
	// Make the ball normal again
	paddle->RemovePaddleType(PlayerPaddle::LaserPaddle);
	this->isActive = false;
	GameItem::Deactivate();
}