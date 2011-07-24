/**
 * BlackoutItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BLACKOUTITEM_H__
#define __BLACKOUTITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * The BlackoutItem is responsible for causing a level to go pitch black
 * except for the paddle and ball.
 */
class BlackoutItem : public GameItem {

public:
	static const char* BLACKOUT_ITEM_NAME;
	static const double BLACKOUT_TIMER_IN_SECS;

	BlackoutItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~BlackoutItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::BlackoutItem;
	}
};
#endif