#include "GameMenu.h"
#include "TextureFontSet.h"
#include "Camera.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextureFontSet* font, const std::string& text) : font(font), text(text) {
	assert(font != NULL);
}

// Copy constructor
GameMenuItem::GameMenuItem(const GameMenuItem& item) {
	this->font = item.font;
	this->text = item.text;
}

GameMenuItem::~GameMenuItem() {
}

// Return the height of this menu item
unsigned int GameMenuItem::GetHeight() const {
	return this->font->GetHeight();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(const Point2D& topLeftCorner) {
	this->font->OrthoPrint(topLeftCorner, this->text);
}

// GameMenu Functions **********************************************

GameMenu::GameMenu(const Point2D& topLeftCorner) : menuItemPadding(1), topLeftCorner(topLeftCorner) {
}

GameMenu::~GameMenu() {
	// Delete all the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		delete this->menuItems[i];
	}
	this->menuItems.clear();
}

/**
 * Draw this game menu in its current state.
 */
void GameMenu::Draw() {
	Point2D currPos = this->topLeftCorner;
	
	glColor3f(0, 1, 0);

	// Draw each menu item
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		currItem->Draw(currPos);
		currPos = currPos - Vector2D(0, currItem->GetHeight() + this->menuItemPadding);
	}

}

/**
 * Draw debug stuffs for the menu - includes point where the menu
 * is located (top-left corner).
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
	glColor3f(1,1,1);
	glBegin(GL_LINES);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0]+1000, this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1],0.0f);
		glVertex3f(this->topLeftCorner[0], this->topLeftCorner[1]-1000,0.0f);
	glEnd();
	
	glPopMatrix();
	glPopAttrib(); 
	Camera::PopWindowCoords();
}