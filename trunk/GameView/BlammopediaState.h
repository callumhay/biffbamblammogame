/**
 * BlammopediaState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BLAMMOPEDIASTATE_H__
#define __BLAMMOPEDIASTATE_H__

#include "DisplayState.h"
#include "ItemListView.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "../Blammopedia.h"

class Texture;

class BlammopediaState : public DisplayState {
public:
	BlammopediaState(GameDisplay* display);
	~BlammopediaState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const int ITEM_NAME_BORDER_SIZE;
    static const int TOTAL_MENU_HEIGHT;

	size_t currListViewIndex;
	std::vector<ItemListView*> listViews;

    AnimationLerp<float> fadeAnimation;

    TextLabel2D selectedItemNameLbl;

    Texture2D* starryBG;

    std::map<ItemListView::ListItem*, Blammopedia::Entry*> itemToEntryMap;
    //std::map<ItemListView::ListItem*, PopupTutorialHint*> itemToTutorialMap;

    // Menu items and states
    static const int NO_MENU_ITEM_INDEX       = -1;
    static const int GAMEPLAY_MENU_ITEM_INDEX = 0;
    static const int ITEMS_MENU_ITEM_INDEX    = 1; 
    static const int BLOCK_MENU_ITEM_INDEX    = 2;
    static const int BACK_MENU_ITEM_INDEX     = 3;
    static const int TOTAL_NUM_MENU_ITEMS     = 4;
    
    static const char* LOCKED_NAME;

    static const Colour SELECTION_COLOUR;
    static const Colour IDLE_COLOUR;
    static const float SELECTION_SCALE;
    static const float NO_SELECTION_DROP_SHADOW_AMT;
    static const float SELECTION_DROP_SHADOW_AMT;
    static const float LABEL_ITEM_GAP;

    int currMenuItemIndex;
    std::vector<TextLabel2D*> blammoMenuLabels;

    AnimationLerp<float> itemSelTabAnim;     // Animation for when an item is selected in the menu and the tab to highlight it moves
    AnimationMultiLerp<float> itemHighlightWiggle; // Animation to wiggle the highlighted item

    bool goBackToMainMenu;

    ItemListView* BuildGameplayListView();
	ItemListView* BuildGameItemsListView(Blammopedia* blammopedia);
	ItemListView* BuildGameBlockListView(Blammopedia* blammopedia);

    ItemListView* GetCurrentListView() const;
        
    void GoBackToMainMenu();
    void SetBlammoMenuItemHighlighted(int menuItemIndex);
    void SetBlammoMenuItemSelection();
    void SetBlammoMenuItemDeselection();

	DISALLOW_COPY_AND_ASSIGN(BlammopediaState);
};

inline void BlammopediaState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType BlammopediaState::GetType() const {
	return DisplayState::BlammopediaMenu;
}

inline ItemListView* BlammopediaState::GetCurrentListView() const {
    if (this->currListViewIndex >= this->listViews.size()) {
        return NULL;
    }
    return this->listViews[this->currListViewIndex];
}

#endif // __BLAMMOPEDIASTATE_H__