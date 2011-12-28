/**
 * LifeUpItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LIFEUPITEM_H__
#define __LIFEUPITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item representing a life-up in the game.
 */
class LifeUpItem : public GameItem {
public:
	static const char*  LIFE_UP_ITEM_NAME;
	static const double LIFE_UP_TIMER_IN_SECS;

	LifeUpItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~LifeUpItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::LifeUpItem;
	}
};

#endif // __LIFEUPITEM_H__