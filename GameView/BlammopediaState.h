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
	static const size_t ITEMS_LIST_VIEW_INDEX;
	static const size_t BLOCKS_LIST_VIEW_INDEX;
	static const size_t STATUS_LIST_VIEW_INDEX;

	size_t currListViewIndex;
	std::vector<ItemListView*> listViews;

    TextLabel2D selectedItemNameLbl;
    AnimationLerp<float> fadeAnimation;

	ItemListView* BuildGameItemsListView(Blammopedia* blammopedia) const;
	ItemListView* BuildGameBlockListView(Blammopedia* blammopedia) const;
	ItemListView* BuildStatusEffectListView(Blammopedia* blammopedia) const;

    ItemListView* GetCurrentListView() const;
        
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