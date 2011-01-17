/**
 * ItemListView.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ITEMLISTVIEW_H__
#define __ITEMLISTVIEW_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"

#include "../GameControl/GameControl.h"

class Texture;
class Camera;

class ItemListView {
public:
	static const int NO_ITEM_SELECTED_INDEX;
    static const int DEFAULT_NUM_ITEMS_PER_ROW;
    static const int MAX_ITEM_WIDTH;

	class ListItem {
	public:
		ListItem(const ItemListView* parent, const std::string& name, const Texture* itemTexture, bool isLocked);
		~ListItem();

        void DrawSelection(double dT, const Camera& camera, size_t width, 
            size_t height, float alphaOrange, float alphaYellow, float scale, float otherScale);
		void DrawItem(double dT, const Camera& camera, size_t width, size_t height, float alpha, float scale);

        void SetSelected(bool isSelected);

		//void SetIntData(int data) { this->intData = data; }
		//int GetIntData() const { return this->intData; }
		const std::string& GetName() const { return this->name; }
        bool GetIsLocked() const { return this->isLocked; }

	private:
		bool isLocked;
		std::string name;
		const Texture* texture;
        float halfSelectionBorderSize;
		//int intData;

        AnimationLerp<float> sizeAnimation;

        void DrawItemQuadBottomLeft(size_t width, size_t height);
        void DrawItemQuadCenter(float width, float height);

		DISALLOW_COPY_AND_ASSIGN(ListItem);
	};


	ItemListView(size_t width);
	~ItemListView();

	void Draw(double dT, const Camera& camera);

	ItemListView::ListItem* AddItem(const std::string& name, const Texture* itemTexture, bool isLocked);
    void SetSelectedItemIndex(int index);
	ItemListView::ListItem* GetSelectedItem() const;
    bool GetIsItemActivated() const;

    size_t GetSmallestBorderSize() const;

    void ButtonPressed(const GameControl::ActionButton& pressedButton);

private:
	// Location and dimension information
	size_t horizontalBorder, verticalBorder;
	size_t horizontalGap, verticalGap;
	size_t itemPixelWidth, itemPixelHeight;
    int numItemsPerRow;

	// Item information
    bool itemIsActivated;
	int selectedItemIndex;
	std::vector<ListItem*> items;	// List items in order of rendering from
									// the top left corner of the list to the bottom right
    
    // Animation of the alphas for the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    // Animation of the scales for the selection indicator
    AnimationMultiLerp<float> selectionBorderAddYellowAnim; 

    // Animations for selection
    AnimationMultiLerp<float> lockedSelectionAnim;

    // Animations for activation
    AnimationLerp<float> activatedItemGrowAnim;
    AnimationLerp<float> activatedItemFadeAnim;
    AnimationLerp<float> nonActivatedItemsFadeAnim;

    //AnimationLerp<float> blackBorderAnim;
    //AnimationLerp<float> revealAnim;

    void GetTranslationToItemAtIndex(int index, float& xTranslation, float& yTranslation);
	size_t AdjustItemWidthAndListLayout(size_t width);
    void MoveSelectionX(bool right);
    void MoveSelectionY(bool up);
    int GetNumItemsOnRow(int rowIdx);

    void ItemActivated();
    void ItemDeactivated();
    void StartLockedAnimation();

    void SetSelection(int index);

	DISALLOW_COPY_AND_ASSIGN(ItemListView);
};

inline void ItemListView::SetSelectedItemIndex(int index) {
    if (index == -1 || (index >= 0 && index < static_cast<int>(this->items.size()))) {
        this->SetSelection(index);
    }
    else {
        assert(false);
    }
}

// Gets the currently selected item in this list view
inline ItemListView::ListItem* ItemListView::GetSelectedItem() const {
	if (this->selectedItemIndex >= static_cast<int>(this->items.size()) || this->selectedItemIndex < 0) {
		return NULL;
	}
	return this->items[this->selectedItemIndex];
}

inline bool ItemListView::GetIsItemActivated() const {
    return this->itemIsActivated;
}

inline size_t ItemListView::GetSmallestBorderSize() const {
    return std::min<size_t>(this->horizontalGap, std::min<size_t>(this->verticalGap, std::min<size_t>(this->horizontalBorder, this->verticalBorder)));
}

#endif // __ITEMLISTVIEW_H__