/**
 * FireBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __FIREBALLITEM_H__
#define __FIREBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameModel.h"
#include "GameItem.h"

class Point2D;

class FireBallItem : public GameItem {
public:
	static const char* FIRE_BALL_ITEM_NAME;
	static const double FIRE_BALL_TIMER_IN_SECS;

	FireBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~FireBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::FireBallItem;
	}

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}
};


#endif // __FIREBALLITEM_H__