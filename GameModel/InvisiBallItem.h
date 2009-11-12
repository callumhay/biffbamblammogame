#ifndef __INVISIBALLITEM_H__
#define __INVISIBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

class Point2D;

class InvisiBallItem : public GameItem {
public:
	static const std::string INVISI_BALL_ITEM_NAME;
	static const double INVISI_BALL_TIMER_IN_SECS;

	InvisiBallItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~InvisiBallItem();
	
	const GameBall* GetBallAffected() const {
		assert(this->gameModel->GetGameBalls().size() > 0);
		return *this->gameModel->GetGameBalls().begin();
	}

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::InvisiBallItem;
	}
};
#endif