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
DisplayState(display) {

	this->listViews.reserve(3);
	this->listViews.push_back(this->BuildGameItemsListView());
	this->listViews.push_back(this->BuildGameBlockListView());
	this->listViews.push_back(this->BuildStatusEffectListView());
}

BlammopediaState::~BlammopediaState() {
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
	const Camera& camera = this->display->GetCamera();
	ItemListView* itemsListView = new ItemListView(camera.GetWindowWidth());
	
	// Add each item in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
	// TODO

	return itemsListView;
}

ItemListView* BlammopediaState::BuildGameBlockListView() const {
	const Camera& camera = this->display->GetCamera();
	ItemListView* blockListView = new ItemListView(camera.GetWindowWidth());


	assert(false);
	return blockListView;
}

ItemListView* BlammopediaState::BuildStatusEffectListView() const {
	const Camera& camera = this->display->GetCamera();
	ItemListView* statusListView = new ItemListView(camera.GetWindowWidth());


	assert(false);
	return statusListView;
}
