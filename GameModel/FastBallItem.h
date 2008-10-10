#ifndef __FASTBALLITEM_H__
#define __FASTBALLITEM_H__

#include "../Utils/Point.h"
#include "GameItem.h"

class FastBallItem : public GameItem {
public:
	static const std::string FAST_BALL_ITEM_NAME;
	static const double FAST_BALL_TIMER_IN_SECS;

	FastBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~FastBallItem();

	virtual GameItemTimer* Activate();
	virtual void Deactivate();

};
#endif