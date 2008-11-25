#ifndef __INVISIBALLITEM_H__
#define __INVISIBALLITEM_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameItem.h"

class InvisiBallItem : public GameItem {
public:
	static const std::string INVISI_BALL_ITEM_NAME;
	static const double INVISI_BALL_TIMER_IN_SECS;

	InvisiBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~InvisiBallItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif