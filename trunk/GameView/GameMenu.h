/**
 * GameMenu.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

#include "GameFontAssetsManager.h"
#include "GameMenuItem.h"

class Texture;

/**
 * The game menu events class is an abstract class / interface that will
 * allow someone using the game menu to implement their own methods of
 * handling certain menu events.
 */
class GameMenuEventHandler {
public:
	GameMenuEventHandler() {}
	virtual ~GameMenuEventHandler() {}

	/**
	 * Event called when a new menu item has been highlighted.
	 * Parameters: itemIndex - the index of the item that was highlighted.
	 */
	virtual void GameMenuItemHighlightedEvent(int itemIndex) = 0;

	/**
	 * Event called when a menu item has been activated.
	 * Parameters: itemIndex - the index of the item that was activated.
	 */
	virtual void GameMenuItemActivatedEvent(int itemIndex) = 0;

	/**
	 * Event called when a menu item has changed (e.g., a new selection is made).
	 * Parameters: itemIndex - the index of the item that changed/was selected.
	 */
	virtual void GameMenuItemChangedEvent(int itemIndex) = 0;

	/**
	 * Event called when a menu item has both been activated and then verified (via a verify menu).
	 * Parameters: itemIndex - the index of the item that has been verified.
	 */
	virtual void GameMenuItemVerifiedEvent(int itemIndex) = 0;

	/** 
	 * Event called when the user tries to quickly escape from a menu
	 * (by hitting Esc key).
	 */
	virtual void EscMenu() = 0;
};

/**
 * An organized set of menu items, formated for selection by the
 * user.
 */
class GameMenu {
public:
	enum MenuAlignment { LeftJustified, CenterJustified };

	static const float BACKGROUND_PADDING;
	static const int NUM_RAND_COLOURS = 18;
	static const Colour RAND_COLOUR_LIST[GameMenu::NUM_RAND_COLOURS];

	GameMenu();
	GameMenu(const Point2D& topLeftCorner);
	virtual ~GameMenu();

	static void DrawBackgroundQuad(float halfMenuWidth, float halfMenuHeight);

	void SetAlignment(MenuAlignment align) {
		this->alignment = align;
	}

	void AddEventHandler(GameMenuEventHandler* eventHandler) {
		assert(eventHandler != NULL);
		this->eventHandlers.push_back(eventHandler);
	}

	// Sets the colour scheme for all items in the menu
	void SetColourScheme(const Colour& idle, const Colour& highlight, const Colour& activate, const Colour& greyedOut) {
		this->idleColour = idle;
		this->highlightColour = highlight;
		this->activateColour = activate;
		this->greyedOutColour = greyedOut;
	}

	// Set the currently highlighted menu item
	virtual void SetSelectedMenuItem(int index);

	// Functions for Activating and Deactivating the currently selected/highlighted menu item
	void ActivateSelectedMenuItem();
	void DeactivateSelectedMenuItem();

	// Obtain the index for the currently highlighted menu item
	int GetSelectedMenuItem() const {
		return this->selectedMenuItemIndex;
	}

	/**
	 * Adds a new item to the very end of this menu. An item requires
	 * two text labels to define its small and large characteristics as well
	 * as a possible submenu for that item (allowed to be NULL).
	 * Returns: The index of the item added in this menu.
	 */
	int AddMenuItem(TextLabel2D& smLabel, TextLabel2D& lgLabel, GameSubMenu* subMenu) {
		GameMenuItem* newMenuItem = new GameMenuItem(smLabel, lgLabel, subMenu);
		newMenuItem->SetParent(this);
		this->menuItems.push_back(newMenuItem);
		
		this->menuHeight += smLabel.GetHeight() + this->GetMenuItemPadding();
		this->menuWidth = std::max<float>(lgLabel.GetLastRasterWidth(), this->menuWidth);

		return this->menuItems.size() - 1;
	}
	int AddMenuItem(GameMenuItem* menuItem) {
		assert(menuItem != NULL);
		menuItem->SetParent(this);
		this->menuItems.push_back(menuItem);

		this->menuHeight += menuItem->GetHeight() + this->GetMenuItemPadding();
		this->menuWidth = std::max<float>(menuItem->GetWidth(), this->menuWidth);

		return this->menuItems.size() - 1;
	}

	// Sets the padding between menu items, measured in pixels
	void SetPaddingBetweenMenuItems(unsigned int paddingInPixels) {
		this->menuItemPadding = paddingInPixels;
	}

	// Sets the top-left corner of the menu
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}
	void SetCenteredOnScreen(int screenWidth, int screenHeight);

	void Draw(double dT, int windowWidth, int windowHeight);
	void DebugDraw();

	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);

	inline void MenuItemHighlighted() {
		for (std::list<GameMenuEventHandler*>::iterator iter = this->eventHandlers.begin(); iter != this->eventHandlers.end(); ++iter) {
			(*iter)->GameMenuItemHighlightedEvent(this->selectedMenuItemIndex);
		}
	}

	/**
	 * Tell this menu that the currently activated menu item has changed
	 */
	inline void ActivatedMenuItemChanged() {
		for (std::list<GameMenuEventHandler*>::iterator iter = this->eventHandlers.begin(); iter != this->eventHandlers.end(); ++iter) {
			(*iter)->GameMenuItemChangedEvent(this->selectedMenuItemIndex);
		}
	}

	/**
	 * Tell this menu that the currently activated menu item has been verified
	 */
	inline void ActivatedMenuItemVerified() {
		for (std::list<GameMenuEventHandler*>::iterator iter = this->eventHandlers.begin(); iter != this->eventHandlers.end(); ++iter) {
			(*iter)->GameMenuItemVerifiedEvent(this->selectedMenuItemIndex);
		}
	}

protected:
	static const float UP_DOWN_ARROW_HEIGHT;
	static const float UP_DOWN_ARROW_TOP_PADDING;
	static const float UP_DOWN_ARROW_BOTTOM_PADDING;

	std::list<GameMenuEventHandler*> eventHandlers;	// Event handlers for this game menu
	
	float menuItemPadding;
	float menuWidth, menuHeight;
	int selectedMenuItemIndex;			// The currently selected menu item
	bool isSelectedItemActivated;		// Whether the currently selected menu item has been activated

	Point2D topLeftCorner;
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

	Colour idleColour;					// Colour of menu items when they are left alone
	Colour highlightColour;			// Colour of menu items when they are highlighted
	Colour activateColour;			// Colour of menu items when they are selected
	Colour greyedOutColour;			// Colour when item is not in the current menu level

	Colour bgColour;	// Colour assigned to the background for this menu

	AnimationMultiLerp<float> selArrowScaleAnim;
	AnimationMultiLerp<float> selArrowFadeAnim;

	MenuAlignment alignment;

	virtual float GetMenuItemPadding() const;
	virtual void DrawMenuBackground(double dT);
	virtual void DrawMenuItem(double dT, const Point2D& pos, GameMenuItem& menuItem, int windowWidth, int windowHeight) { 
		menuItem.Draw(dT, pos, windowWidth, windowHeight); 
	}
	virtual void DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem);
	
	// Tell the menu that the user has moved their selection up 1 item
	void UpAction() {
		if (this->menuItems.size() == 0) {
			return;
		}
		this->SetSelectedMenuItem((this->selectedMenuItemIndex + this->menuItems.size() - 1) % this->menuItems.size());
	}

	// Tell the menu that the user has moved their selection down 1 item
	void DownAction() {
		if (this->menuItems.size() == 0) {
			return;
		}
		this->SetSelectedMenuItem((this->selectedMenuItemIndex + 1) % this->menuItems.size());
	}

private:
	void SetupAnimations();
};

/**
 * A menu for a particular menu item in the GameMenu - this menu is a sub menu
 * associated with items in a parent menu.
 */
class GameSubMenu : public GameMenu {
private:
	Texture* arrowTex;

	static const double ARROW_ANIM_FREQ;
	static const float ARROW_BOUNCE_AMT;
	static const float ARROW_SQUISH_AMT;

	AnimationMultiLerp<float> arrowBounceAnim;
	AnimationMultiLerp<float> arrowSquishAnim;
	
	AnimationMultiLerp<Vector2D> menuBGOpenAnim;
	AnimationMultiLerp<float> menuItemOpenFadeIn;

	AnimationMultiLerp<Vector2D> menuBGOpenGhostScale;
	AnimationMultiLerp<float> menuBGOpenGhostFade;

protected:
	float GetMenuItemPadding() const;
	void DrawMenuBackground(double dT);
	void DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem);
	void DrawMenuItem(double dT, const Point2D& pos, GameMenuItem& menuItem, int windowWidth, int windowHeight);

public:
	static const float HALF_ARROW_WIDTH;
	
	GameSubMenu();
	~GameSubMenu();

	void SetSelectedMenuItem(int index);

	void AnimateMenuOpen();

};

#endif