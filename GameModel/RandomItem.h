/**
 * RandomItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __RANDOMITEM_H__
#define __RANDOMITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"
#include "GameEventManager.h"

// A random item can be any sort of other item in the game - It wraps up the random item in itself so
// that it drops as an entirely separate type of item.
class RandomItem : public GameItem {
public:
	static const char* RANDOM_ITEM_NAME;

	RandomItem(const Point2D &spawnOrigin, GameModel *gameModel);
	~RandomItem();
	
	std::set<const GameBall*> GetBallsAffected() const ;
	double Activate();
	void Deactivate();
	GameItem::ItemType GetItemType() const;

    GameItem::ItemType GetBlinkingRandomItemType() const;

private:
    static const long TIME_BETWEEN_BLINKS_IN_MS;

	GameItem* randomItem;
    std::vector<GameItem::ItemType> possibleItemDropTypes;
    
    mutable int currRandomIdx;
    mutable unsigned long lastBlinkTime;

    DISALLOW_COPY_AND_ASSIGN(RandomItem);
};

inline std::set<const GameBall*> RandomItem::GetBallsAffected() const {
	return this->randomItem->GetBallsAffected();
}

inline void RandomItem::Deactivate() {
	this->randomItem->Deactivate();
}

#endif