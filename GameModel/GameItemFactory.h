/**
 * GameItemFactory.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	static GameItemFactory* GetInstance();
	static void DeleteInstance();

	// Factory functions for the creation of game items
	GameItem* CreateRandomItem(const Point2D &spawnOrigin, const Vector2D& dropDir, 
        GameModel *gameModel, bool allowRandomItemType) const;   // Could be either power-up or down
	GameItem::ItemType CreateRandomItemType(GameModel *gameModel, bool allowRandomItemType) const;
	GameItem::ItemType GetItemTypeFromName(const std::string& itemName) const;
	GameItem* CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) const;

	GameItem* CreateRandomItemForCurrentLevel(const Point2D &spawnOrigin, const Vector2D& dropDir, 
        GameModel *gameModel, bool allowRandomItemType) const;
	GameItem::ItemType CreateRandomItemTypeForCurrentLevel(GameModel *gameModel, bool allowRandomItemType) const;

	GameItem::ItemDisposition GetItemTypeDisposition(const GameItem::ItemType& itemType) const;

	bool IsValidItemTypeName(const std::string& itemName) const;
    bool IsValidItemType(int itemType) const;

	const std::set<GameItem::ItemType>& GetAllItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerUpItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerNeutralItemTypes() const;
	const std::set<GameItem::ItemType>& GetPowerDownItemTypes() const;
	
	const std::map<std::string, GameItem::ItemType>& GetItemNameToTypeMap() const;

private:
	GameItemFactory();
	~GameItemFactory();

	static GameItemFactory* instance;

	std::map<std::string, GameItem::ItemType> itemNameToTypeMap;
	
	std::set<GameItem::ItemType> allItemTypes;
	std::set<GameItem::ItemType> allPowerUpItemTypes;
	std::set<GameItem::ItemType> allPowerNeutralItemTypes;
	std::set<GameItem::ItemType> allPowerDownItemTypes;

    /*
    // Good-luck / Bad-luck information for combinations of items based
    // on the current amount of luck the player has
    struct ComboItemGroups {
        std::vector<GameItem::ItemType> goodLuckItems;  // In order from least good to most good
        std::vector<GameItem::ItemType> badLuckItems;   // In order from least bad to most bad
    };
    std::map<GameItem::ItemType, ComboItemGroups*> itemCombos;
    */

    DISALLOW_COPY_AND_ASSIGN(GameItemFactory);
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

inline bool GameItemFactory::IsValidItemType(int itemType) const {
    return (itemType >= 0 && itemType <= static_cast<int>(GameItem::RandomItem));
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

inline const std::map<std::string, GameItem::ItemType>& GameItemFactory::GetItemNameToTypeMap() const {
	return this->itemNameToTypeMap;
}

#endif