/**
 * PaddleSizeItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLESIZEITEM_H__
#define __PADDLESIZEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

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
	~PaddleSizeItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return (this->sizeChangeType == ShrinkPaddle) ? GameItem::PaddleShrinkItem : GameItem::PaddleGrowItem;
	}
};
#endif