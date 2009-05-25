#ifndef __LASERPADDLEITEM_H__
#define __LASERPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

class LaserPaddleItem : public GameItem {
public:
	static const std::string LASER_PADDLE_ITEM_NAME;
	static const double LASER_PADDLE_TIMER_IN_SECS;

	LaserPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~LaserPaddleItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif