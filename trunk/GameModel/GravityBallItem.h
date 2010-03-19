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