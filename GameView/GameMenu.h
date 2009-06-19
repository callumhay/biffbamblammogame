#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem {
private:
	TextLabel2D textLabel;
	AnimationMultiLerp<float> wiggleAnimation;

public:
	GameMenuItem(const TextLabel2D& textLabel);
	~GameMenuItem();

	void Draw(double dT, const Point2D& topLeftCorner);
	unsigned int GetHeight() const;
	unsigned int GetWidth() const;

	void ToggleWiggleAnimationOn(float amplitude, float freqency);
	void ToggleWiggleAnimationOff();

	inline void SetTextColour(const Colour& c) {
		this->textLabel.SetColour(c);
	}

};

/**
 * An organized set of menu items, formated for selection by the
 * user.
 */
class GameMenu {

private:
	static const float UP_DOWN_ARROW_PADDING;

	int selectedMenuItemIndex;
	Point2D topLeftCorner;
	unsigned int menuItemPadding;						// Padding space between items in the menu
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

	Colour idleColour;					// Colour of menu items when they are left alone
	Colour highlightColour;			// Colour of menu items when they are highlighted
	Colour selectionColour;			// Colour of menu items when they are selected

	void DrawUpDownArrows(const Point2D& itemPos, const GameMenuItem& menuItem);

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
	void SetSelectedMenuItem(int index) {
		assert(index >= 0 && index < static_cast<int>(this->menuItems.size()));
		
		// Deselect the previous selection if necessary
		if (this->selectedMenuItemIndex >= 0 && this->selectedMenuItemIndex < static_cast<int>(this->menuItems.size())) {
			this->menuItems[this->selectedMenuItemIndex]->ToggleWiggleAnimationOff();
		}

		// Select the new selection
		this->selectedMenuItemIndex = index;
		this->menuItems[index]->ToggleWiggleAnimationOn(5.0f, 0.10f);
	}
	// Obtain the index for the currently highlighted menu item
	int GetSelectedMenuItem() const {
		return this->selectedMenuItemIndex;
	}

	/**
	 * Adds a new item to the very end of this menu.
	 * Returns: The index of the item added in this menu.
	 */
	int AddMenuItem(const TextLabel2D& label) {
		this->menuItems.push_back(new GameMenuItem(label));
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

};

#endif