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

BlammopediaState::BlammopediaState(GameDisplay* display) : DisplayState(display) {

	const Camera& camera = this->display->GetCamera();

	this->listViews.reserve(3);
	this->listViews.push_back(this->BuildGameItemsListView());
	this->listViews.push_back(this->BuildGameBlockListView());
	this->listViews.push_back(new ItemListView(0, camera.GetWindowHeight(), camera.GetWindowWidth()));
}

BlammopediaState::~BlammopediaState() {
}

void BlammopediaState::RenderFrame(double dT) {
}

void BlammopediaState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
}

void BlammopediaState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
}

ItemListView* BlammopediaState::BuildGameItemsListView() const {
	ItemListView* itemsListView = new ItemListView(0, camera.GetWindowHeight(), camera.GetWindowWidth());
	
	// Add each item in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
	// TODO...
}

ItemListView* BlammopediaState::BuildGameBlockListView() const {
	ItemListView* blockListView = new ItemListView(0, camera.GetWindowHeight(), camera.GetWindowWidth());


}

ItemListView* BlammopediaState::BuildStatusEffectListView() const {
	ItemListView* statusListView = new ItemListView(0, camera.GetWindowHeight(), camera.GetWindowWidth());



}
