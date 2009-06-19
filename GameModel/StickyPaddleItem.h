#ifndef __STICKYPADDLEITEM_H__
#define __STICKYPADDLEITEM_H__

#include "GameItem.h"

/** 
 * Item for the sticky paddle - causes the ball to stick to the paddle
 * when it hits it (one ball at a time). This is considered a power-up
 * and it lasts for a limited time.
 */
class StickyPaddleItem : public GameItem {
public:
	static const std::string STICKY_PADDLE_ITEM_NAME;
	static const double STICKY_PADDLE_TIMER_IN_SECS;

	StickyPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~StickyPaddleItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif