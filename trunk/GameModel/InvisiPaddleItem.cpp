/**
 * InvisiPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "InvisiPaddleItem.h"
#include "GameModel.h"

const char* InvisiPaddleItem::INVISI_PADDLE_ITEM_NAME       = "InvisiPaddle";
const double InvisiPaddleItem::INVISI_PADDLE_TIMER_IN_SECS  = 20.0;

InvisiPaddleItem::InvisiPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(InvisiPaddleItem::INVISI_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

InvisiPaddleItem::~InvisiPaddleItem() {
}

double InvisiPaddleItem::Activate() {
	this->isActive = true;

	// Remove any previous invisipaddle item power-up timers and replace it with this one
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::InvisiPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the invisiball timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
        GameItemTimer* currTimer = removeTimers[i];
        currTimer->SetDeactivateItemOnStop(false);
        activeTimers.remove(currTimer);
        delete currTimer;
        currTimer = NULL;
	}

	// Activate the invisipaddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    paddle->AddPaddleType(PlayerPaddle::InvisiPaddle);
	
	GameItem::Activate();
	return InvisiPaddleItem::INVISI_PADDLE_TIMER_IN_SECS;
}

void InvisiPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	paddle->RemovePaddleType(PlayerPaddle::InvisiPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}