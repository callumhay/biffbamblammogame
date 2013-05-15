/**
 * CrazyBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    std::set<const GameBall*> GetBallsAffected() const {
	    std::set<const GameBall*> ballsAffected;
        ballsAffected.insert(this->gameModel->GetGameBalls().begin(), this->gameModel->GetGameBalls().end());
        return ballsAffected;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(CrazyBallItem);
};


#endif // __CRAZYBALLITEM_H__