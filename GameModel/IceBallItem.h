/**
 * IceBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ICEBALLITEM_H__
#define __ICEBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameModel.h"
#include "GameItem.h"

class Point2D;

class IceBallItem : public GameItem {
public:
	static const char* ICE_BALL_ITEM_NAME;
	static const double ICE_BALL_TIMER_IN_SECS;

	IceBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~IceBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::IceBallItem;
	}

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}
};


#endif // __ICEBALLITEM_H__