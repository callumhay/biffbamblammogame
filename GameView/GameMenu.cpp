/**
 * GameMenu.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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

const float GameMenu::UP_DOWN_ARROW_HEIGHT          = 12.0f;	
const float GameMenu::UP_DOWN_ARROW_TOP_PADDING     = 8.0f;
const float GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING  = 12.0f;
const float GameMenu::BACKGROUND_PADDING            = 12.0f;

const Colour GameMenu::RAND_COLOUR_LIST[GameMenu::NUM_RAND_COLOURS] = {
	Colour(0.89f, 0.149f, 0.2118f),                 // Alizarin Red
	Colour(0.6f, 0.4f, 0.8f),                       // Amethyst Purple
	Colour(0.0f, 0.5f, 1.0f),                       // Azure Blue
	Colour(0.8588235f, 0.439215686f, 0.57647f),     // Pale Violet
	Colour(1.0f, 0.3333f, 0.639f),                  // Rose
	Colour(0.39f, 0.5843f, 0.9294f),                // Cornflower Blue
	Colour(0.8745f, 0.451f, 1.0f),                  // Pinkish-Purple (Heliotrope)
	Colour(0.251f, 0.8784f, 0.815686f),             // Turquoise
	Colour(0.3137f, 0.7843f, 0.47f),                // Emerald Green
	Colour(1.0f, 0.6f, 0.4f),                       // Grapefruit pink
	Colour(0.57647f, 0.4392f, 0.8588f),             // Medium Purple
	Colour(0.45098f, 0.76078f, 0.98431f)            // Maya Blue
};

GameMenu::GameMenu() : topLeftCorner(Point2D(0,0)), menuItemPadding(0.0f),
selectedMenuItemIndex(-1), isSelectedItemActivated(false), menuWidth(0.0f), menuHeight(0.0f),
alignment(GameMenu::LeftJustified)  {
	
	this->bgColour = Colour(0.45098f, 0.76078f, 0.98431f);	// Maya Blue
	this->SetupAnimations();
}

GameMenu::GameMenu(const Point2D& topLeftCorner) : topLeftCorner(topLeftCorner), menuItemPadding(0.0f),
selectedMenuItemIndex(-1), isSelectedItemActivated(false), menuWidth(0.0f), menuHeight(0.0f),
alignment(GameMenu::LeftJustified) {
	
	this->bgColour = Colour(0.45098f, 0.76078f, 0.98431f);	// Maya Blue
	this->SetupAnimations();
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
 * Private helper function for setting up the game menu animations (e.g., selection
 * arrows pulsing).
 */
void GameMenu::SetupAnimations() {
	const double SEL_ARROW_PULSE_TIME = 0.45;
	
	std::vector<double> timeVals;
	timeVals.reserve(3);
	timeVals.push_back(0.0);
	timeVals.push_back(SEL_ARROW_PULSE_TIME);
	timeVals.push_back(2 * SEL_ARROW_PULSE_TIME);

	std::vector<float> scaleVals;
	scaleVals.reserve(3);
	scaleVals.push_back(1.0f);
	scaleVals.push_back(0.5f);
	scaleVals.push_back(1.0f);

	std::vector<float> fadeVals;
	fadeVals.reserve(3);
	fadeVals.push_back(1.0f);
	fadeVals.push_back(0.5f);
	fadeVals.push_back(1.0f);

	this->selArrowScaleAnim.SetLerp(timeVals, scaleVals);
	this->selArrowFadeAnim.SetLerp(timeVals, fadeVals);
	this->selArrowScaleAnim.SetRepeat(true);
	this->selArrowFadeAnim.SetRepeat(true);
}

/**
 * Determine the amount of padding between menu items when drawing
 * this menu.
 */
float GameMenu::GetMenuItemPadding() const {
    float scaling = 1.0f;
    if (!this->menuItems.empty()) {
        scaling = this->menuItems.front()->GetCurrLabel()->GetScale();
    }

	return this->menuItemPadding + scaling * (GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING + GameMenu::UP_DOWN_ARROW_TOP_PADDING + GameMenu::UP_DOWN_ARROW_HEIGHT);
}

void GameMenu::DrawBackgroundQuad(float halfMenuWidth, float halfMenuHeight) {
	glBegin(GL_QUADS);
		glVertex2f(-halfMenuWidth, halfMenuHeight);
		glVertex2f(-halfMenuWidth, -halfMenuHeight);
		glVertex2f(halfMenuWidth, -halfMenuHeight);
		glVertex2f(halfMenuWidth, halfMenuHeight);
	glEnd();
}

void GameMenu::DrawBackgroundQuadOutline(float halfMenuWidth, float halfMenuHeight) {
    glBegin(GL_LINE_LOOP);
		glVertex2f(-halfMenuWidth, halfMenuHeight);
		glVertex2f(-halfMenuWidth, -halfMenuHeight);
		glVertex2f(halfMenuWidth, -halfMenuHeight);
		glVertex2f(halfMenuWidth, halfMenuHeight);
    glEnd();
}

/**
 * Draw an outline around the menu with a colourful plain background.
 */
void GameMenu::DrawMenuBackground(double dT) {
	UNUSED_PARAMETER(dT);
}

/**
 * Draw up and down arrows around a menu item at the given position.
 */
void GameMenu::DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem) {
	const float ARROW_WIDTH				= 40 * menuItem.GetCurrLabel()->GetScale();
	const float HALF_ARROW_WIDTH	= ARROW_WIDTH / 2.0f;
	const float HALF_ARROW_HEIGHT	= GameMenu::UP_DOWN_ARROW_HEIGHT * menuItem.GetCurrLabel()->GetScale() / 2.0f;

	const float SCALED_PADDING = GameMenu::UP_DOWN_ARROW_BOTTOM_PADDING * menuItem.GetCurrLabel()->GetScale();
	const float ARROW_X = (menuItem.GetWidth() - ARROW_WIDTH) / 2.0f;
	const float TOP_ARROW_BOTTOM_Y	= SCALED_PADDING;
	
	const float BOTTOM_ARROW_TOP_Y		=  -1.0f * (static_cast<float>(menuItem.GetHeight()) + SCALED_PADDING);
	//const float BOTTOM_ARROW_BOTTOM_Y	= BOTTOM_ARROW_TOP_Y - GameMenu::UP_DOWN_ARROW_HEIGHT * menuItem.GetCurrLabel()->GetScale();

	float selArrowPulseScale = this->selArrowScaleAnim.GetInterpolantValue();
	float selArrowPulseFade	 = this->selArrowFadeAnim.GetInterpolantValue();

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Draw the top arrow first
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(itemPos[0] + ARROW_X + HALF_ARROW_WIDTH, itemPos[1] + TOP_ARROW_BOTTOM_Y + HALF_ARROW_HEIGHT, 0.0f);
	glScalef(selArrowPulseScale, selArrowPulseScale, selArrowPulseScale);

	// Fill in the arrows
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, selArrowPulseFade);
	
	glBegin(GL_TRIANGLES);
	glVertex2f(-HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT);
	glVertex2f(HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT);
	glVertex2f(0.0f, HALF_ARROW_HEIGHT);
	glEnd();

	glLoadIdentity();
	glTranslatef(itemPos[0] + ARROW_X + HALF_ARROW_WIDTH, itemPos[1] + BOTTOM_ARROW_TOP_Y - HALF_ARROW_HEIGHT, 0.0f);
	glScalef(selArrowPulseScale, selArrowPulseScale, selArrowPulseScale);
	
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, -HALF_ARROW_HEIGHT);
	glVertex2f(HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT);
	glVertex2f(-HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT);
	glEnd();

	glPopMatrix();
	Camera::PopWindowCoords();

	glPopAttrib();

	this->selArrowScaleAnim.Tick(dT);
	this->selArrowFadeAnim.Tick(dT);
}

/**
 * Sets this menu to be centered on the screen given the screen width and height.
 */
void GameMenu::SetCenteredOnScreen(int screenWidth, int screenHeight) {
	assert(screenWidth > 0);
	assert(screenHeight > 0);

	// Grab the largest possible item
	float maxHalfWidth = 0.0f;
	float totalHeight  = 0.0f;
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		maxHalfWidth = std::max<float>(currItem->GetWidth(), maxHalfWidth);
		totalHeight  += currItem->GetHeight() + this->GetMenuItemPadding();
	}
	maxHalfWidth *= 0.5f;

	this->topLeftCorner[0] = (screenWidth * 0.5f) - maxHalfWidth;
	this->topLeftCorner[1] = (screenHeight + totalHeight) * 0.5f;
}

/**
 * Draw this game menu in its current state.
 */
void GameMenu::Draw(double dT, int windowWidth, int windowHeight) {
	
	GameMenu* subMenu = NULL;

	// In the case of a center alignment we need to know what menu item is the largest
	// so that we can draw everything centered to it
	float maxHalfWidth = 0.0f;
	if (this->alignment == GameMenu::CenterJustified) {
		for (size_t i = 0; i < this->menuItems.size(); i++) {
			GameMenuItem* currItem = this->menuItems[i];
			maxHalfWidth = std::max<float>(currItem->GetWidth(), maxHalfWidth);
		}
		maxHalfWidth *= 0.5f;
		assert(maxHalfWidth > 0.0f);
	}

	// Draw the background of the menu
	this->DrawMenuBackground(dT);

    Point2D currPosItem      = this->topLeftCorner;
    Point2D currPosIndicator = this->topLeftCorner;

	// Draw the menu items
	for (size_t i = 0; i < this->menuItems.size(); i++) {
		GameMenuItem* currItem = this->menuItems[i];
		
		switch (this->alignment) {
			case GameMenu::LeftJustified:
				break;
			case GameMenu::CenterJustified:
				currPosItem      = Point2D(this->topLeftCorner[0] + maxHalfWidth - (0.5f * currItem->GetWidth(false)), currPosItem[1]);
                currPosIndicator = Point2D(this->topLeftCorner[0] + maxHalfWidth - (0.5f * currItem->GetWidth()), currPosIndicator[1]);
				break;
			default:
				assert(false);
				break;
		}
		
		// Check to see if we are iterating on the selected/highlighted menu item
		if (i == static_cast<size_t>(this->selectedMenuItemIndex)) {

			// Check to see if the highlighted item is activated
			if (this->isSelectedItemActivated) {
				// Since the item is activated we need to check for a submenu.
				assert(subMenu == NULL);	// Only one submenu can ever be active at a time
				subMenu = currItem->GetSubMenu();
			}
			else {
				// The item has not been activated
				this->DrawSelectionIndicator(dT, currPosIndicator, *currItem);
			}
		}
		else {
			currItem->SetTextColour(this->idleColour);
		}
		
		this->DrawMenuItem(dT, currPosItem, *currItem, windowWidth, windowHeight);

        Vector2D diff(0, currItem->GetHeight() + this->GetMenuItemPadding());
		currPosItem = currPosItem - diff;
        currPosIndicator = currPosIndicator - diff;
	}
	
	// If there was a submenu then we draw it (this can lead to a nest of menu/submenus)
	if (subMenu != NULL) {
		subMenu->Draw(dT, windowWidth, windowHeight);
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
	this->MenuItemHighlighted();
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
	activatedItem->Activate();
	
	if (activatedItem->GetSubMenu() != NULL) {
		activatedItem->GetSubMenu()->AnimateMenuOpen();
	}
	
	// Go through all the menu items except the selected one and grey them out
	for (size_t i = 0; i < this->menuItems.size(); i++) {	
		if (i == static_cast<size_t>(this->selectedMenuItemIndex)) { continue; }
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
	this->menuItems[this->selectedMenuItemIndex]->Deactivate();

	// Go through all the menu items except the selected one and set their idle colours
	for (size_t i = 0; i < this->menuItems.size(); i++) {	
		if (i == static_cast<size_t>(this->selectedMenuItemIndex)) { continue; }
		this->menuItems[i]->SetTextColour(this->idleColour);
	}
}

/**
 * Capture input from the keyboard for a key pressed event
 * and navigate or change the menu based on the given key input.
 */
void GameMenu::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	// We check to see if we're in a sub menu - so that we know where
	// to pass the key pressed event
	GameMenu* currentMenu = this;
	if (this->isSelectedItemActivated) {
		GameMenuItem* activatedMenuItem = this->menuItems[this->selectedMenuItemIndex];
		currentMenu = activatedMenuItem->GetSubMenu();
		
		if (currentMenu == NULL) {
			// In this case we are dealing with a menu item that takes input,
			// give that menu item the input
			activatedMenuItem->ButtonPressed(pressedButton);
		}
		else {
			// Recursively tell the menu about the key press
			currentMenu->ButtonPressed(pressedButton);
		}
		return;
	}
	
	// If we managed to get here then the key press is specific to this menu,
	// handle the input by changing the menu according to the key pressed.
	switch(pressedButton) {
		
		case GameControl::DownButtonAction:
			currentMenu->DownAction();
			break;

		case GameControl::UpButtonAction:
			currentMenu->UpAction();
			break;
	
		case GameControl::EnterButtonAction:
			currentMenu->ActivateSelectedMenuItem();
			break;

		case GameControl::EscapeButtonAction:
			for (std::list<GameMenuEventHandler*>::iterator iter = currentMenu->eventHandlers.begin(); iter != currentMenu->eventHandlers.end(); ++iter) {
				(*iter)->EscMenu();
			}
			break;

		default:
			break;
	}
}

void GameMenu::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	// We check to see if we're in a sub menu - so that we know where
	// to pass the key released event
	GameMenu* currentMenu = this;
	if (this->isSelectedItemActivated) {
		GameMenuItem* activatedMenuItem = this->menuItems[this->selectedMenuItemIndex];
		currentMenu = activatedMenuItem->GetSubMenu();
		
		if (currentMenu == NULL) {
			// In this case we are dealing with a menu item that takes input,
			// give that menu item the input
			activatedMenuItem->ButtonReleased(releasedButton);
		}
		else {
			// Recursively tell the menu about the key press
			currentMenu->ButtonReleased(releasedButton);
		}
		return;
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

const double GameSubMenu::ARROW_ANIM_FREQ		= 0.2;															// Frequency of arrow bouncing and squishing
const float GameSubMenu::ARROW_BOUNCE_AMT		= GameSubMenu::BACKGROUND_PADDING;	// How far the arrow deviates when bouncing
const float GameSubMenu::ARROW_SQUISH_AMT		= 0.60f;														// How much the arrow will smoosh

GameSubMenu::GameSubMenu() : GameMenu(), arrowTex(NULL) {

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
	openTimeVals.reserve(2);
	openTimeVals.push_back(0.0);
	openTimeVals.push_back(0.4);

	std::vector<Vector2D> bgScaleAmts;
	bgScaleAmts.reserve(2);
	bgScaleAmts.push_back(Vector2D(EPSILON, EPSILON));
	bgScaleAmts.push_back(Vector2D(1, 1));

	std::vector<float> itemFadeAmts;
	itemFadeAmts.reserve(2);
	itemFadeAmts.push_back(0.0f);
	itemFadeAmts.push_back(1.0f);

	this->menuBGOpenAnim.SetLerp(openTimeVals, bgScaleAmts);
	this->menuItemOpenFadeIn.SetLerp(openTimeVals, itemFadeAmts);
	this->menuBGOpenAnim.SetRepeat(false);
	this->menuItemOpenFadeIn.SetRepeat(false);

	// When the sub menu opens it has a 'ghost' that keeps opening but fades out
	openTimeVals.reserve(openTimeVals.size() + 1);
	openTimeVals.push_back(0.7);
	bgScaleAmts.reserve(bgScaleAmts.size() + 1);
	bgScaleAmts.push_back(Vector2D(1.4f, 1.4f));

	std::vector<float> ghostFadeAmts;
	ghostFadeAmts.reserve(openTimeVals.size());
	ghostFadeAmts.push_back(0.0f);
	ghostFadeAmts.push_back(1.0f);
	ghostFadeAmts.push_back(0.0f);

	this->menuBGOpenGhostScale.SetLerp(openTimeVals, bgScaleAmts);
	this->menuBGOpenGhostFade.SetLerp(openTimeVals, ghostFadeAmts);
	this->menuBGOpenGhostScale.SetRepeat(false);
	this->menuBGOpenGhostFade.SetRepeat(false);
}

GameSubMenu::~GameSubMenu() {
	// Release the arrow texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->arrowTex);
    UNUSED_VARIABLE(success);
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
		
	const float GHOST_MENU_WIDTH_DIV2		= (MENU_WIDTH * this->menuBGOpenGhostScale.GetInterpolantValue()[0]) / 2.0f;
	const float GHOST_MENU_HEIGHT_DIV2	= (MENU_HEIGHT * this->menuBGOpenGhostScale.GetInterpolantValue()[1]) / 2.0f;

	const Point2D bgTopLeftCorner = this->topLeftCorner + Vector2D(-BACKGROUND_PADDING + MENU_WIDTH_DIV2, BACKGROUND_PADDING - MENU_HEIGHT_DIV2);
	const Point2D ghostBGTopLeftCorner = this->topLeftCorner + Vector2D(-BACKGROUND_PADDING + GHOST_MENU_WIDTH_DIV2, BACKGROUND_PADDING - GHOST_MENU_HEIGHT_DIV2);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();
	
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(bgTopLeftCorner[0], bgTopLeftCorner[1], -0.5f);
	glLineWidth(4.0f);

	// Draw the outline of the ghost background
	if (this->menuBGOpenGhostFade.GetInterpolantValue() != 0) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(ghostBGTopLeftCorner[0], ghostBGTopLeftCorner[1], 0.0f);
		
		glColor4f(0, 0, 0, this->menuBGOpenGhostFade.GetInterpolantValue());
		GameMenu::DrawBackgroundQuadOutline(GHOST_MENU_WIDTH_DIV2, GHOST_MENU_HEIGHT_DIV2);
		
		glPopMatrix();
	}
	// Draw the outline of the actual background
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	GameMenu::DrawBackgroundQuadOutline(MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
	
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_POINT);
	glPointSize(4.0f);
	GameMenu::DrawBackgroundQuad(MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);

	glPolygonMode(GL_FRONT, GL_FILL);

	// Draw the fill of the ghost background
	if (this->menuBGOpenGhostFade.GetInterpolantValue() != 0) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(ghostBGTopLeftCorner[0], ghostBGTopLeftCorner[1], 0.0f);

		glColor4f(this->bgColour.R(), this->bgColour.G(), this->bgColour.B(), this->menuBGOpenGhostFade.GetInterpolantValue());
		GameMenu::DrawBackgroundQuad(GHOST_MENU_WIDTH_DIV2, GHOST_MENU_HEIGHT_DIV2);

		glPopMatrix();
	}
	// Fill in the actual background
	glColor4f(this->bgColour.R(), this->bgColour.G(), this->bgColour.B(), 1.0f);
	GameMenu::DrawBackgroundQuad(MENU_WIDTH_DIV2, MENU_HEIGHT_DIV2);
	
	glPopMatrix();
	Camera::PopWindowCoords();

	glDisable(GL_BLEND);

	this->menuBGOpenAnim.Tick(dT);
	this->menuBGOpenGhostFade.Tick(dT);
	this->menuBGOpenGhostScale.Tick(dT);
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

void GameSubMenu::DrawMenuItem(double dT, const Point2D& pos, GameMenuItem& menuItem, int windowWidth, int windowHeight) {
	ColourRGBA textColour = menuItem.GetTextColour();
	textColour[3] = this->menuItemOpenFadeIn.GetInterpolantValue();
	menuItem.SetTextColour(textColour);
	menuItem.Draw(dT, pos, windowWidth, windowHeight);

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

	this->MenuItemHighlighted();
}

/** 
 * Start the animation to open up this submenu. This will animate the scale
 * of the background for the menu as well as the alpha of the items in the menu.
 */
void GameSubMenu::AnimateMenuOpen() {
	// Restart the animations to open the menu and fade in its items
	this->menuBGOpenAnim.ResetToStart();
	this->menuItemOpenFadeIn.ResetToStart();

	this->menuBGOpenGhostScale.ResetToStart();
	this->menuBGOpenGhostFade.ResetToStart();
	
	// Choose a new colour for the menu
	const int randColourIdx = Randomizer::GetInstance()->RandomUnsignedInt() % GameMenu::NUM_RAND_COLOURS;
	this->bgColour = GameMenu::RAND_COLOUR_LIST[randColourIdx];
}