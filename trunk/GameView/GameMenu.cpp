#include "GameMenu.h"
#include "../BlammoEngine/Camera.h"

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu) : 
smTextLabel(smLabel), lgTextLabel(lgLabel), subMenu(subMenu) {
	
	this->currLabel = &this->smTextLabel;
	this->wiggleAnimation.SetInterpolantValue(0.0f);

	// Set up the characteristics of the submenu based on this item
	if (subMenu != NULL) {
		subMenu->SetTopLeftCorner(lgLabel.GetTopLeftCorner() + Vector2D(lgLabel.GetLastRasterWidth(), 0.0f));
	}
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

	// Animate the wiggle (if there is any animation loaded)
	this->wiggleAnimation.Tick(dT);
}

/** 
 * Toggles the wiggle animation for this menu item on with the
 * given amplitude and frequency of wiggle given.
 */
void GameMenuItem::ToggleWiggleAnimationOn(float amplitude, float freqency) {
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
	timeValues.push_back(freqency);
	timeValues.push_back(2*freqency);
	timeValues.push_back(3*freqency);
	timeValues.push_back(4*freqency);

	this->wiggleAnimation.SetLerp(timeValues, wiggleValues);
	this->wiggleAnimation.SetRepeat(true);
}

void GameMenuItem::ToggleWiggleAnimationOff() {
	this->wiggleAnimation.ClearLerp();
	this->wiggleAnimation.SetRepeat(false);
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

// GameMenu Functions **********************************************

const float GameMenu::UP_DOWN_ARROW_TOP_PADDING		 = 8.0f;
const float GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING = 16.0f;

GameMenu::GameMenu(const Point2D& topLeftCorner) : menuItemPadding(1), topLeftCorner(topLeftCorner),
selectedMenuItemIndex(-1), isSelectedItemActivated(false) {
}

GameMenu::~GameMenu() {
	// Delete all the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		delete this->menuItems[i];
		this->menuItems[i] = NULL;
	}
	this->menuItems.clear();
}

/**
 * Draw up and down arrows around a menu item at the given position.
 */
void GameMenu::DrawUpDownArrows(const Point2D& itemPos, const GameMenuItem& menuItem) {
	const float ARROW_HEIGHT = 15;
	const float ARROW_WIDTH  = 50;

	const float ARROW_X = itemPos[0] + (menuItem.GetWidth() - ARROW_WIDTH) / 2.0f;

	const float TOP_ARROW_BOTTOM_Y	= GameMenu::UP_DOWN_ARROW_TOP_PADDING + itemPos[1];
	const float TOP_ARROW_TOP_Y			= TOP_ARROW_BOTTOM_Y + ARROW_HEIGHT;
	
	const float BOTTOM_ARROW_TOP_Y		= itemPos[1] - menuItem.GetHeight() - GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING;
	const float BOTTOM_ARROW_BOTTOM_Y	= BOTTOM_ARROW_TOP_Y - ARROW_HEIGHT;

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Draw the outlines of the arrows
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	
	// Top arrow
	glVertex2f(ARROW_X, TOP_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH, TOP_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH / 2.0f, TOP_ARROW_TOP_Y);

	// Bottom arrow
	glVertex2f(ARROW_X + ARROW_WIDTH / 2.0f, BOTTOM_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH, BOTTOM_ARROW_TOP_Y);
	glVertex2f(ARROW_X, BOTTOM_ARROW_TOP_Y);

	glEnd();

	// Fill in the arrows
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	
	// Top arrow
	glVertex2f(ARROW_X, TOP_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH, TOP_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH / 2.0f, TOP_ARROW_TOP_Y);

	// Bottom arrow
	glVertex2f(ARROW_X + ARROW_WIDTH / 2.0f, BOTTOM_ARROW_BOTTOM_Y);
	glVertex2f(ARROW_X + ARROW_WIDTH, BOTTOM_ARROW_TOP_Y);
	glVertex2f(ARROW_X, BOTTOM_ARROW_TOP_Y);

	glEnd();


	Camera::PopWindowCoords();
}

/**
 * Draw this game menu in its current state.
 */
void GameMenu::Draw(double dT) {
	Point2D currPos = this->topLeftCorner;

	// Draw the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		
		// Check to see if we are iterating on the selected/highlighted menu item
		if (i == this->selectedMenuItemIndex) {

			// Check to see if the highlighted item is activated
			if (this->isSelectedItemActivated) {
				// Since the item is activated we need to check for a submenu,
				// if the item has a submenu then we draw it (this can lead to a nest of menu/submenus)
				GameMenu* subMenu = currItem->GetSubMenu();
				if (subMenu != NULL) {
					subMenu->Draw(dT);
				}
			}
			else {
				// The item has not been activated
				this->DrawUpDownArrows(currPos, *currItem);
			}
		}
		else {
			currItem->SetTextColour(this->idleColour);
		}
		
		currItem->Draw(dT, currPos);
		currPos = currPos - Vector2D(0, currItem->GetHeight() + this->menuItemPadding + GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING + GameMenu::UP_DOWN_ARROW_TOP_PADDING);
	}

}

/**
 * Sets the currently highlighted/selected menu item in this
 * game menu. This will change its properties to make it obvious that
 * it has been highlighted.
 */
void GameMenu::SetSelectedMenuItem(int index) {
	assert(index >= 0 && index < static_cast<int>(this->menuItems.size()));
	
	// Deselect the previous selection if necessary
	if (this->selectedMenuItemIndex >= 0 && this->selectedMenuItemIndex < static_cast<int>(this->menuItems.size())) {
		this->menuItems[this->selectedMenuItemIndex]->ToggleWiggleAnimationOff();
		this->menuItems[this->selectedMenuItemIndex]->SetSize(false);
	}

	// Select the new selection
	this->selectedMenuItemIndex = index;
	this->menuItems[index]->ToggleWiggleAnimationOn(5.0f, 0.10f);
	this->menuItems[index]->SetSize(true);
	this->menuItems[index]->SetTextColour(this->highlightColour);
}

/**
 * Activate the currently selected menu item. This will cause a submenu (if it has one)
 * to appear and change the properties of the previous menu to reflect the change.
 */
void GameMenu::ActivateSelectedMenuItem() {
	this->isSelectedItemActivated = true;
	this->menuItems[this->selectedMenuItemIndex]->SetTextColour(this->activateColour);
	
	// Go through all the menu items except the selected one and grey them out
	for (size_t i = 0; i < this->menuItems.size(); i++) {	
		if (i == this->selectedMenuItemIndex) { continue; }
		this->menuItems[i]->SetTextColour(this->greyedOutColour);
	}
}

/**
 * Capture input from the keyboard for a key pressed event
 * and navigate or change the menu based on the given key input.
 */
void GameMenu::KeyPressed(SDLKey key) {

	// Handle the input by changing the menu according to the key pressed
	switch(key) {
		case SDLK_DOWN:
			this->DownAction();
			break;

		case SDLK_UP:
			this->UpAction();
			break;
	
		default:
			break;
	}
}

/**
 * Draw debug stuffs for the menu - includes point where the menu
 * is located (top-left corner crosshair).
 */
void GameMenu::DebugDraw() {
	Camera::PushWindowCoords();
	
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	glLoadIdentity();

	glLineWidth(1.0f);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0]+1000, this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1]-1000,0.0f);
	glEnd();
	
	glPopMatrix();
	glPopAttrib(); 
	Camera::PopWindowCoords();

	debug_opengl_state();
}