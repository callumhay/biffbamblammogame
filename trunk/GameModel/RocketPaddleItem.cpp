/**
 * RocketPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "RocketPaddleItem.h"
#include "GameModel.h"

const double RocketPaddleItem::ROCKET_PADDLE_TIMER_IN_SECS	= 0.0;						// The rocket is not a timed power-up, it's activated when shot
const char* RocketPaddleItem::ROCKET_PADDLE_ITEM_NAME			  = "RocketPaddle";

RocketPaddleItem::RocketPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(RocketPaddleItem::ROCKET_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

RocketPaddleItem::~RocketPaddleItem() {
}

double RocketPaddleItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	paddle->AddPaddleType(PlayerPaddle::RocketPaddle);

	GameItem::Activate();
	return RocketPaddleItem::ROCKET_PADDLE_TIMER_IN_SECS;
}

void RocketPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	this->isActive = false;
	GameItem::Deactivate();
}