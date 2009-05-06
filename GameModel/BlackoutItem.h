#ifndef __BLACKOUTITEM_H__
#define __BLACKOUTITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * The BlackoutItem is responsible for causing a level to go pitch black
 * except for the paddle and ball.
 */
class BlackoutItem : public GameItem {

public:
	static const std::string BLACKOUT_ITEM_NAME;
	static const double BLACKOUT_TIMER_IN_SECS;

	BlackoutItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~BlackoutItem();

	virtual double Activate();
	virtual void Deactivate();

};
#endif