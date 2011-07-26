/**
 * LaserBeamPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERBEAMPADDLEITEM_H_
#define __LASERBEAMPADDLEITEM_H_

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class LaserBeamPaddleItem : public GameItem {
public:
	static const char* LASER_BEAM_PADDLE_ITEM_NAME;
	static const double LASER_BEAM_PADDLE_TIMER_IN_SECS;

	LaserBeamPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~LaserBeamPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::LaserBeamPaddleItem;
	}
};

#endif // __LASERBEAMPADDLEITEM_H_