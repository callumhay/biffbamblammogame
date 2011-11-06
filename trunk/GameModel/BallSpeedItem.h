/**
 * BallSpeedItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	static const char* SLOW_BALL_ITEM_NAME;
	static const char* FAST_BALL_ITEM_NAME;

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	BallSpeedItem(const BallSpeedType type, const Point2D &spawnOrigin, GameModel *gameModel);
	~BallSpeedItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return (this->spdType == FastBall) ? GameItem::BallSpeedUpItem : GameItem::BallSlowDownItem;
	}
};
#endif