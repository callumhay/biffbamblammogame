/**
 * LaserPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserPaddleItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS	= 15.0;
const std::string LaserPaddleItem::LASER_PADDLE_ITEM_NAME = "LaserBulletPaddle";

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
		if (currTimer->GetTimerItemType() == GameItem::LaserBulletPaddleItem) {
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
	
	// Make the paddle normal again
	paddle->RemovePaddleType(PlayerPaddle::LaserBulletPaddle);
	this->isActive = false;
	GameItem::Deactivate();
}