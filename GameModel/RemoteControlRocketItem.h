/**
 * RemoteControlRocketItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __REMOTECONTROLROCKETITEM_H__
#define __REMOTECONTROLROCKETITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

/**
 * Represents the power-up item for the paddle's rocket bullet firing ability -
 * the paddle gains the ability to fire a single rocket up at the level that destroys
 * a whole bunch of blocks.
 */
class RemoteControlRocketItem : public GameItem {
public:
	static const char* REMOTE_CONTROL_ROCKET_ITEM_NAME;
	static const double REMOTE_CONTROL_ROCKET_TIMER_IN_SECS;

	RemoteControlRocketItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~RemoteControlRocketItem ();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return GameItem::RemoteCtrlRocketItem;
	}

private:
    DISALLOW_COPY_AND_ASSIGN(RemoteControlRocketItem);
};

#endif // __REMOTECONTROLROCKETITEM_H__