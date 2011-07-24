/**
 * ShieldPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SHIELDPADDLEITEM_H__
#define __SHIELDPADDLEITEM_H__

#include "GameItem.h"

class ShieldPaddleItem : public GameItem {
public:
	static const char* SHIELD_PADDLE_ITEM_NAME;
	static const double SHIELD_PADDLE_TIMER_IN_SECS;

	ShieldPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~ShieldPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::ShieldPaddleItem;
	}
};

#endif // __SHIELDPADDLEITEM_H__