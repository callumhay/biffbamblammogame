#ifndef __PADDLECAMITEM_H__
#define __PADDLECAMITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

/**
 * Item for placing the user's viewpoint inside the paddle looking upwards
 * into the level. Brutal power-down of doom!
 */
class PaddleCamItem : public GameItem {
public:
	static const std::string PADDLE_CAM_ITEM_NAME;
	static const double PADDLE_CAM_TIMER_IN_SECS;

	PaddleCamItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~PaddleCamItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif