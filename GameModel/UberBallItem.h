/**
 * UberBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __UBERBALLITEM_H__
#define __UBERBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"

#include "GameItem.h"
#include "GameModel.h"

class UberBallItem : public GameItem {
public:
	static const char*  UBER_BALL_ITEM_NAME;
	static const double UBER_BALL_TIMER_IN_SECS;

	UberBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~UberBallItem();

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::UberBallItem;
	}
};
#endif