#ifndef __UBERBALLITEM_H__
#define __UBERBALLITEM_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameItem.h"

class UberBallItem : public GameItem {
public:
	static const std::string UBER_BALL_ITEM_NAME;
	static const double UBER_BALL_TIMER_IN_SECS;

	UberBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~UberBallItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif