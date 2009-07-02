/**
 * GameMenuItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameMenuItem.h"
#include "GameMenu.h"

const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE	= 8.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_SMALL	= 4.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_FREQ				= 0.2f;
const float GameMenuItem::SUB_MENU_PADDING						= 15.0f;

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameSubMenu* subMenu) : 
smTextLabel(smLabel), lgTextLabel(lgLabel), subMenu(subMenu) {
	
	assert(smLabel.GetText() == lgLabel.GetText());

	this->currLabel = &this->smTextLabel;
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

GameMenuItem::~GameMenuItem() {
}

// Return the height of this menu item
unsigned int GameMenuItem::GetHeight() const {
	return this->currLabel->GetHeight();
}

unsigned int GameMenuItem::GetWidth() const {
	return this->currLabel->GetLastRasterWidth();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(double dT, const Point2D& topLeftCorner) {
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();

	this->currLabel->SetTopLeftCorner(topLeftCorner + Vector2D(wiggleAmount, 0.0f));
	this->currLabel->Draw();

	// Animate the wiggle and/or pulse (if there are any animations loaded)
	this->wiggleAnimation.Tick(dT);

	if (subMenu != NULL) {
		const float PAD_AMT = GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE + GameMenuItem::SUB_MENU_PADDING + GameSubMenu::BACKGROUND_PADDING + 2*GameSubMenu::HALF_ARROW_WIDTH;
		subMenu->SetTopLeftCorner(topLeftCorner + Vector2D(this->lgTextLabel.GetLastRasterWidth() + PAD_AMT, 0.0f));
	}
}

/** 
 * Toggles the wiggle animation for this menu item on with the
 * given amplitude and frequency of wiggle given.
 */
void GameMenuItem::ToggleWiggleAnimationOn(float amplitude, float frequency) {

	std::vector<float> wiggleValues;
	wiggleValues.reserve(5);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(amplitude);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(-amplitude);
	wiggleValues.push_back(0.0f);

	std::vector<double> timeValues;
	timeValues.reserve(5);
	timeValues.push_back(0.0);
	timeValues.push_back(frequency);
	timeValues.push_back(2*frequency);
	timeValues.push_back(3*frequency);
	timeValues.push_back(4*frequency);

	this->wiggleAnimation.SetLerp(timeValues, wiggleValues);
	this->wiggleAnimation.SetRepeat(true);
}

void GameMenuItem::ToggleWiggleAnimationOff() {
	this->wiggleAnimation.ClearLerp();
	this->wiggleAnimation.SetRepeat(false);
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

// SelectionListMenuItem Functions ***************************************************************************

SelectionListMenuItem::SelectionListMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, const std::vector<std::string>& items) :
GameMenuItem(smLabel, lgLabel, NULL), selectionList(items), selectedIndex(SelectionListMenuItem::NO_SELECTION),
baseLabelStr(lgLabel.GetText()) {

	this->SetSelectionList(items);
}

SelectionListMenuItem::~SelectionListMenuItem() {
}

/**
 * Sets the list of selectable items in this menu item.
 */
void SelectionListMenuItem::SetSelectionList(const std::vector<std::string>& items) {
	if (items.size() > 0 && this->selectedIndex == SelectionListMenuItem::NO_SELECTION) {
		this->selectedIndex = 0;
	}
	this->selectionList = items;
}

void SelectionListMenuItem::Draw(double dT, const Point2D& topLeftCorner) {
	// TODO
}

void SelectionListMenuItem::KeyPressed(SDLKey key) {
	switch (key) {
		case SDLK_LEFT:
			// TODO
			break;

		case SDLK_RIGHT:
			// TODO
			break;

		default:
			break;
	}
}