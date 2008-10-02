#ifndef __GAMEITEMFACTORY_H__
#define __GAMEITEMFACTORY_H__

#include <vector>

class GameItem;
class GameModel;

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
	~GameItemFactory() {};

	// Static factory functions for the creation of game items
	static GameItem* CreateRandomItem(GameModel *gameModel);			// Could be either power-up or down

};
#endif