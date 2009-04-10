#include "GameMenu.h"
#include "../BlammoEngine/Camera.h"

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& textLabel) : textLabel(textLabel) {
}

GameMenuItem::~GameMenuItem() {
}

// Return the height of this menu item
unsigned int GameMenuItem::GetHeight() const {
	return this->textLabel.GetHeight();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(const Point2D& topLeftCorner) {
	this->textLabel.SetTopLeftCorner(topLeftCorner);
	this->textLabel.Draw();
}

// GameMenu Functions **********************************************

GameMenu::GameMenu(const Point2D& topLeftCorner) : menuItemPadding(1), topLeftCorner(topLeftCorner),
highlightedMenuItemIndex(-1) {
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
 * Draw this game menu in its current state.
 */
void GameMenu::Draw() {
	Point2D currPos = this->topLeftCorner;

	// Draw the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		
		if (i == this->highlightedMenuItemIndex) {
			currItem->SetTextColour(this->highlightColour);
		}
		else {
			currItem->SetTextColour(this->idleColour);
		}
		
		currItem->Draw(currPos);
		currPos = currPos - Vector2D(0, currItem->GetHeight() + this->menuItemPadding);
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