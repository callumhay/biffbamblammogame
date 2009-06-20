#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

#include "GameFontAssetsManager.h"

class GameMenu;

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
	GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu);
	~GameMenuItem();

	void Draw(double dT, const Point2D& topLeftCorner);
	unsigned int GetHeight() const;
	unsigned int GetWidth() const;
	GameMenu* GetSubMenu() { return this->subMenu; }

	void ToggleWiggleAnimationOn(float amplitude, float freqency);
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

private:
	static const float UP_DOWN_ARROW_TOP_PADDING;
	static const float UP_DOWN_ARROW_BOTTOM_PADDING;

	int selectedMenuItemIndex;			// The currently selected menu item
	bool isSelectedItemActivated;		// Whether the currently selected menu item has been activated

	Point2D topLeftCorner;
	unsigned int menuItemPadding;						// Padding space between items in the menu
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

	Colour idleColour;					// Colour of menu items when they are left alone
	Colour highlightColour;			// Colour of menu items when they are highlighted
	Colour activateColour;			// Colour of menu items when they are selected
	Colour greyedOutColour;			// Colour when item is not in the current menu level

	void DrawUpDownArrows(const Point2D& itemPos, const GameMenuItem& menuItem);
	
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
	GameMenu(const Point2D& topLeftCorner);
	~GameMenu();

	// Sets the colour scheme for all items in the menu
	void SetColourScheme(const Colour& idle, const Colour& highlight, const Colour& activate, const Colour& greyedOut) {
		this->idleColour = idle;
		this->highlightColour = highlight;
		this->activateColour = activate;
		this->greyedOutColour = greyedOut;
	}

	// Set the currently highlighted menu item
	void SetSelectedMenuItem(int index);

	// Activate teh currently selected/highlighted menu item
	void ActivateSelectedMenuItem();

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
	int AddMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameMenu* subMenu) {
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

#endif