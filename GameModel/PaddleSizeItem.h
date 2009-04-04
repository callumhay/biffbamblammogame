#ifndef __PADDLESIZEITEM_H__
#define __PADDLESIZEITEM_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameItem.h"
#include "GameModel.h"

class PaddleSizeItem : public GameItem {
public:
	enum PaddleSizeChangeType { ShrinkPaddle, GrowPaddle };

private:
	PaddleSizeChangeType sizeChangeType;

public:
	static const std::string PADDLE_GROW_ITEM_NAME;
	static const std::string PADDLE_SHRINK_ITEM_NAME;
	static const double PADDLE_SIZE_TIMER_IN_SECS;

	PaddleSizeItem(const PaddleSizeChangeType type, const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~PaddleSizeItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif