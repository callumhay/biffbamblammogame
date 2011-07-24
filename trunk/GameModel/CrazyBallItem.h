/**
 * CrazyBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CRAZYBALLITEM_H__
#define __CRAZYBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class CrazyBallItem : public GameItem {
public:
	static const char* CRAZY_BALL_ITEM_NAME;
	static const double CRAZY_BALL_TIMER_IN_SECS;

	CrazyBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~CrazyBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::CrazyBallItem;
	}

	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}
};


#endif // __CRAZYBALLITEM_H__