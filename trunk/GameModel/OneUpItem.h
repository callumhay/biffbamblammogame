#ifndef __ONEUPITEM_H__
#define __ONEUPITEM_H__

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item representing a free life / 1UP in the game.
 */
class OneUpItem : public GameItem {
public:
	static const std::string ONE_UP_ITEM_NAME;
	static const double ONE_UP_TIMER_IN_SECS;

	OneUpItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~OneUpItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif