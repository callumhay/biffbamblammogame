/**
 * UberBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::UberBallItem;
	}

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(UberBallItem);
};
#endif