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

// GameMenu Functions **********************************************

const float GameMenu::UP_DOWN_ARROW_TOP_PADDING		 = 8.0f;
const float GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING = 16.0f;

const Colour GameMenu::RAND_COLOUR_LIST[GameMenu::NUM_RAND_COLOURS] = {
	Colour(0.4375f, 0.5f, 0.5647f),							// slate greyish-blue
	Colour(0.2745098f, 0.5098039f, 0.70588f),		// steel blue
	Colour(0.28235f, 0.2392f, 0.545098f),				// slate purple-blue
	Colour(0.51372549f, 0.4352941f, 1.0f),			// slate purple
	Colour(0.8588235f, 0.439215686f, 0.57647f),	// pale violet
	Colour(1.0f, 0.75686f, 0.75686f),						// rosy brown 
	Colour(0.7215686f, 0.52549f, 0.043f),				// goldenrod
	Colour(0.4196f, 0.5568627f, 0.1372549f),		// olive
	Colour(0.4f, 0.8039215f, 0.666667f),				// deep aquamarine
	Colour(0.3725f, 0.6196078f, 0.62745098f)		// cadet (olive-) blue
};


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
	glLineWidth(3.0f);
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
	this->DrawMenuBackground(dT);

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
		
		this->DrawMenuItem(dT, currPos, *currItem);
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
	GameMenuItem* activatedItem = this->menuItems[this->selectedMenuItemIndex];
	activatedItem->SetTextColour(this->activateColour);
	activatedItem->ToggleWiggleAnimationOff();
	
	if (activatedItem->GetSubMenu() != NULL) {
		activatedItem->GetSubMenu()->AnimateMenuOpen();
	}
	
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
		GameMenuItem* activatedMenuItem = this->menuItems[this->selectedMenuItemIndex];
		currentMenu = activatedMenuItem->GetSubMenu();
		
		if (currentMenu == NULL) {
			// In this case we are dealing with a menu item that takes input,
			// give that menu item the input
			activatedMenuItem->KeyPressed(this, key);
		}
		else {
			// Recursively tell the menu about the key press
			currentMenu->KeyPressed(key);
		}
		return;
	}
	
	// If we managed to get here then the key press is specific to this menu,
	// handle the input by changing the menu according to the key pressed.
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

	// Set the background open animations	
	std::vector<double> openTimeVals;
	openTimeVals.reserve(3);
	openTimeVals.push_back(0.0);
	openTimeVals.push_back(0.25);
	openTimeVals.push_back(0.4);

	std::vector<Vector2D> bgScaleAmts;
	bgScaleAmts.reserve(3);
	bgScaleAmts.push_back(Vector2D(EPSILON, EPSILON));
	bgScaleAmts.push_back(Vector2D(1, 1));
	bgScaleAmts.push_back(Vector2D(1, 1));

	std::vector<float> itemFadeAmts;
	itemFadeAmts.reserve(3);
	itemFadeAmts.push_back(0.0f);
	itemFadeAmts.push_back(0.0f);
	itemFadeAmts.push_back(1.0f);

	this->menuBGOpenAnim.SetLerp(openTimeVals, bgScaleAmts);
	this->menuItemOpenFadeIn.SetLerp(openTimeVals, itemFadeAmts);
	this->menuBGOpenAnim.SetRepeat(false);
	this->menuItemOpenFadeIn.SetRepeat(false);

	const int randColourIdx = Randomizer::GetInstance()->RandomUnsignedInt() % GameMenu::NUM_RAND_COLOURS;
	this->randBGColour = GameMenu::RAND_COLOUR_LIST[randColourIdx];
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
void GameSubMenu::DrawMenuBackground(double dT) {

	const Vector2D openAnimScale = this->menuBGOpenAnim.GetInterpolantValue();

	const float MENU_WIDTH = (this->menuWidth + 2 * BACKGROUND_PADDING + 2 * GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE) * openAnimScale[0];
	const float MENU_HEIGHT = (this->menuHeight + 2 * BACKGROUND_PADDING) * openAnimScale[1];
	
	const float MENU_WIDTH_DIV2		= MENU_WIDTH / 2.0f;
	const float MENU_HEIGHT_DIV2	= MENU_HEIGHT / 2.0f;
		
	const Point2D bgTopLeftCorner = this->topLeftCorner + Vector2D(-BACKGROUND_PADDING + MENU_WIDTH_DIV2, BACKGROUND_PADDING - MENU_HEIGHT_DIV2);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Draw the outline of the background
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(3.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(bgTopLeftCorner[0], bgTopLeftCorner[1], -0.5f);

	glBegin(GL_QUADS);
		glVertex2f(-MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
		glVertex2f(-MENU_WIDTH_DIV2, -MENU_HEIGHT_DIV2);
		glVertex2f(MENU_WIDTH_DIV2, -MENU_HEIGHT_DIV2);
		glVertex2f(MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
	glEnd();

	// Fill in the background
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(this->randBGColour.R(), this->randBGColour.G(), this->randBGColour.B(), 1.0f);

	glBegin(GL_QUADS);
		glVertex2f(-MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
		glVertex2f(-MENU_WIDTH_DIV2, -MENU_HEIGHT_DIV2);
		glVertex2f(MENU_WIDTH_DIV2, -MENU_HEIGHT_DIV2);
		glVertex2f(MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
	glEnd();
	
	glPopMatrix();
	Camera::PopWindowCoords();

	this->menuBGOpenAnim.Tick(dT);
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

void GameSubMenu::DrawMenuItem(double dT, const Point2D& pos, GameMenuItem& menuItem) {
	ColourRGBA textColour = menuItem.GetTextColour();
	textColour[3] = this->menuItemOpenFadeIn.GetInterpolantValue();
	menuItem.SetTextColour(textColour);
	menuItem.Draw(dT, pos);

	this->menuItemOpenFadeIn.Tick(dT);
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

/** 
 * Start the animation to open up this submenu. This will animate the scale
 * of the background for the menu as well as the alpha of the items in the menu.
 */
void GameSubMenu::AnimateMenuOpen() {
	// Restart the animations to open the menu and fade in its items
	this->menuBGOpenAnim.ResetToStart();
	this->menuItemOpenFadeIn.ResetToStart();
	
	// Choose a new colour for the menu
	const int randColourIdx = Randomizer::GetInstance()->RandomUnsignedInt() % GameMenu::NUM_RAND_COLOURS;
	this->randBGColour = GameMenu::RAND_COLOUR_LIST[randColourIdx];
}