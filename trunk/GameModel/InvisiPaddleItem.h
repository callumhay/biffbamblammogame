/**
 * InvisiPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INVISIPADDLEITEM_H__
#define __INVISIPADDLEITEM_H__

#include "GameItem.h"

/** 
 * Item for the invisipaddle - causes the paddle to be cloaked/mostly transparent,
 * it lasts for a limited time.
 */
class InvisiPaddleItem : public GameItem {
public:
	static const char*  INVISI_PADDLE_ITEM_NAME;
	static const double INVISI_PADDLE_TIMER_IN_SECS;

	InvisiPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~InvisiPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::InvisiPaddleItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(InvisiPaddleItem);
};
#endif // __INVISIPADDLEITEM_H__