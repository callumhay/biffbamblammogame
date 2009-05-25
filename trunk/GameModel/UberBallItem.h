#ifndef __UBERBALLITEM_H__
#define __UBERBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"

#include "GameItem.h"
#include "GameModel.h"

class UberBallItem : public GameItem {
public:
	static const std::string UBER_BALL_ITEM_NAME;
	static const double UBER_BALL_TIMER_IN_SECS;

	UberBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~UberBallItem();

	virtual const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	virtual double Activate();
	virtual void Deactivate();
};
#endif