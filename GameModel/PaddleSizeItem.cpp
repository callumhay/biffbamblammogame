#include "PaddleSizeItem.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

const double PaddleSizeItem::PADDLE_SIZE_TIMER_IN_SECS	= 0.0;

const std::string PaddleSizeItem::PADDLE_GROW_ITEM_NAME		= "Paddle Grow (Power-up)";
const std::string PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME = "Paddle Shrink (Power-down)";

PaddleSizeItem::PaddleSizeItem(const PaddleSizeChangeType type, const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(((type == ShrinkPaddle) ? PADDLE_SHRINK_ITEM_NAME : PADDLE_GROW_ITEM_NAME), spawnOrigin, gameModel, 
				 ((type == ShrinkPaddle) ? GameItem::Bad : GameItem::Good)), sizeChangeType(type) {
}

PaddleSizeItem::~PaddleSizeItem() {
}

double PaddleSizeItem::Activate() {
	this->isActive = true;

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	bool paddleSizeChanged = false;

	switch(this->sizeChangeType) {
		case ShrinkPaddle:
			paddleSizeChanged = paddle->DecreasePaddleSize();
			break;
		case GrowPaddle:
			paddleSizeChanged = paddle->IncreasePaddleSize();
			break;
		default:
			assert(false);
			break;
	}

	// Nothing gets activated if there was no effect...
	if (paddleSizeChanged) {
		GameItem::Activate();
	}
	
	return PaddleSizeItem::PADDLE_SIZE_TIMER_IN_SECS;
}

/** 
 * Does nothing since the paddle size item has no timer or way of being cancelled out.
 */
void PaddleSizeItem::Deactivate() {
}