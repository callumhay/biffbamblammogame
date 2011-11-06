/**
 * PaddleSizeItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleSizeItem.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

const double PaddleSizeItem::PADDLE_SIZE_TIMER_IN_SECS	= 0.0;

const char* PaddleSizeItem::PADDLE_GROW_ITEM_NAME		= "PaddleGrow";
const char* PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME = "PaddleShrink";

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
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}