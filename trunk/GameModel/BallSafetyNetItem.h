/**
 * BallSafetyNetItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BALLSAFETYNETITEM_H__
#define __BALLSAFETYNETITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item for when there is a strip of "safety net" saving the ball from falling
 * into the abyss - this lasts as long as a ball doesn't hit it.
 */
class BallSafetyNetItem : public GameItem {

public:
	static const char* BALL_SAFETY_NET_ITEM_NAME;
	static const double BALL_SAFETY_NET_TIMER_IN_SECS;

	BallSafetyNetItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~BallSafetyNetItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::BallSafetyNetItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(BallSafetyNetItem);
};
#endif