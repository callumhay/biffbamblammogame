/**
 * ShieldPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "ShieldPaddleItem.h"
#include "GameModel.h"

const char* ShieldPaddleItem::SHIELD_PADDLE_ITEM_NAME	      = "ShieldPaddle";
const double ShieldPaddleItem::SHIELD_PADDLE_TIMER_IN_SECS    = 25.0;

ShieldPaddleItem::ShieldPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(ShieldPaddleItem::SHIELD_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

ShieldPaddleItem::~ShieldPaddleItem() {
}

double ShieldPaddleItem::Activate() {
	this->isActive = true;

	// Remove any previous shield paddle item timers and replace it with this one
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::ShieldPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the shield paddle timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Activate the sheild paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->AddPaddleType(PlayerPaddle::ShieldPaddle);
	
	GameItem::Activate();
	return ShieldPaddleItem::SHIELD_PADDLE_TIMER_IN_SECS;
}

void ShieldPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	
	// Remove the sticky paddle effect and shoot off any balls that
	// may currently be 'stuck' to the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	paddle->RemovePaddleType(PlayerPaddle::ShieldPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}