/**
 * BlammopediaState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BlammopediaState.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "ItemListView.h"

#include "../ResourceManager.h"
#include "../GameModel/GameItemFactory.h"

BlammopediaState::BlammopediaState(GameDisplay* display) : 
DisplayState(display), lockedEntryTexture(NULL) {

	this->lockedEntryTexture = ResourceManager::GetInstance()->GetImgTextureResource(
		GameViewConstants::GetInstance()->TEXTURE_LOCKED_BLAMMOPEDIA_ENTRY, Texture::Trilinear, GL_TEXTURE_2D);

	this->listViews.reserve(3);
	this->listViews.push_back(this->BuildGameItemsListView());
	this->listViews.push_back(this->BuildGameBlockListView());
	this->listViews.push_back(this->BuildStatusEffectListView());
}

BlammopediaState::~BlammopediaState() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lockedEntryTexture);
	assert(success);

	for (std::vector<ItemListView*>::iterator iter = this->listViews.begin(); iter != this->listViews.end(); ++iter) {
		ItemListView* currListView = *iter;
		delete currListView;
		currListView = NULL;
	}
	this->listViews.clear();
}

void BlammopediaState::RenderFrame(double dT) {
	UNUSED_PARAMETER(dT);
}

void BlammopediaState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
}

void BlammopediaState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

ItemListView* BlammopediaState::BuildGameItemsListView() const {
	assert(this->lockedEntryTexture != NULL);

	const Camera& camera = this->display->GetCamera();
	ItemListView* itemsListView = new ItemListView(camera.GetWindowWidth());
	
	// Add each item in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
	const std::set<GameItem::ItemType>& itemTypes = GameItemFactory::GetInstance()->GetAllItemTypes();
	const std::map<GameItem::ItemType, std::string>& itemTextures = GameViewConstants::GetInstance()->GetItemTextures();

	for (std::set<GameItem::ItemType>::const_iterator iter = itemTypes.begin(); iter != itemTypes.end(); ++iter) {
		const GameItem::ItemType& currItemType = *iter;
		std::map<GameItem::ItemType, std::string>::const_iterator findIter = itemTextures.find(currItemType);
		assert(findIter != itemTextures.end());

		// Grab the item texture, name and description
		const Texture* itemTexture = ResourceManager::GetInstance()->GetImgTextureResource(findIter->second, Texture::Trilinear, GL_TEXTURE_2D);
		assert(itemTexture != NULL);
		// Name and description come from the item resource text file...
		//ItemBlammopediaEntry* itemData = ResourceManager::GetBlammopediaEntryForItem(currItemType);


	}

	return itemsListView;
}

ItemListView* BlammopediaState::BuildGameBlockListView() const {
	assert(this->lockedEntryTexture != NULL);

	const Camera& camera = this->display->GetCamera();
	ItemListView* blockListView = new ItemListView(camera.GetWindowWidth());


	assert(false);
	return blockListView;
}

ItemListView* BlammopediaState::BuildStatusEffectListView() const {
	assert(this->lockedEntryTexture != NULL);

	const Camera& camera = this->display->GetCamera();
	ItemListView* statusListView = new ItemListView(camera.GetWindowWidth());


	assert(false);
	return statusListView;
}
