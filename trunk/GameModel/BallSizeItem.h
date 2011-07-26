/**
 * BallSizeItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLSIZEITEM_H__
#define __BALLSIZEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class Point2D;

class BallSizeItem : public GameItem {
public:
	enum BallSizeChangeType { ShrinkBall, GrowBall };

private:
	BallSizeChangeType sizeChangeType;

public:
	static const char* BALL_GROW_ITEM_NAME;
	static const char* BALL_SHRINK_ITEM_NAME;
	static const double BALL_SIZE_TIMER_IN_SECS;

	BallSizeItem(const BallSizeChangeType type, const Point2D &spawnOrigin, GameModel *gameModel);
	~BallSizeItem();

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	double Activate();
	void Deactivate();
	
	GameItem::ItemType GetItemType() const {
		return (this->sizeChangeType == ShrinkBall) ? GameItem::BallShrinkItem : GameItem::BallGrowItem;
	}

};
#endif