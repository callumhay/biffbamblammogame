/**
 * IceBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(IceBallItem);
};


#endif // __ICEBALLITEM_H__