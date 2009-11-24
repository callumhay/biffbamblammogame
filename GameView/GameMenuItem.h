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


class GameMenuItemEventHandler {
public:
	virtual void MenuItemScrolled() = 0;
	virtual void MenuItemEnteredAndSet() = 0;
	virtual void MenuItemCancelled() = 0;
};

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
	GameMenuItemEventHandler* eventHandler;

public:
	static const float MENU_ITEM_WOBBLE_AMT_LARGE;
	static const float MENU_ITEM_WOBBLE_AMT_SMALL;
	static const float MENU_ITEM_WOBBLE_FREQ;
	static const float SUB_MENU_PADDING;

	GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameSubMenu* subMenu);
	virtual ~GameMenuItem();

	void SetEventHandler(GameMenuItemEventHandler* eventHandler) { 
		assert(eventHandler != NULL);
		this->eventHandler = eventHandler; 
	}

	virtual void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
	virtual void KeyPressed(GameMenu* parent, SDLKey key) {};

	unsigned int GetHeight() const;
	virtual float GetWidth() const;
	
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

	virtual void Activate() {};
	virtual void Deactivate() {};

	GameSubMenu* GetSubMenu() { return this->subMenu; }

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

	int previouslySelectedIndex;							// The index that was selected before this item was activated and possibly changed
	int selectedIndex;												// Index in the list that's currently selected 
	std::vector<std::string> selectionList;		// List of items that can be selected
	std::string baseLabelStr;									// The label of this item (this text always appears on the item)
	
	float maxWidth;	// The maximum width of this menu item

	void DrawSelectionArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing);

public:
	SelectionListMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, const std::vector<std::string>& items);
	virtual ~SelectionListMenuItem();

	void SetSelectionList(const std::vector<std::string>& items);
	
	inline void SetSelectedItem(int index) {
		assert(index >= 0 && index < static_cast<int>(this->selectionList.size()));
		this->selectedIndex = index;
	}
	inline int GetSelectedItemIndex() const {
		return this->selectedIndex;
	}
	inline std::string GetSelectedItemText() const {
		assert(this->selectedIndex >= 0 && this->selectedIndex < static_cast<int>(this->selectionList.size()));
		return this->selectionList[this->selectedIndex];
	}

	virtual void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
	virtual void KeyPressed(GameMenu* parent, SDLKey key);

	virtual float GetWidth() const { return this->maxWidth; }

	virtual void Activate();
};

/**
 * A menu item that causes a dialog asking you to verify your choice.
 */
class VerifyMenuItem : public GameMenuItem {

public:
	VerifyMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, const TextureFontSet* verifyDescFont, 
		const TextureFontSet* verifyIdleFont, const TextureFontSet* verifySelFont);
	virtual ~VerifyMenuItem();

	enum VerifyMenuOption { Confirm, Cancel };
	
	void SetSelectedVerifyMenuOption(VerifyMenuOption option);
	// Gets the currently selected/highlighted verify menu option
	VerifyMenuOption GetSelectedVerifyMenuOption() const {
		return this->selectedOption;
	}

	void SetVerifyMenuText(const std::string& descriptionText, const std::string& confirmText, const std::string& cancelText);
	void SetVerifyMenuColours(const Colour& descTxtColour, const Colour& idleColour, const Colour& selColour);

	virtual void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
	virtual void KeyPressed(GameMenu* parent, SDLKey key);

private:
	static const float VERIFY_MENU_HPADDING;
	static const float VERIFY_MENU_VPADDING;
	static const float VERIFY_MENU_OPTION_HSPACING;
	static const float VERIFY_MENU_OPTION_VSPACING;
	static const float VERIFY_MENU_OPTION_WOBBLE_FREQ;	

	bool verifyMenuActive;																		// Whether or not the verify menu is active or not
	float verifyMenuWidth, verifyMenuHeight;
	TextLabel2D descriptionLabel, confirmLabel, cancelLabel;	// Labels for the verify menu
	VerifyMenuOption selectedOption;													// The option currently selecteded/highlighted in the verify menu
	
	const TextureFontSet* verifyDescFont;
	const TextureFontSet* verifyIdleFont;
	const TextureFontSet* verifySelFont;

	Colour randomMenuBGColour;						// Random colour assigned to the background of the verify menu
	Colour idleColour, selectionColour;		// Colours used for confirm/cancel items when idle and selected
	
	AnimationMultiLerp<float> optionItemWiggleAnim;	// Wiggle animation for verify menu items
	AnimationMultiLerp<float> verifyMenuBGScaleAnim;
	AnimationMultiLerp<float> verifyMenuBGFadeAnim;

	virtual void Activate();
	virtual void Deactivate();

};
#endif