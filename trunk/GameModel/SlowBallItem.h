#ifndef __SLOWBALLITEM_H__
#define __SLOWBALLITEM_H__

#include "../Utils/Point.h"
#include "GameItem.h"

class GameModel;

class SlowBallItem : public GameItem {

public:
	static const double SLOW_BALL_TIMER_IN_SECS;
	static const std::string SLOW_BALL_ITEM_NAME;

	SlowBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~SlowBallItem();

	virtual GameItemTimer* Activate();
	virtual void Deactivate();

};
#endif