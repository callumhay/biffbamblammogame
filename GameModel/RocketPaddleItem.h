/**
 * RocketPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */


#ifndef __ROCKETPADDLEITEM_H__
#define __ROCKETPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

/**
 * Represents the power-up item for the paddle's rocket bullet firing ability -
 * the paddle gains the ability to fire a single rocket up at the level that destroys
 * a whole bunch of blocks.
 */
class RocketPaddleItem : public GameItem {
public:
	static const char* ROCKET_PADDLE_ITEM_NAME;
	static const double ROCKET_PADDLE_TIMER_IN_SECS;

	RocketPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~RocketPaddleItem ();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::RocketPaddleItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(RocketPaddleItem);
};
#endif // __ROCKETPADDLEITEM_H__