/**
 * PaddleMineLauncherItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEMINELAUNCHERITEM_H__
#define __PADDLEMINELAUNCHERITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

/**
 * Represents the power-up item for the paddle's rocket bullet firing ability -
 * the paddle gains the ability to fire a single rocket up at the level that destroys
 * a whole bunch of blocks.
 */
class PaddleMineLauncherItem : public GameItem {
public:
	static const char* MINE_LAUNCHER_PADDLE_ITEM_NAME;
	static const double MINE_LAUNCHER_PADDLE_TIMER_IN_SECS;

	PaddleMineLauncherItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~PaddleMineLauncherItem ();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::MineLauncherPaddleItem;
	}
};
#endif // __PADDLEMINELAUNCHERITEM_H__