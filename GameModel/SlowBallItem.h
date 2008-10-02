#ifndef __SLOWBALLITEM_H__
#define __SLOWBALLITEM_H__

#include "../Utils/Point.h"
#include "GameItem.h"

class GameModel;

class SlowBallItem : public GameItem {
private:
	static const std::string SLOW_BALL_ITEM_NAME;
public:
	SlowBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~SlowBallItem();

	virtual void Activate();

};
#endif