/**
 * PaddleCamItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLECAMITEM_H__
#define __PADDLECAMITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

/**
 * Item for placing the user's viewpoint inside the paddle looking upwards
 * into the level. Brutal power-down of doom!
 */
class PaddleCamItem : public GameItem {
public:
	static const char* PADDLE_CAM_ITEM_NAME;
	static const double PADDLE_CAM_TIMER_IN_SECS;

	PaddleCamItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~PaddleCamItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::PaddleCamItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(PaddleCamItem);
};
#endif