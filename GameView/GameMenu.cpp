/**
 * GameMenu.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameMenu.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"

#include "../ResourceManager.h"

const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE	= 8.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_SMALL	= 4.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_FREQ				= 0.1f;
const float GameMenuItem::SUB_MENU_PADDING						= 15.0f;

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu) : 
smTextLabel(smLabel), lgTextLabel(lgLabel), subMenu(subMenu) {
	
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

// GameMenu Functions **********************************************

const float GameMenu::UP_DOWN_ARROW_TOP_PADDING		 = 8.0f;
const float GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING = 16.0f;

GameMenu::GameMenu() : menuItemPadding(1), topLeftCorner(Point2D(0,0)),
selectedMenuItemIndex(-1), isSelectedItemActivated(false) {
}

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
 * Determine the amount of padding between menu items when drawing
 * this menu.
 */
float GameMenu::GetMenuItemPadding() const {
	return this->menuItemPadding + GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING + GameMenu::UP_DOWN_ARROW_TOP_PADDING;
}

/**
 * Draw up and down arrows around a menu item at the given position.
 */
void GameMenu::DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem) {
	const float ARROW_HEIGHT = 12;
	const float ARROW_WIDTH  = 40;

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
	GameMenu* subMenu = NULL;

	// Draw the background of the menu
	this->DrawMenuBackground();

	// Draw the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		
		// Check to see if we are iterating on the selected/highlighted menu item
		if (i == this->selectedMenuItemIndex) {

			// Check to see if the highlighted item is activated
			if (this->isSelectedItemActivated) {
				// Since the item is activated we need to check for a submenu.
				assert(subMenu == NULL);	// Only one submenu can ever be active at a time
				subMenu = currItem->GetSubMenu();
			}
			else {
				// The item has not been activated
				this->DrawSelectionIndicator(dT, currPos, *currItem);
			}
		}
		else {
			currItem->SetTextColour(this->idleColour);
		}
		
		currItem->Draw(dT, currPos);
		currPos = currPos - Vector2D(0, currItem->GetHeight() + this->GetMenuItemPadding());
	}
	
	// If there was a submenu then we draw it (this can lead to a nest of menu/submenus)
	if (subMenu != NULL) {
		subMenu->Draw(dT);
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
	this->menuItems[index]->ToggleWiggleAnimationOn(GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE, GameMenuItem::MENU_ITEM_WOBBLE_FREQ);
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
	this->menuItems[this->selectedMenuItemIndex]->ToggleWiggleAnimationOff();
	
	// Go through all the menu items except the selected one and grey them out
	for (size_t i = 0; i < this->menuItems.size(); i++) {	
		if (i == this->selectedMenuItemIndex) { continue; }
		this->menuItems[i]->SetTextColour(this->greyedOutColour);
	}

	// An item is being activated - tell the event handlers about this
	for (std::list<GameMenuEventHandler*>::iterator iter = this->eventHandlers.begin(); iter != this->eventHandlers.end(); ++iter) {
		(*iter)->GameMenuItemActivatedEvent(this->selectedMenuItemIndex);
	}
}

void GameMenu::DeactivateSelectedMenuItem() {
	this->isSelectedItemActivated = false;
	this->SetSelectedMenuItem(this->selectedMenuItemIndex);

	// Go through all the menu items except the selected one and set their idle colours
	for (size_t i = 0; i < this->menuItems.size(); i++) {	
		if (i == this->selectedMenuItemIndex) { continue; }
		this->menuItems[i]->SetTextColour(this->idleColour);
	}
}

/**
 * Capture input from the keyboard for a key pressed event
 * and navigate or change the menu based on the given key input.
 */
void GameMenu::KeyPressed(SDLKey key) {
	// We check to see if we're in a sub menu - so that we know where
	// to pass the key pressed event
	GameMenu* currentMenu = this;
	if (this->isSelectedItemActivated) {
		currentMenu = this->menuItems[this->selectedMenuItemIndex]->GetSubMenu();
		if (currentMenu == NULL) {
			currentMenu = this;
		}
	}
	
	// Handle the input by changing the menu according to the key pressed
	switch(key) {
		
		case SDLK_DOWN:
			currentMenu->DownAction();
			break;

		case SDLK_UP:
			currentMenu->UpAction();
			break;
	
		case SDLK_RETURN: 
			currentMenu->ActivateSelectedMenuItem();
			break;

		case SDLK_ESCAPE:
			for (std::list<GameMenuEventHandler*>::iterator iter = currentMenu->eventHandlers.begin(); iter != currentMenu->eventHandlers.end(); ++iter) {
				(*iter)->EscMenu();
			}
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

// GameSubMenu Functions *************************************************************

const float GameSubMenu::HALF_ARROW_WIDTH		= 20.0f;
const float GameSubMenu::BACKGROUND_PADDING = 10.0f;

const double GameSubMenu::ARROW_ANIM_FREQ		= 0.2;															// Frequency of arrow bouncing and squishing
const float GameSubMenu::ARROW_BOUNCE_AMT		= GameSubMenu::BACKGROUND_PADDING;	// How far the arrow deviates when bouncing
const float GameSubMenu::ARROW_SQUISH_AMT		= 0.60f;														// How much the arrow will smoosh

GameSubMenu::GameSubMenu() : GameMenu(), arrowTex(NULL), menuWidth(0.0f), menuHeight(0.0f) {

	// Obtain the arrow texture briefly from the texture manager
	this->arrowTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_UP_ARROW, Texture::Bilinear, GL_TEXTURE_2D);
	assert(this->arrowTex != NULL);

	// Setup the arrow animations
	std::vector<double> arrowAnimTimes;
	arrowAnimTimes.reserve(5);
	arrowAnimTimes.push_back(0.0);
	arrowAnimTimes.push_back(GameSubMenu::ARROW_ANIM_FREQ);
	arrowAnimTimes.push_back(2*GameSubMenu::ARROW_ANIM_FREQ);
	arrowAnimTimes.push_back(3*GameSubMenu::ARROW_ANIM_FREQ);
	arrowAnimTimes.push_back(4*GameSubMenu::ARROW_ANIM_FREQ);

	std::vector<float> arrowBounceAmts;
	arrowBounceAmts.reserve(5);
	arrowBounceAmts.push_back(0.0f);
	arrowBounceAmts.push_back(GameSubMenu::ARROW_BOUNCE_AMT);
	arrowBounceAmts.push_back(0.0f);
	arrowBounceAmts.push_back(-GameSubMenu::ARROW_BOUNCE_AMT);
	arrowBounceAmts.push_back(0.0f);

	std::vector<float> arrowSquishAmts;
	arrowSquishAmts.reserve(5);
	arrowSquishAmts.push_back(1.0f);
	arrowSquishAmts.push_back(GameSubMenu::ARROW_SQUISH_AMT);
	arrowSquishAmts.push_back(1.0f);
	arrowSquishAmts.push_back(1.0f);
	arrowSquishAmts.push_back(1.0f);	

	this->arrowBounceAnim.SetLerp(arrowAnimTimes, arrowBounceAmts);
	this->arrowSquishAnim.SetLerp(arrowAnimTimes, arrowSquishAmts);

	this->arrowBounceAnim.SetRepeat(true);
	this->arrowSquishAnim.SetRepeat(true);
}

GameSubMenu::~GameSubMenu() {
	// Release the arrow texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->arrowTex);
	assert(success);
}

/**
 * Determine the amount of padding between menu items when drawing
 * this submenu.
 */
float GameSubMenu::GetMenuItemPadding() const {
	return this->menuItemPadding;
}

/**
 * Draw an outline around the menu with a colourful plain background.
 */
void GameSubMenu::DrawMenuBackground() {
	const Point2D bgTopLeftCorner = this->topLeftCorner + Vector2D(-BACKGROUND_PADDING, BACKGROUND_PADDING);

	const float currMenuWidth = this->menuWidth + 2 * BACKGROUND_PADDING + 2 * GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE;
	const float currMenuHeight = this->menuHeight + 2 * BACKGROUND_PADDING;

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Draw the outline of the background
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(3.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS);
		glVertex3f(bgTopLeftCorner[0], bgTopLeftCorner[1], -0.5f);
		glVertex3f(bgTopLeftCorner[0], bgTopLeftCorner[1] - currMenuHeight, -0.5f);
		glVertex3f(bgTopLeftCorner[0] + currMenuWidth, bgTopLeftCorner[1] - currMenuHeight, -0.5f);
		glVertex3f(bgTopLeftCorner[0] + currMenuWidth, bgTopLeftCorner[1], -0.5f);
	glEnd();

	// Fill in the background
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, 1.0f); // TODO: pick another colour...

	glBegin(GL_QUADS);
		glVertex3f(bgTopLeftCorner[0], bgTopLeftCorner[1], -0.5f);
		glVertex3f(bgTopLeftCorner[0], bgTopLeftCorner[1] - currMenuHeight, -0.5f);
		glVertex3f(bgTopLeftCorner[0] + currMenuWidth, bgTopLeftCorner[1] - currMenuHeight, -0.5f);
		glVertex3f(bgTopLeftCorner[0] + currMenuWidth, bgTopLeftCorner[1], -0.5f);
	glEnd();

	Camera::PopWindowCoords();
}

/**
 * The selection indicator for a sub menu is a horizontal arrow pointing at the
 * currently selected menu item.
 */
void GameSubMenu::DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem) {
	const float ARROW_HEIGHT = menuItem.GetHeight();
	const float X_POS = itemPos[0] - (GameSubMenu::BACKGROUND_PADDING / 2.0f) - GameSubMenu::HALF_ARROW_WIDTH;
	const float Y_POS = itemPos[1] - (menuItem.GetHeight() / 2.0f) + (ARROW_HEIGHT / 2.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Camera::PushWindowCoords();

	// Fill in the background
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, 1.0f);
	
	glPushMatrix();
	glTranslatef(X_POS + this->arrowBounceAnim.GetInterpolantValue(), Y_POS, 0.0f);
	glScalef(this->arrowSquishAnim.GetInterpolantValue(), 1.0f, 1.0f);

	this->arrowTex->BindTexture();

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-GameSubMenu::HALF_ARROW_WIDTH, 0.0f, 0.0f);
		glTexCoord2f(1, 0);
		glVertex3f(-GameSubMenu::HALF_ARROW_WIDTH, -ARROW_HEIGHT, 0.0f);
		glTexCoord2f(1, 1);
		glVertex3f(GameSubMenu::HALF_ARROW_WIDTH, -ARROW_HEIGHT, 0.0f);
		glTexCoord2f(0, 1);
		glVertex3f(GameSubMenu::HALF_ARROW_WIDTH, 0.0f, 0.0f);
	glEnd();

	this->arrowTex->UnbindTexture();

	glPopMatrix();
	glDisable(GL_BLEND);

	Camera::PopWindowCoords();

	// Tick the animations
	this->arrowBounceAnim.Tick(dT);
	this->arrowSquishAnim.Tick(dT);
}

void GameSubMenu::SetSelectedMenuItem(int index) {
	assert(index >= 0 && index < static_cast<int>(this->menuItems.size()));
	
	// Deselect the previous selection if necessary
	if (this->selectedMenuItemIndex >= 0 && this->selectedMenuItemIndex < static_cast<int>(this->menuItems.size())) {
		this->menuItems[this->selectedMenuItemIndex]->ToggleWiggleAnimationOff();
		this->menuItems[this->selectedMenuItemIndex]->SetSize(false);
	}

	// Select the new selection
	this->selectedMenuItemIndex = index;
	this->menuItems[index]->ToggleWiggleAnimationOn(GameMenuItem::MENU_ITEM_WOBBLE_AMT_SMALL, GameMenuItem::MENU_ITEM_WOBBLE_FREQ);
	this->menuItems[index]->SetSize(true);
	this->menuItems[index]->SetTextColour(this->highlightColour);
}