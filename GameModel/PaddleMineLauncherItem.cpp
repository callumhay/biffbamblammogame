/**
 * PaddleMineLauncherItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleMineLauncherItem.h"
#include "GameModel.h"

const double PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_TIMER_IN_SECS	= 10.0;
const char* PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_ITEM_NAME	    = "MineLauncherPaddle";

PaddleMineLauncherItem::PaddleMineLauncherItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

PaddleMineLauncherItem::~PaddleMineLauncherItem() {
}

double PaddleMineLauncherItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other mine launcher paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::MineLauncherPaddleItem) {
			delete currTimer;
			currTimer = NULL;
			iter = activeTimers.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Make the paddle have mine launching abilities
	paddle->AddPaddleType(PlayerPaddle::MineLauncherPaddle);

	GameItem::Activate();
	return PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_TIMER_IN_SECS;
}

void PaddleMineLauncherItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    paddle->RemovePaddleType(PlayerPaddle::MineLauncherPaddle);
    
    this->isActive = false;
	GameItem::Deactivate();
}