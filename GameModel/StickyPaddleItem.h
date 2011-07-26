/**
 * StickyPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __STICKYPADDLEITEM_H__
#define __STICKYPADDLEITEM_H__

#include "GameItem.h"

/** 
 * Item for the sticky paddle - causes the ball to stick to the paddle
 * when it hits it (one ball at a time). This is considered a power-up
 * and it lasts for a limited time.
 */
class StickyPaddleItem : public GameItem {
public:
	static const char*  STICKY_PADDLE_ITEM_NAME;
	static const double STICKY_PADDLE_TIMER_IN_SECS;

	StickyPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~StickyPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::StickyPaddleItem;
	}
};
#endif