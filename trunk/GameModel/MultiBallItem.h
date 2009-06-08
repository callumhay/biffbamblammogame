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

private:
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
	static const unsigned int MAX_NUM_SPLITBALLS = 9;	// Maximum number of balls that can split off in a multiball
	static const unsigned int MIN_NUM_SPLITBALLS = 1;		// Minimum number of balls that can split off in a multiball
	static const float MIN_SPLIT_DEGREES;								// Minimum number of degrees between balls when splitting

	MultiBallItem(const Point2D &spawnOrigin, GameModel *gameModel, NumMultiBalls numBalls);
	virtual ~MultiBallItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif