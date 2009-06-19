#include "StickyPaddleItem.h"
#include "GameModel.h"

const std::string StickyPaddleItem::STICKY_PADDLE_ITEM_NAME	= "Sticky Paddle (Power-up)";
const double StickyPaddleItem::STICKY_PADDLE_TIMER_IN_SECS  = 20.0;

StickyPaddleItem::StickyPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(StickyPaddleItem::STICKY_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

StickyPaddleItem::~StickyPaddleItem() {
}

/**
 * Activate the sticky paddle item so that the paddle becomes 'sticky' such
 * that balls will stick to it.
 * Returns: duration of the sticky paddle item.
 */
double StickyPaddleItem::Activate() {
	this->isActive = true;

	// Remove any previous sticky paddle item power-up timers and replace it with this one
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == StickyPaddleItem::STICKY_PADDLE_ITEM_NAME) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the sticky paddle timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Activate the sticky paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->AddPaddleType(PlayerPaddle::StickyPaddle);
	
	GameItem::Activate();
	return StickyPaddleItem::STICKY_PADDLE_TIMER_IN_SECS;
}

/**
 * Deactivate the sticky paddle item if it's still active.
 */
void StickyPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	
	// Remove the sticky paddle effect and shoot off any balls that
	// may currently be 'stuck' to the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	assert((paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle);

	paddle->Shoot(this->gameModel);
	paddle->RemovePaddleType(PlayerPaddle::StickyPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}