#ifndef __GAMEITEMFACTORY_H__
#define __GAMEITEMFACTORY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "GameItem.h"

class GameModel;
class Point2D;

/**
 * Class that follows the factory design pattern - used to hide the creation
 * of GameItems that alter the gameplay for the player when they 'catch' them
 * with the paddle. GameItems themselves only need be activated, their implementation
 * is not significant to the caller.
 */
class GameItemFactory {

private:
	GameItemFactory() {};

public:
	static const unsigned int TOTAL_NUM_OF_ITEMS = 21;
	~GameItemFactory() {};

	// Static factory functions for the creation of game items
	static GameItem* CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel);			// Could be either power-up or down
	
#ifdef _DEBUG
	// Item specific creation methods for debugging
	static GameItem* CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, GameModel *gameModel);
#endif
};
#endif