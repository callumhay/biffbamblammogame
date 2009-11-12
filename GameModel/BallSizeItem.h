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
	static const std::string BALL_GROW_ITEM_NAME;
	static const std::string BALL_SHRINK_ITEM_NAME;
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