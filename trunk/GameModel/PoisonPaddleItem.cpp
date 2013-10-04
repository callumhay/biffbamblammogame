/**
 * PoisonPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PoisonPaddleItem.h"

const char* PoisonPaddleItem::POISON_PADDLE_ITEM_NAME       = "PoisonPaddle";
const double PoisonPaddleItem::POISON_PADDLE_TIMER_IN_SECS	= 18.0f;

PoisonPaddleItem::PoisonPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

PoisonPaddleItem::~PoisonPaddleItem() {
}

/**
 * Removes all other active poison items, re-poisons the paddle and
 * returns the length of the new poison timer.
 */
double PoisonPaddleItem::Activate() {
	this->isActive = true;

	// Get rid of all other active poison items
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::PoisonPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	
	// Remove the poison timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
	    GameItemTimer* currTimer = removeTimers[i];
	    activeTimers.remove(currTimer);
	    delete currTimer;
	    currTimer = NULL;
	}

	// Re-poison the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->AddPaddleType(PlayerPaddle::PoisonPaddle);
	GameItem::Activate();

	return PoisonPaddleItem::POISON_PADDLE_TIMER_IN_SECS;
}

void PoisonPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// De-poisonify the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->RemovePaddleType(PlayerPaddle::PoisonPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}