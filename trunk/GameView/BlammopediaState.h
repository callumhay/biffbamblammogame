/**
 * BlammopediaState.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __BLAMMOPEDIASTATE_H__
#define __BLAMMOPEDIASTATE_H__

#include "DisplayState.h"
#include "ItemListView.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameSound/SoundCommon.h"

#include "../Blammopedia.h"

class Texture;

class BlammopediaState : public DisplayState {
public:
	BlammopediaState(GameDisplay* display);
	~BlammopediaState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const int ITEM_NAME_BORDER_SIZE;
    static const int TOTAL_MENU_HEIGHT;

    class BlammopediaListEventHandler : public ItemListViewEventHandler {
    public:
        BlammopediaListEventHandler(BlammopediaState* state) : state(state) {}
        ~BlammopediaListEventHandler() {}

        void ItemHighlightedChanged();
        void ItemActivated(bool locked);
        void ItemDeactivated();

    private:
        BlammopediaState* state;
    };

	size_t currListViewIndex;
	std::vector<ItemListView*> listViews;

    AnimationLerp<float> fadeAnimation;

    //TextLabel2D selectedItemNameLbl;

    std::map<ItemListView::ListItem*, Blammopedia::Entry*> itemToEntryMap;
    //std::map<ItemListView::ListItem*, PopupTutorialHint*> itemToTutorialMap;

    SoundID bgLoopedSoundID;

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

    BlammopediaListEventHandler* listEventHandler;

    ItemListView* BuildTutorialListView();
	ItemListView* BuildGameItemsListView(Blammopedia* blammopedia);
	ItemListView* BuildGameBlockListView(Blammopedia* blammopedia);

    ItemListView* GetCurrentListView() const;
        
    void GoBackToMainMenu();
    void SetBlammoMenuItemHighlighted(int menuItemIndex, bool playSound = true);
    void SetBlammoMenuItemSelection(bool playSound = true);
    void SetBlammoMenuItemDeselection(bool playSound = true);

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