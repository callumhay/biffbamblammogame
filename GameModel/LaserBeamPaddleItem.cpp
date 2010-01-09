/**
 * LaserBeamPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserBeamPaddleItem.h"
#include "GameModel.h"

const double LaserBeamPaddleItem::LASER_BEAM_PADDLE_TIMER_IN_SECS	= 0.0; // The laser beam is not a timed power-up, it's activated when shot
const std::string LaserBeamPaddleItem::LASER_BEAM_PADDLE_ITEM_NAME = "LaserBeamPaddle";

LaserBeamPaddleItem::LaserBeamPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(LASER_BEAM_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

LaserBeamPaddleItem::~LaserBeamPaddleItem() {
}

double LaserBeamPaddleItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other laser beam paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::LaserBeamPaddleItem) {
			delete currTimer;
			currTimer = NULL;
			iter = activeTimers.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Make the paddle have laser beam blasting abilities
	paddle->AddPaddleType(PlayerPaddle::LaserBeamPaddle);

	GameItem::Activate();
	return LaserBeamPaddleItem::LASER_BEAM_PADDLE_TIMER_IN_SECS;
}

void LaserBeamPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	
	// Make the paddle normal again - remove the laser beam power-up
	paddle->RemovePaddleType(PlayerPaddle::LaserBeamPaddle);
	this->isActive = false;
	GameItem::Deactivate();
}