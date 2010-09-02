/**
 * GameItemFactory.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	static const unsigned int TOTAL_NUM_OF_ITEMS = 25;
	~GameItemFactory() {};

	// Static factory functions for the creation of game items
	static GameItem* CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel);			// Could be either power-up or down
	
#ifdef _DEBUG
	// Item specific creation methods for debugging
	static GameItem* CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, GameModel *gameModel);
#endif
};
#endif