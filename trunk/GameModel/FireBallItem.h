/**
 * FireBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(FireBallItem);
};


#endif // __FIREBALLITEM_H__