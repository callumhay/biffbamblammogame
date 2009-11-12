/**
 * MultiBallItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MULTIBALLITEM_H__
#define __MULTIBALLITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"

class Point2D;

class MultiBallItem : public GameItem {
public:
	enum NumMultiBalls { ThreeMultiBalls = 3, FiveMultiBalls = 5 };
	
	static const std::string MULTI3_BALL_ITEM_NAME;
	static const std::string MULTI5_BALL_ITEM_NAME;
	
	static const double MULTI_BALL_TIMER_IN_SECS;
	static const double BALL_COLLISIONS_DISABLED_DURATION;

private:
	NumMultiBalls numBalls;
	unsigned int numNewSpawnedBalls;	// The number of balls created for each preexisting gameball in play

	static std::string NameFromNumBalls(NumMultiBalls n) {
		switch (n) {
			case ThreeMultiBalls:
				return MULTI3_BALL_ITEM_NAME;
			case FiveMultiBalls:
				return MULTI5_BALL_ITEM_NAME;
			default:
				assert(false);
				break;
		}
		return MULTI3_BALL_ITEM_NAME;
	}

public:
	static const unsigned int MAX_NUM_SPLITBALLS = 5;		// Maximum number of balls that can split off in a multiball
	static const unsigned int MIN_NUM_SPLITBALLS = 1;		// Minimum number of balls that can split off in a multiball
	static const float MIN_SPLIT_DEGREES;								// Minimum number of degrees between balls when splitting

	MultiBallItem(const Point2D &spawnOrigin, GameModel *gameModel, NumMultiBalls numBalls);
	~MultiBallItem();

	double Activate();
	void Deactivate();

	GameItem::ItemType GetItemType() const {
		return (this->numBalls == ThreeMultiBalls) ? GameItem::MultiBall3Item : GameItem::MultiBall5Item;
	}
};
#endif