/**
 * GameMenu.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
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

class GameMenu;
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
	 * Event called when a menu item has been activated.
	 * Parameters: itemIndex - the index of the item that was activated.
	 */
	virtual void GameMenuItemActivatedEvent(int itemIndex) = 0;

	/** 
	 * Event called when the user tries to quickly escape from a menu
	 * (by hitting Esc key).
	 */
	virtual void EscMenu() = 0;
};

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem {
private:
	TextLabel2D* currLabel;
	TextLabel2D smTextLabel, lgTextLabel;

	AnimationMultiLerp<float> wiggleAnimation;

	GameMenu* subMenu;

public:
	static const float MENU_ITEM_WOBBLE_AMT_LARGE;
	static const float MENU_ITEM_WOBBLE_AMT_SMALL;
	static const float MENU_ITEM_WOBBLE_FREQ;
	static const float SUB_MENU_PADDING;

	GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu);
	~GameMenuItem();

	void Draw(double dT, const Point2D& topLeftCorner);
	unsigned int GetHeight() const;
	unsigned int GetWidth() const;
	GameMenu* GetSubMenu() { return this->subMenu; }

	void ToggleWiggleAnimationOn(float amplitude, float frequency);
	void ToggleWiggleAnimationOff();

	inline void SetSize(bool isLarge) {
		if (isLarge) {
			this->currLabel = &this->lgTextLabel;
		}
		else {
			this->currLabel = &this->smTextLabel;
		}
	}

	inline void SetTextColour(const Colour& c) {
		this->currLabel->SetColour(c);
	}

};

/**
 * An organized set of menu items, formated for selection by the
 * user.
 */
class GameMenu {

protected:
	static const float UP_DOWN_ARROW_TOP_PADDING;
	static const float UP_DOWN_ARROW_BOTTOM_PADDING;

	std::list<GameMenuEventHandler*> eventHandlers;	// Event handlers for this game menu

	int selectedMenuItemIndex;			// The currently selected menu item
	bool isSelectedItemActivated;		// Whether the currently selected menu item has been activated

	Point2D topLeftCorner;
	unsigned int menuItemPadding;						// Padding space between items in the menu
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

	Colour idleColour;					// Colour of menu items when they are left alone
	Colour highlightColour;			// Colour of menu items when they are highlighted
	Colour activateColour;			// Colour of menu items when they are selected
	Colour greyedOutColour;			// Colour when item is not in the current menu level

	virtual float GetMenuItemPadding() const;
	virtual void DrawMenuBackground() {}
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

public:
	GameMenu();
	GameMenu(const Point2D& topLeftCorner);
	~GameMenu();

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
	virtual int AddMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu) {
		this->menuItems.push_back(new GameMenuItem(smLabel, lgLabel, subMenu));
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

	void Draw(double dT);
	void DebugDraw();

	void KeyPressed(SDLKey key);
};

class GameSubMenu : public GameMenu {
private:
	Texture* arrowTex;
	float menuWidth, menuHeight;

	static const double ARROW_ANIM_FREQ;
	static const float ARROW_BOUNCE_AMT;
	static const float ARROW_SQUISH_AMT;

	AnimationMultiLerp<float> arrowBounceAnim;
	AnimationMultiLerp<float> arrowSquishAnim;

protected:
	virtual float GetMenuItemPadding() const;
	virtual void DrawMenuBackground();
	virtual void DrawSelectionIndicator(double dT, const Point2D& itemPos, const GameMenuItem& menuItem);

public:
	static const float HALF_ARROW_WIDTH;
	static const float BACKGROUND_PADDING;

	GameSubMenu();
	~GameSubMenu();

	virtual int AddMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu) {
		this->menuItems.push_back(new GameMenuItem(smLabel, lgLabel, subMenu));
		
		this->menuHeight += smLabel.GetHeight() + this->GetMenuItemPadding();
		this->menuWidth = std::max<float>(lgLabel.GetLastRasterWidth(), this->menuWidth);

		return this->menuItems.size() - 1;
	}

	virtual void SetSelectedMenuItem(int index);

};

#endif