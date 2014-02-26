/**
 * GameMenuItem.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GAMEMENUITEM_H__
#define __GAMEMENUITEM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

#include "../GameControl/GameControl.h"

#include "../GameSound/GameSound.h"

#include "IGameMenu.h"

class GameMenu;
class GameSubMenu;

class GameMenuItemEventHandler {
public:
    GameMenuItemEventHandler() {}
    virtual ~GameMenuItemEventHandler() {}

    virtual void MenuItemActivated()   {};
    virtual void MenuItemDeactivated() {};
    virtual void MenuItemScrolled()    {};
    virtual void MenuItemConfirmed()   {};
    virtual void MenuItemCancelled()   {};
};

class SelectionListEventHandlerWithSound : public GameMenuItemEventHandler {
public:
    SelectionListEventHandlerWithSound(GameSound* sound) : GameMenuItemEventHandler(), sound(sound) {}
    virtual ~SelectionListEventHandlerWithSound() {}

    virtual void MenuItemActivated() { this->sound->PlaySound(GameSound::MenuItemEnteredEvent, false); }
    virtual void MenuItemScrolled()  { this->sound->PlaySound(GameSound::MenuSelectionItemScrolledEvent, false); }
    virtual void MenuItemConfirmed() { this->sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false); }
    virtual void MenuItemCancelled() { this->sound->PlaySound(GameSound::MenuItemCancelEvent, false); }

private:
    GameSound* sound;
};

class ScrollerItemEventHandlerWithSound : public GameMenuItemEventHandler {
public:
    ScrollerItemEventHandlerWithSound(GameSound* sound) : GameMenuItemEventHandler(), sound(sound), lastScrollingSoundID(INVALID_SOUND_ID) {}
    virtual ~ScrollerItemEventHandlerWithSound() {}

    virtual void MenuItemActivated() { this->sound->PlaySound(GameSound::MenuItemEnteredEvent, false); }
    virtual void MenuItemScrolled()  { 
        if (!this->sound->IsSoundPlaying(this->lastScrollingSoundID)) {
            this->lastScrollingSoundID = this->sound->PlaySound(GameSound::MenuScrollerItemScrolledEvent, false);
        }
    }
    virtual void MenuItemConfirmed() { this->sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false); }
    virtual void MenuItemCancelled() { this->sound->PlaySound(GameSound::MenuItemCancelEvent, false); }

private:
    GameSound* sound;
    SoundID lastScrollingSoundID;
};

class VerifyMenuEventHandlerWithSound : public GameMenuItemEventHandler {
public:
    VerifyMenuEventHandlerWithSound(GameSound* sound) : GameMenuItemEventHandler(), sound(sound) {}
    virtual ~VerifyMenuEventHandlerWithSound() {}

    virtual void MenuItemActivated()   { this->sound->PlaySound(GameSound::MenuOpenSubMenuWindowEvent, false); }
    virtual void MenuItemDeactivated() { /* Sounds don't really work well here... need an animation to make it work */ }
    virtual void MenuItemScrolled()    { this->sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false); }
    virtual void MenuItemConfirmed()   { this->sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false); }
    virtual void MenuItemCancelled()   { this->sound->PlaySound(GameSound::MenuItemCancelEvent, false); }

protected:
    GameSound* sound;
};

/**
 * An item in a game menu that may be highlighted and selected
 * by the user.
 */
class GameMenuItem : public IGameMenu {
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

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude) {
		UNUSED_PARAMETER(pressedButton);
        UNUSED_PARAMETER(magnitude);
	}
	virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) {
		UNUSED_PARAMETER(releasedButton);
	}

	unsigned int GetHeight() const;
	virtual float GetWidth(bool useMax = true) const;
	
	const TextLabel2D* GetCurrLabel() const {
		return this->currLabel;
	}

	void ToggleWiggleAnimationOn(float amplitude, float frequency);
	void ToggleWiggleAnimationOff();

	void SetParent(IGameMenu* parent) { this->parent = parent; }

	inline void SetSize(bool isLarge) {
		if (isLarge) {
			this->currLabel = &this->lgTextLabel;
		}
		else {
			this->currLabel = &this->smTextLabel;
		}
	}
    inline void SetText(const std::string& text) {
        this->smTextLabel.SetText(text);
        this->lgTextLabel.SetText(text);
    }

    virtual void Activate() { if (this->eventHandler != NULL) { this->eventHandler->MenuItemActivated(); } this->isActivated = true; };
    virtual void Deactivate() { if (this->eventHandler != NULL) { this->eventHandler->MenuItemDeactivated(); } this->isActivated = false; };

	GameSubMenu* GetSubMenu() { return this->subMenu; }

	inline void SetTextColour(const Colour& c) {
		this->currLabel->SetColour(c);
	}
	inline void SetTextColour(const ColourRGBA& c) {
		this->currLabel->SetColour(c);
	}
	inline const ColourRGBA& GetTextColour() const {
		return this->currLabel->GetColour();
	}

    inline bool GetIsActivated() const {
        return this->isActivated;
    }

    virtual bool IsLeftRightScrollable() const { return false; }

protected:
	IGameMenu* parent;
	GameSubMenu* subMenu;
	TextLabel2D* currLabel;
	TextLabel2D smTextLabel, lgTextLabel;
	AnimationMultiLerp<float> wiggleAnimation;
	GameMenuItemEventHandler* eventHandler;
    bool isActivated;

    virtual void ActivatedMenuItemChanged()  {}
    virtual void DeactivateSelectedMenuItem() {}

private:
    DISALLOW_COPY_AND_ASSIGN(GameMenuItem);
};

/**
 * A menu item with a flashy label appended to it.
 */
class GameMenuItemWithFlashLabel : public GameMenuItem {
public:
    GameMenuItemWithFlashLabel(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, 
        const TextLabel2D& flashLabel, GameSubMenu* subMenu);
    ~GameMenuItemWithFlashLabel();

    void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);

private:
    AnimationMultiLerp<Colour> flashAnimation;
    TextLabel2D flashLbl;
};

/**
 * A specific kind of list item - one that offers a selection of options
 * that can be scrolled though and selected.
 */
class SelectionListMenuItem : public GameMenuItem {
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
	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
    float GetWidth(bool useMax) const;
	void Activate();

    bool IsLeftRightScrollable() const { return (this->selectionList.size() > 1); }

protected:
	static const int NO_SELECTION = -1;

	static const float INTERIOR_PADDING;
	static const float SELECTION_ARROW_WIDTH;

	int previouslySelectedIndex;                // The index that was selected before this item was activated and possibly changed
	int selectedIndex;                          // Index in the list that's currently selected 
	std::vector<std::string> selectionList;		// List of items that can be selected
	std::string baseLabelStr;                   // The label of this item (this text always appears on the item)
	
	float maxWidth;	// The maximum width of this menu item

	void DrawSelectionArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing);
    
    virtual void EscapeFromMenuItem(bool saveChange);

private:
    DISALLOW_COPY_AND_ASSIGN(SelectionListMenuItem);
};

class VerifyMenuItem;

class SelectionListMenuItemWithVerify : public SelectionListMenuItem {
public:
    SelectionListMenuItemWithVerify(const TextLabel2D& smLabel, const TextLabel2D& lgLabel,
        const std::vector<std::string>& items, VerifyMenuItem* verifyMenu);
    ~SelectionListMenuItemWithVerify();

    void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
    void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
    void ButtonReleased(const GameControl::ActionButton& releasedButton);

private:
    VerifyMenuItem* verifyMenu;
    bool verifyMenuActive;
    
    void EscapeFromMenuItem(bool saveChange);
    void DeactivateSelectedMenuItem();

    DISALLOW_COPY_AND_ASSIGN(SelectionListMenuItemWithVerify);
};

/**
 * A menu item that allows the user to change the value from some minimum to some maximum
 * as a filled bar.
 */
class AmountScrollerMenuItem : public GameMenuItem {
public:
	AmountScrollerMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, 
		float minValue, float maxValue, float currentValue, float incrementAmt);
	~AmountScrollerMenuItem();

	inline float GetScrollerValue() const {
		return this->currentValue;
	}

	// Allows the widget to constantly send update events everytime it is changed while active
	// (not just after it is deactivated/set).
	void SetConstantChangeFeedback(bool alwaysUpdate) {
		this->alwaysSendChangeUpdates = alwaysUpdate;
	}

	// Inherited from GameMenuItem
	void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    float GetWidth(bool useMax) const { UNUSED_PARAMETER(useMax); return this->maxWidth; }

    bool IsLeftRightScrollable() const { return true; }

private:
	static const float INTERIOR_PADDING_TEXT_ARROWS;
	static const float INTERIOR_PADDING_ARROWS_SCROLLER;
	static const float SCROLLER_ARROW_WIDTH;
	static const float SCROLLER_WIDTH;

	static const double INCREMENT_PULSE_TIME;

	bool isActive;

	float minValue;									// Minimum value the scroller can assume
	float maxValue;									// Maximum value the scroller can assume
	float currentValue;							// Current value of the scroller
	float previouslySelectedValue;	// The value selected before this item was activated
	
	float incrementAmt;							// Amount that the scroller increments/decrements by with each change
	float baseIncrementAmt;					// The base amount the scroller changes by

	bool alwaysSendChangeUpdates;
	float maxWidth;
	std::string baseLabelStr;	// The label of this item (this text always appears on the item)

	bool increaseValueButtonPressed;
	bool decreaseValueButtonPressed;

	void ChangeScrollerValue(float changeAmt, bool signalEvent);
	void DrawScrollerArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing);
	

	void Activate() {
        GameMenuItem::Activate();
		this->isActive = true; 
		this->previouslySelectedValue = this->currentValue;
	}
	void Deactivate() { 
        GameMenuItem::Deactivate();
        this->isActive = false;
    }

    void EscapeFromMenuItem(bool saveChange);

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

	void Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight);
	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);

	void Activate();
	void Deactivate();

private:
	static const float VERIFY_MENU_HPADDING;
	static const float VERIFY_MENU_VPADDING;
	static const float VERIFY_MENU_OPTION_HSPACING;
	static const float VERIFY_MENU_OPTION_VSPACING;
	static const float VERIFY_MENU_OPTION_WOBBLE_FREQ;	

	bool verifyMenuActive;                     // Whether or not the verify menu is active or not
	float verifyMenuWidth, verifyMenuHeight;   // Dimensions of the menu
    TextLabel2DFixedWidth descriptionLabel;
	TextLabel2D confirmLabel, cancelLabel;
	VerifyMenuOption selectedOption;           // The option currently selecteded/highlighted in the verify menu
	
	const TextureFontSet* verifyDescFont;
	const TextureFontSet* verifyIdleFont;
	const TextureFontSet* verifySelFont;

	Colour randomMenuBGColour;			    // Random colour assigned to the background of the verify menu
	Colour idleColour, selectionColour;		// Colours used for confirm/cancel items when idle and selected
	
	AnimationMultiLerp<float> optionItemWiggleAnim;	// Wiggle animation for verify menu items
	AnimationMultiLerp<float> verifyMenuBGScaleAnim;
	AnimationMultiLerp<float> verifyMenuBGFadeAnim;
};

#endif