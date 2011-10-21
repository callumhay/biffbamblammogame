/**
 * MagnetPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MagnetPaddleItem.h"
#include "GameModel.h"

const char* MagnetPaddleItem::MAGNET_PADDLE_ITEM_NAME       = "MagnetPaddle";
const double MagnetPaddleItem::MAGNET_PADDLE_TIMER_IN_SECS  = 50.0;

MagnetPaddleItem::MagnetPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(MagnetPaddleItem::MAGNET_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

MagnetPaddleItem::~MagnetPaddleItem() {
}

double MagnetPaddleItem::Activate() {
	this->isActive = true;

	// Remove any previous magnet paddle item power-up timers and replace it with this one
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::MagnetPaddleItem) {
			 removeTimers.push_back(currTimer);
		}
	}
	// Remove the magnet paddle timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
        GameItemTimer* currTimer = removeTimers[i];
        currTimer->SetDeactivateItemOnStop(false);
        activeTimers.remove(currTimer);
        delete currTimer;
        currTimer = NULL;
	}

	// Activate the magnet paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    paddle->AddPaddleType(PlayerPaddle::MagnetPaddle);
	
	GameItem::Activate();
	return MagnetPaddleItem::MAGNET_PADDLE_TIMER_IN_SECS;
}

void MagnetPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	paddle->RemovePaddleType(PlayerPaddle::MagnetPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}