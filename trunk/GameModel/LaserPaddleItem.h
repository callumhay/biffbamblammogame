#ifndef __LASERPADDLEITEM_H__
#define __LASERPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

/**
 * Represents the power-up item for the paddle's laser bullet ability -
 * The paddle gains the ability to shoot laser bullets at the blocks to
 * destroy them faster.
 */
class LaserPaddleItem : public GameItem {
public:
	static const std::string LASER_PADDLE_ITEM_NAME;
	static const double LASER_PADDLE_TIMER_IN_SECS;

	LaserPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~LaserPaddleItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::LaserBulletPaddleItem;
	}
};
#endif