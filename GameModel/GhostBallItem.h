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
	virtual ~GhostBallItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif