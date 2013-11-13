/**
 * ItemListView.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ITEMLISTVIEW_H__
#define __ITEMLISTVIEW_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Colour.h"
#include "../GameControl/GameControl.h"

class Texture;
class Camera;
class TextLabel2D;
class TextLabel2DFixedWidth;
class KeyboardHelperLabel;
class PopupTutorialHint;


class ItemListViewEventHandler {
public:
    ItemListViewEventHandler() {}
    virtual ~ItemListViewEventHandler() {}

    virtual void ItemHighlightedChanged()   = 0;
    virtual void ItemActivated(bool locked) = 0;
    virtual void ItemDeactivated()          = 0;
};


class ItemListView {
public:
	static const int NO_ITEM_SELECTED_INDEX;
    static const int DEFAULT_NUM_ITEMS_PER_ROW;
    static const int MAX_ITEM_WIDTH;
    static const int MIN_ITEM_WIDTH;
    static const int MAX_ITEMS_PER_ROW;
    static const int HORIZ_ITEM_ACTIVATED_BORDER;

    static const float MIN_VERT_BORDER;

	class ListItem {
	public:
		ListItem(const ItemListView* parent, size_t index, const std::string& name,
            const Texture* itemTexture, bool isLocked);
		virtual ~ListItem();

        virtual void Activated() {};
        virtual void Deactivated() {};

        virtual void Tick(double dT);
        virtual void DrawAsActivated(const Camera& camera) = 0;
        virtual void TurnOffNewLabel() {};
        virtual void DrawNewLabel(float bottomLeftX, float bottomLeftY, size_t width, size_t height, float alpha, float scale);
        virtual Colour GetColour() const = 0;
        virtual bool DrawBorderOnActivation() const = 0;

        void DrawSelection(const Camera& camera, size_t width, size_t height, 
                           float alphaOrange, float alphaYellow, float scale, float otherScale);

		void DrawItem(const Camera& camera, size_t width, size_t height, float alpha, float scale);
        void DrawItem(const Camera& camera, size_t width, size_t height, float alpha);

        virtual void SetSelected(bool isSelected);

        size_t GetIndex() const { return this->index; }
        const std::string& GetName() const { return this->name; }
        bool GetIsLocked() const { return this->isLocked; }

	protected:
        const ItemListView* parent;
        size_t index;
        bool isLocked;
        std::string name;
		const Texture* texture;
        float halfSelectionBorderSize;

        AnimationLerp<float> sizeAnimation;
        
        void DrawItemQuadBottomLeft(size_t width, size_t height);
        void DrawItemQuadCenter(float width, float height);
        void DrawNextArrows();

		DISALLOW_COPY_AND_ASSIGN(ListItem);
	};

    class BlammopediaListItem : public ListItem {
	public:
        BlammopediaListItem(const ItemListView* parent, size_t index, const std::string& name, 
            const Texture* itemTexture, PopupTutorialHint* item, bool isLocked, bool hasBeenViewed);
		~BlammopediaListItem();

        void Activated();
        void Deactivated();

        void Tick(double dT);
        void DrawAsActivated(const Camera& camera);
        void TurnOffNewLabel();
        void DrawNewLabel(float bottomLeftX, float bottomLeftY, size_t width, size_t height, float alpha, float scale);
        bool DrawBorderOnActivation() const { return false; }
        Colour GetColour() const { return Colour(1.0f, 1.0f, 1.0f); }

        void SetSelected(bool isSelected);

	private:
        PopupTutorialHint* popup;
        bool isActivated;
        TextLabel2D* newLbl;

        AnimationMultiLerp<Colour> newColourFlashAnimation;

		DISALLOW_COPY_AND_ASSIGN(BlammopediaListItem);
	};

    class BlammopediaListLockedItem : public ListItem {
    public:
        BlammopediaListLockedItem(const ItemListView* parent, size_t index, const Texture* lockedTexture) : 
          ListItem(parent, index, "Locked!", lockedTexture, true) {}
        ~BlammopediaListLockedItem() {}

        void Tick(double dT) { ListItem::Tick(dT); }
        void DrawAsActivated(const Camera&) {}
        bool DrawBorderOnActivation() const { return false; }

        Colour GetColour() const { return Colour(0.5f, 0.5f, 0.5f); }

    private:
        DISALLOW_COPY_AND_ASSIGN(BlammopediaListLockedItem);
    };


    class TutorialListItem : public ListItem {
    public:
        TutorialListItem(const ItemListView* parent, size_t index, const std::string& name,
            PopupTutorialHint* tutorialPopup, const Texture* itemTexture);
        ~TutorialListItem();

        void Activated();
        void Deactivated();

        void Tick(double dT);
        void DrawAsActivated(const Camera& camera);
        bool DrawBorderOnActivation() const { return false; }

        void SetSelected(bool isSelected);

        Colour GetColour() const { return Colour(1.0f, 1.0f, 1.0f); }

    private:
        PopupTutorialHint* tutorialPopup;
        bool isActivated;

        DISALLOW_COPY_AND_ASSIGN(TutorialListItem);
    };


	ItemListView(size_t width, size_t height);
	~ItemListView();

    void SetEventHandler(ItemListViewEventHandler* handler);

	void Draw(double dT, const Camera& camera);
    void DrawPost(const Camera& camera);

    ItemListView::ListItem* AddBlammopediaItem(const std::string& name, const Texture* itemTexture,
        PopupTutorialHint* item, bool hasBeenViewed);
    ItemListView::ListItem* AddLockedBlammopediaItem(const Texture* lockedTexture);
    ItemListView::ListItem* AddTutorialItem(const std::string& name, const Texture* itemTexture, PopupTutorialHint* item);

    void SetSelectedItemIndex(int index);
	ItemListView::ListItem* GetSelectedItem() const;
    bool GetIsItemActivated() const;
    bool GetIsSelectedItemOnLastRow() const;

    size_t GetListWidth() const;
    size_t GetSmallestBorderSize() const;

    void AdjustSizeToHeight(size_t height);

    void ButtonPressed(const GameControl::ActionButton& pressedButton);

private:
	// Location and dimension information
	size_t horizontalBorder, verticalBorder;
	size_t horizontalGap, verticalGap;
	size_t itemPixelWidth, itemPixelHeight;
    const size_t listWidth, listHeight;
    int numItemsPerRow;

    KeyboardHelperLabel* keyLabel;

    ItemListViewEventHandler* eventHandler;

	// Item information
    bool itemIsActivated;
	int selectedItemIndex;
	std::vector<ListItem*> items;	// List items in order of rendering from
									// the top left corner of the list to the bottom right
    std::map<size_t, ListItem*> nonLockedItemsMap;
    
    // Animation of the alphas for the selection indicator
    AnimationMultiLerp<float> selectionAlphaOrangeAnim;   
    AnimationMultiLerp<float> selectionAlphaYellowAnim;   
    // Animation of the scales for the selection indicator
    AnimationMultiLerp<float> selectionBorderAddYellowAnim; 

    AnimationMultiLerp<ColourRGBA> arrowFlashAnim;

    // Animations for selection
    AnimationMultiLerp<float> lockedSelectionAnim;

    // Animations for activation
    AnimationLerp<float> activatedItemGrowAnim;
    AnimationLerp<float> activatedItemFadeAnim;
    AnimationLerp<float> nonActivatedItemsFadeAnim;
    AnimationLerp<float> activatedItemXPicAnim;
    AnimationLerp<float> activatedItemAlphaAnim;
    AnimationMultiLerp<float> pressEscAlphaAnim;

    void GetTranslationToItemAtIndex(int index, float& xTranslation, float& yTranslation);
	size_t AdjustItemWidthAndListLayout(size_t width);
    void MoveSelectionX(bool right);
    void MoveSelectionY(bool up);
    int GetNumItemsOnRow(int rowIdx);

    void ItemActivated();
    void ItemDeactivated();
    void StartLockedAnimation();

    void SetSelection(int index);

    void Tick(double dT);

	DISALLOW_COPY_AND_ASSIGN(ItemListView);
};

inline void ItemListView::SetEventHandler(ItemListViewEventHandler* handler) {
    this->eventHandler = handler;
}

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

inline bool ItemListView::GetIsSelectedItemOnLastRow() const {
    const ItemListView::ListItem* selectedItem = this->GetSelectedItem();
    return (selectedItem->GetIndex() >= this->items.size() - this->numItemsPerRow);
}

inline size_t ItemListView::GetSmallestBorderSize() const {
    return std::min<size_t>(this->horizontalGap, std::min<size_t>(this->verticalGap, std::min<size_t>(this->horizontalBorder, this->verticalBorder)));
}

inline size_t ItemListView::GetListWidth() const {
    return this->listWidth;
}

#endif // __ITEMLISTVIEW_H__