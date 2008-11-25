#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

#include "../BlammoEngine/BlammoEngine.h"

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem {
private:
	TextLabel2D textLabel;

public:
	GameMenuItem(const TextLabel2D& textLabel);
	~GameMenuItem();

	void Draw(const Point2D& topLeftCorner);
	unsigned int GetHeight() const;

	void SetTextColour(const Colour& c) {
		this->textLabel.SetColour(c);
	}

};

/**
 * An organized set of menu items, formated for selection by the
 * user.
 */
class GameMenu {

private:
	int highlightedMenuItemIndex;
	Point2D topLeftCorner;
	unsigned int menuItemPadding;						// Padding space between items in the menu
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

	Colour idleColour;					// Colour of menu items when they are left alone
	Colour highlightColour;			// Colour of menu items when they are highlighted
	Colour selectionColour;			// Colour of menu items when they are selected

public:
	GameMenu(const Point2D& topLeftCorner);
	~GameMenu();

	// Sets the colour scheme for all items in the menu
	void SetColourScheme(const Colour& idle, const Colour& highlight, const Colour& selection) {
		this->idleColour = idle;
		this->highlightColour = highlight;
		this->selectionColour = selection;
	}

	// Set the currently highlighted menu item
	void SetHighlightedMenuItem(int index) {
		assert(index >= 0 && index < static_cast<int>(this->menuItems.size()));
		this->highlightedMenuItemIndex = index;
	}

	// Adds a new item to the very end of this menu
	void AddMenuItem(const TextLabel2D& label) {
		this->menuItems.push_back(new GameMenuItem(label));
	}
	
	// Sets the padding between menu items, measured in pixels
	void SetPaddingBetweenMenuItems(unsigned int paddingInPixels) {
		this->menuItemPadding = paddingInPixels;
	}
	
	// Sets the top-left corner of the menu
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}

	// Obtain the index for the currently highlighted menu item
	int GetHighlightedMenuItem() const {
		return this->highlightedMenuItemIndex;
	}

	void Draw();
	void DebugDraw();

	// Tell the menu that the user has moved their selection up 1 item
	void UpAction() {
		if (this->menuItems.size() == 0) {
			return;
		}
		this->highlightedMenuItemIndex = (this->highlightedMenuItemIndex + this->menuItems.size() - 1) % this->menuItems.size();
	}

	// Tell the menu that the user has moved their selection down 1 item
	void DownAction() {
		if (this->menuItems.size() == 0) {
			return;
		}
		this->highlightedMenuItemIndex = (this->highlightedMenuItemIndex + 1) % this->menuItems.size();
	}

};

#endif