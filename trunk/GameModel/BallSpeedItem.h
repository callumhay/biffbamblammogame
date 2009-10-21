#ifndef __BALLSPEEDITEM_H__
#define __BALLSPEEDITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item associated with speeding up or slowing down the ball.
 */
class BallSpeedItem : public GameItem {

public:
	enum BallSpeedType { FastBall, SlowBall };

private:
	BallSpeedType spdType;
	void SwitchSpeed(BallSpeedType newSpd);

public:
	static const double BALL_SPEED_TIMER_IN_SECS;
	static const std::string SLOW_BALL_ITEM_NAME;
	static const std::string FAST_BALL_ITEM_NAME;

	virtual const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	BallSpeedItem(const BallSpeedType type, const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~BallSpeedItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif