/**
 * OmniLaserBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __OMNILASERBALLITEM_H__
#define __OMNILASERBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameModel.h"
#include "GameItem.h"

class Point2D;

/**
 * Represents the power-neutral that makes the ball start to shoot lasers in random directions
 * outwards from itself.
 */
class OmniLaserBallItem : public GameItem {
public:
	static const char* OMNI_LASER_BALL_ITEM_NAME;
	static const double OMNI_LASER_BALL_TIMER_IN_SECS;

	OmniLaserBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~OmniLaserBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::OmniLaserBallItem;
	}
	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

private:
    GameBall* GetBallAffected() {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

    DISALLOW_COPY_AND_ASSIGN(OmniLaserBallItem);

};
#endif // __OMNILASERBALLITEM_H__