#include "GameMenu.h"
#include "../BlammoEngine/Camera.h"

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& textLabel) : textLabel(textLabel) {
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

GameMenuItem::~GameMenuItem() {
}

// Return the height of this menu item
unsigned int GameMenuItem::GetHeight() const {
	return this->textLabel.GetHeight();
}

unsigned int GameMenuItem::GetWidth() const {
	return this->textLabel.GetLastRasterWidth();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(double dT, const Point2D& topLeftCorner) {
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();
		
	this->textLabel.SetTopLeftCorner(topLeftCorner + Vector2D(wiggleAmount, 0.0f));
	this->textLabel.Draw();

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

const float GameMenu::UP_DOWN_ARROW_PADDING = 10.0f;

GameMenu::GameMenu(const Point2D& topLeftCorner) : menuItemPadding(1), topLeftCorner(topLeftCorner),
selectedMenuItemIndex(-1) {
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

	const float TOP_ARROW_BOTTOM_Y	= GameMenu::UP_DOWN_ARROW_PADDING + itemPos[1];
	const float TOP_ARROW_TOP_Y			= TOP_ARROW_BOTTOM_Y + ARROW_HEIGHT;
	
	const float BOTTOM_ARROW_TOP_Y		= itemPos[1] - menuItem.GetHeight() - GameMenu::UP_DOWN_ARROW_PADDING;
	const float BOTTOM_ARROW_BOTTOM_Y	= BOTTOM_ARROW_TOP_Y - ARROW_HEIGHT;

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	glColor4f(0, 0, 0, 1);
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
		
		if (i == this->selectedMenuItemIndex) {
			currItem->SetTextColour(this->highlightColour);
			this->DrawUpDownArrows(currPos, *currItem);
		}
		else {
			currItem->SetTextColour(this->idleColour);
		}
		
		currItem->Draw(dT, currPos);
		currPos = currPos - Vector2D(0, currItem->GetHeight() + this->menuItemPadding + 2 * GameMenu::UP_DOWN_ARROW_PADDING);
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