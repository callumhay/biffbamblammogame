/**
 * PoisonPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __POISONPADDLEITEM_H__
#define __POISONPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item that will 'poison' the paddle - has all kinds of terrible effects on the
 * player and their ability to control the paddle. This effect has a timer.
 */
class PoisonPaddleItem : public GameItem {
public:
	static const char* POISON_PADDLE_ITEM_NAME;
	static const double POISON_PADDLE_TIMER_IN_SECS;

	PoisonPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~PoisonPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::PoisonPaddleItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(PoisonPaddleItem);
};
#endif