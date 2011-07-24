/**
 * RandomItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RandomItem.h"
#include "GameItemFactory.h"

const char*  RandomItem::RANDOM_ITEM_NAME = "Random";

RandomItem::RandomItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(RandomItem::RANDOM_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral), randomItem(NULL) {

	// Build the random item...
	this->randomItem = GameItemFactory::GetInstance()->CreateRandomItemForCurrentLevel(spawnOrigin, gameModel, false);
	assert(this->randomItem != NULL);
}

RandomItem::~RandomItem() {
	delete this->randomItem;
	this->randomItem = NULL;
}
	



