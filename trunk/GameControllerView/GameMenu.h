#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

#include <vector>
#include <string>

#include "../Utils/Point.h"
#include "../Utils/Colour.h"

class TextureFontSet;

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem {
private:
	const TextureFontSet* font;
	std::string text;

public:
	GameMenuItem(const TextureFontSet* font, const std::string& text);
	GameMenuItem(const GameMenuItem& item);
	~GameMenuItem();

	void Draw(const Point2D& topLeftCorner);
	unsigned int GetHeight() const;


};

/**
 * An organized set of menu items, formated for selection by the
 * user.
 */
class GameMenu {
public:
	//enum MenuAlignment { LeftJustified, RightJustified, CenterJustified };

private:
	//MenuAlignment alignment;							// Alignment of items in the menu
	Point2D topLeftCorner;
	unsigned int menuItemPadding;						// Padding space between items in the menu
	std::vector<GameMenuItem*> menuItems;		// The set of items in the menu

public:
	GameMenu(const Point2D& topLeftCorner);
	~GameMenu();

	// Adds a new item to the very end of this menu
	void AddMenuItem(const GameMenuItem& item) {
		this->menuItems.push_back(new GameMenuItem(item));
	}
	// Sets the padding between menu items, measured in pixels
	void SetMenuItemPadding(unsigned int paddingInPixels) {
		this->menuItemPadding = paddingInPixels;
	}

	void Draw();
	void DebugDraw();

};

#endif