/**
 * OneUpItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ONEUPITEM_H__
#define __ONEUPITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item representing a free life / 1UP in the game.
 */
class OneUpItem : public GameItem {
public:
	static const std::string ONE_UP_ITEM_NAME;
	static const double ONE_UP_TIMER_IN_SECS;

	OneUpItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~OneUpItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::OneUpItem;
	}
};
#endif