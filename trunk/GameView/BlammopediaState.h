/**
 * BlammopediaState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BLAMMOPEDIASTATE_H__
#define __BLAMMOPEDIASTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class ItemListView;
class Texture;
class Blammopedia;

class BlammopediaState : public DisplayState {
public:
	BlammopediaState(GameDisplay* display);
	~BlammopediaState();

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const int ITEM_NAME_BORDER_SIZE;
    static const int TOTAL_MENU_HEIGHT;
	static const size_t ITEMS_LIST_VIEW_INDEX;
	static const size_t BLOCKS_LIST_VIEW_INDEX;
	static const size_t STATUS_LIST_VIEW_INDEX;

	size_t currListViewIndex;
	std::vector<ItemListView*> listViews;

    AnimationLerp<float> fadeAnimation;

    TextLabel2D selectedItemNameLbl;

    // Menu items and states
    static const int NO_MENU_ITEM_INDEX     = -1;
    static const int ITEMS_MENU_ITEM_INDEX  = 0; 
    static const int BLOCK_MENU_ITEM_INDEX  = 1;
    static const int STATUS_MENU_ITEM_INDEX = 2;
    static const int BACK_MENU_ITEM_INDEX   = 3;
    static const int TOTAL_NUM_MENU_ITEMS   = 4;
    
    static const char* LOCKED_NAME;

    static const Colour SELECTION_COLOUR;
    static const Colour IDLE_COLOUR;
    static const float SELECTION_SCALE;
    static const float NO_SELECTION_DROP_SHADOW_AMT;
    static const float SELECTION_DROP_SHADOW_AMT;
    static const float LABEL_ITEM_GAP;

    int currMenuItemIndex;
    std::vector<TextLabel2D*> blammoMenuLabels;
    //TextLabel2D backMenuItem;
    //TextLabel2D itemListMenuItem;
    //TextLabel2D blockListMenuItem;
    //TextLabel2D statusListMenuItem;

    AnimationLerp<float> itemSelTabAnim;     // Animation for when an item is selected in the menu and the tab to highlight it moves
    AnimationMultiLerp<float> itemHighlightWiggle; // Animation to wiggle the highlighted item

    bool goBackToMainMenu;

	ItemListView* BuildGameItemsListView(Blammopedia* blammopedia) const;
	ItemListView* BuildGameBlockListView(Blammopedia* blammopedia) const;
	ItemListView* BuildStatusEffectListView(Blammopedia* blammopedia) const;

    ItemListView* GetCurrentListView() const;
        
    void GoBackToMainMenu();
    void SetBlammoMenuItemHighlighted(int menuItemIndex);
    void SetBlammoMenuItemSelection();
    void SetBlammoMenuItemDeselection();

    void DrawFadeOverlay(int width, int height, float alpha);
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