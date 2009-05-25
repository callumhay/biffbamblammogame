#ifndef __POISONPADDLEITEM_H__
#define __POISONPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item that will 'poison' the paddle - has all kinds of terrible effects on the
 * player and their ability to control the paddle. This effect has a timer.
 */
class PoisonPaddleItem : public GameItem {
public:
	static const std::string POISON_PADDLE_ITEM_NAME;
	static const double POISON_PADDLE_TIMER_IN_SECS;

	PoisonPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~PoisonPaddleItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif