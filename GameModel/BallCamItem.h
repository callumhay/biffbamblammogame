/**
 * BallCamItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef _BALLCAMITEM_H_
#define _BALLCAMITEM_H_

#include "GameItem.h"

/**
 * Game item representing the Ball Camera - this is when the player's point of view
 * goes inside the ball as it is in play.
 */
class BallCamItem : public GameItem {
public:
	static const char* BALL_CAM_ITEM_NAME;
	static const double BALL_CAM_TIMER_IN_SECS;

	BallCamItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~BallCamItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::BallCamItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(BallCamItem);
};
#endif