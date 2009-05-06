#include "BallSizeItem.h"
#include "GameModel.h"
#include "GameBall.h"

const double BallSizeItem::BALL_SIZE_TIMER_IN_SECS	= 0.0;

const std::string BallSizeItem::BALL_GROW_ITEM_NAME		= "Ball Grow (Power-up)";
const std::string BallSizeItem::BALL_SHRINK_ITEM_NAME = "Ball Shrink (Power-down)";

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