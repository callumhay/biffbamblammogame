/**
 * PaddleCamItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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