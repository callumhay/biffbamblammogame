/**
 * LaserPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "LaserPaddleItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS = 15.0;
const char* LaserPaddleItem::LASER_PADDLE_ITEM_NAME      = "LaserBulletPaddle";

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

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::LaserBulletPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the laser paddle timers from the list of active timers
	for (int i = 0; i < static_cast<int>(removeTimers.size()); i++) {
	    GameItemTimer* currTimer = removeTimers[i];
	    activeTimers.remove(currTimer);
	    delete currTimer;
	    currTimer = NULL;
	}

	// Make the paddle have laser shooting abilities
	paddle->AddPaddleType(PlayerPaddle::LaserBulletPaddle);

	GameItem::Activate();
	return LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS;
}

void LaserPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	paddle->RemovePaddleType(PlayerPaddle::LaserBulletPaddle);
	this->isActive = false;
	GameItem::Deactivate();
}