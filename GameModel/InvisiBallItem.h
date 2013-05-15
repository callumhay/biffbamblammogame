/**
 * InvisiBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __INVISIBALLITEM_H__
#define __INVISIBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class Point2D;

class InvisiBallItem : public GameItem {
public:
	static const char* INVISI_BALL_ITEM_NAME;
	static const double INVISI_BALL_TIMER_IN_SECS;

	InvisiBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~InvisiBallItem();
	
	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::InvisiBallItem;
	}

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(InvisiBallItem);
};
#endif