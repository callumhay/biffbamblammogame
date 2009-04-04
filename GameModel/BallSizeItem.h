#ifndef __BALLSIZEITEM_H__
#define __BALLSIZEITEM_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameItem.h"
#include "GameModel.h"

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
	virtual ~BallSizeItem();

	virtual const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	virtual double Activate();
	virtual void Deactivate();
};
#endif