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

public:
	static const unsigned int TOTAL_NUM_OF_ITEMS = 25;
	
	static GameItemFactory* GetInstance();
	static void DeleteInstance();

	// Factory functions for the creation of game items
	GameItem* CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) const;			// Could be either power-up or down
	GameItem::ItemType CreateRandomItemType(GameModel *gameModel) const;
	GameItem::ItemType GetItemTypeFromName(const std::string& itemName) const;
	GameItem* CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, GameModel *gameModel) const;

	bool IsValidItemTypeName(const std::string& itemName) const;

	const std::set<GameItem::ItemType>& GetAllItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerUpItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerNeutralItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerDownItemTypes() const;

private:
	GameItemFactory();
	~GameItemFactory();

	static GameItemFactory* instance;

	std::map<std::string, GameItem::ItemType> itemNameToTypeMap;
	
	std::set<GameItem::ItemType> allItemTypes;
	std::set<GameItem::ItemType> allPowerUpItemTypes;
	std::set<GameItem::ItemType> allPowerNeutralItemTypes;
	std::set<GameItem::ItemType> allPowerDownItemTypes;

};

inline GameItemFactory* GameItemFactory::GetInstance() {
	if (GameItemFactory::instance == NULL) {
		GameItemFactory::instance = new GameItemFactory();
	}
	return GameItemFactory::instance;
}

inline void GameItemFactory::DeleteInstance() {
	if (GameItemFactory::instance != NULL) {
		delete GameItemFactory::instance;
		GameItemFactory::instance = NULL;
	}
}

inline bool GameItemFactory::IsValidItemTypeName(const std::string& itemName) const {
	std::map<std::string, GameItem::ItemType>::const_iterator findIter = this->itemNameToTypeMap.find(itemName.c_str());
	return (findIter != this->itemNameToTypeMap.end());
}

inline const std::set<GameItem::ItemType>& GameItemFactory::GetAllItemTypes() const {
	return this->allItemTypes;
}

inline const std::set<GameItem::ItemType>& GameItemFactory::GetPowerUpItemTypes() const {
	return this->allPowerUpItemTypes;
}

inline const std::set<GameItem::ItemType>& GameItemFactory::GetPowerNeutralItemTypes() const {
	return this->allPowerNeutralItemTypes;
}

inline const std::set<GameItem::ItemType>& GameItemFactory::GetPowerDownItemTypes() const {
	return this->allPowerDownItemTypes;
}

#endif