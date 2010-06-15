/**
 * GhostBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GHOSTBALLITEM_H__
#define __GHOSTBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

class GhostBallItem : public GameItem {
public:
	static const std::string GHOST_BALL_ITEM_NAME;
	static const double GHOST_BALL_TIMER_IN_SECS;

	GhostBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~GhostBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::GhostBallItem;
	}
};
#endif