/**
 * BallSizeItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BallSizeItem.h"
#include "GameModel.h"
#include "GameBall.h"

const double BallSizeItem::BALL_SIZE_TIMER_IN_SECS	= 0.0;

const char* BallSizeItem::BALL_GROW_ITEM_NAME		= "BallGrow";
const char* BallSizeItem::BALL_SHRINK_ITEM_NAME = "BallShrink";

BallSizeItem::BallSizeItem(const BallSizeChangeType type, const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(((type == ShrinkBall) ? BallSizeItem::BALL_SHRINK_ITEM_NAME : BallSizeItem::BALL_GROW_ITEM_NAME), spawnOrigin, gameModel, 
				 ((type == ShrinkBall) ? GameItem::Bad : GameItem::Good)), sizeChangeType(type) {
}

BallSizeItem::~BallSizeItem() {
}

double BallSizeItem::Activate() {
	this->isActive = true;

	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	GameBall* affectedBall = *gameBalls.begin();
	assert(affectedBall != NULL);

	bool ballSizeChanged = false;

	switch(this->sizeChangeType) {
		case ShrinkBall:
			ballSizeChanged = affectedBall->DecreaseBallSize();
			break;
		case GrowBall:
			ballSizeChanged = affectedBall->IncreaseBallSize();
			break;
		default:
			assert(false);
			break;
	}

	// Nothing gets activated if there was no effect...
	if (ballSizeChanged) {
		GameItem::Activate();
	}

	return BallSizeItem::BALL_SIZE_TIMER_IN_SECS;
}

/** 
 * Does nothing since the paddle size item has no timer or way of being cancelled out.
 */
void BallSizeItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}