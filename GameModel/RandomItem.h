/**
 * RandomItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
	
	const GameBall* GetBallAffected() const;
	double Activate();
	void Deactivate();
	GameItem::ItemType GetItemType() const;

private:
	GameItem* randomItem;

};

inline const GameBall* RandomItem::GetBallAffected() const {
	return this->randomItem->GetBallAffected();
}

inline double RandomItem::Activate() {
	this->isActive = true;
	this->SetItemDisposition(this->randomItem->GetItemDisposition());
	this->SetItemName(this->randomItem->GetName());
	GameEventManager::Instance()->ActionRandomItemActivated(*this, *this->randomItem);
	return this->randomItem->Activate();
}

inline void RandomItem::Deactivate() {
	this->randomItem->Deactivate();
}

inline GameItem::ItemType RandomItem::GetItemType() const {
	if (this->isActive) {
		return this->randomItem->GetItemType();
	}
	return GameItem::RandomItem;
}

#endif