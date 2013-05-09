/**
 * BallSizeItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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

	static const char* BALL_GROW_ITEM_NAME;
	static const char* BALL_SHRINK_ITEM_NAME;
	static const double BALL_SIZE_TIMER_IN_SECS;

	BallSizeItem(const BallSizeChangeType type, const Point2D &spawnOrigin, GameModel *gameModel);
	~BallSizeItem();

	double Activate();
	void Deactivate();
	
	GameItem::ItemType GetItemType() const {
		return (this->sizeChangeType == ShrinkBall) ? GameItem::BallShrinkItem : GameItem::BallGrowItem;
	}

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
	BallSizeChangeType sizeChangeType;
    DISALLOW_COPY_AND_ASSIGN(BallSizeItem);
};
#endif