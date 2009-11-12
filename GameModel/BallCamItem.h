#ifndef _BALLCAMITEM_H_
#define _BALLCAMITEM_H_

#include "GameItem.h"

/**
 * Game item representing the Ball Camera - this is when the player's point of view
 * goes inside the ball as it is in play.
 */
class BallCamItem : public GameItem {
public:
	static const std::string BALL_CAM_ITEM_NAME;
	static const double BALL_CAM_TIMER_IN_SECS;

	BallCamItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~BallCamItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::BallCamItem;
	}

};
#endif