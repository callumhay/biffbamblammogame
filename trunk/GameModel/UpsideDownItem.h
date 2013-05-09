/**
 * UpsideDownItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __UPSIDEDOWNITEM_H__
#define __UPSIDEDOWNITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class Point2D;

/**
 * Power-down item responsible for turning the level upside down.
 */
class UpsideDownItem : public GameItem {

public:
	static const char*  UPSIDEDOWN_ITEM_NAME;
	static const double UPSIDEDOWN_TIMER_IN_SECS;

	UpsideDownItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~UpsideDownItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::UpsideDownItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(UpsideDownItem);
};
#endif