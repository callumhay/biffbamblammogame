/**
 * GravityBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GRAVITYBALLITEM_H__
#define __GRAVITYBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class Point2D;

class GravityBallItem : public GameItem {
public:
	static const char* GRAVITY_BALL_ITEM_NAME;
	static const double GRAVITY_BALL_TIMER_IN_SECS;

	GravityBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~GravityBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::GravityBallItem;
	}

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}
};
#endif