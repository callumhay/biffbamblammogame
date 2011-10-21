/**
 * MagnetPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MAGNETPADDLEITEM_H__
#define __MAGNETPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class MagnetPaddleItem : public GameItem {
public:
	static const char* MAGNET_PADDLE_ITEM_NAME;
	static const double MAGNET_PADDLE_TIMER_IN_SECS;

	MagnetPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~MagnetPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
        return GameItem::MagnetPaddleItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(MagnetPaddleItem);
};
#endif // __MAGNETPADDLEITEM_H__