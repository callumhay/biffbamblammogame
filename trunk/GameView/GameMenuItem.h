/**
 * GameMenuItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEMENUITEM_H__
#define __GAMEMENUITEM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

class GameMenu;
class GameSubMenu;

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem {
protected:
	GameSubMenu* subMenu;
	TextLabel2D* currLabel;
	TextLabel2D smTextLabel, lgTextLabel;
	AnimationMultiLerp<float> wiggleAnimation;

public:
	static const float MENU_ITEM_WOBBLE_AMT_LARGE;
	static const float MENU_ITEM_WOBBLE_AMT_SMALL;
	static const float MENU_ITEM_WOBBLE_FREQ;
	static const float SUB_MENU_PADDING;

	GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameSubMenu* subMenu);
	virtual ~GameMenuItem();

	virtual void Draw(double dT, const Point2D& topLeftCorner);
	virtual void KeyPressed(GameMenu* parent, SDLKey key) {};

	unsigned int GetHeight() const;
	virtual unsigned int GetWidth() const;
	
	GameSubMenu* GetSubMenu() { return this->subMenu; }

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
	inline void SetTextColour(const ColourRGBA& c) {
		this->currLabel->SetColour(c);
	}
	inline ColourRGBA GetTextColour() const {
		return this->currLabel->GetColour();
	}
};

/**
 * A specific kind of list item - one that offers a selection of options
 * that can be scrolled though and selected.
 */
class SelectionListMenuItem : public GameMenuItem {
private:
	static const int NO_SELECTION = -1;

	static const float INTERIOR_PADDING;
	static const float SELECTION_ARROW_WIDTH;

	int selectedIndex;												// Index in the list that's currently selected 
	std::vector<std::string> selectionList;		// List of items that can be selected
	std::string baseLabelStr;									// The label of this item (this text always appears on the item)
	
	float maxWidth;	// The maximum width of this menu item

	void DrawSelectionArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing);

public:
	SelectionListMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, const std::vector<std::string>& items);
	virtual ~SelectionListMenuItem();

	void SetSelectionList(const std::vector<std::string>& items);

	virtual void Draw(double dT, const Point2D& topLeftCorner);
	virtual void KeyPressed(GameMenu* parent, SDLKey key);

	virtual unsigned int GetWidth() const { return this->maxWidth; }
};

#endif