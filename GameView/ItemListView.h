/**
 * ItemListView.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

class ItemListView {
public:
	static const int NO_ITEM_SELECTED_INDEX;
    static const int DEFAULT_NUM_ITEMS_PER_ROW;
    static const int MAX_ITEM_WIDTH;
    static const int MIN_ITEM_WIDTH;
    static const int MAX_ITEMS_PER_ROW;
    static const int HORIZ_ITEM_ACTIVATED_BORDER;
    static const int BLACK_BORDER_HEIGHT;

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

        void DrawSelection(const Camera& camera, size_t width, size_t height, 
                           float alphaOrange, float alphaYellow, float scale, float otherScale);

		void DrawItem(const Camera& camera, size_t width, size_t height, float alpha, float scale);
        void DrawItem(const Camera& camera, size_t width, size_t height, float alpha);

        void SetSelected(bool isSelected);

        size_t GetIndex() const { return this->index; }
        TextLabel2D* GetNameLbl() const { return this->nameLbl; }
        bool GetIsLocked() const { return this->isLocked; }

	protected:
        const ItemListView* parent;
        size_t index;
        bool isLocked;
		const Texture* texture;
        TextLabel2D* nameLbl;
        float halfSelectionBorderSize;

        AnimationLerp<float> sizeAnimation;
        
        void DrawItemQuadBottomLeft(size_t width, size_t height);
        void DrawItemQuadCenter(float width, float height);

		DISALLOW_COPY_AND_ASSIGN(ListItem);
	};

    class BlammopediaListItem : public ListItem {
	public:
        BlammopediaListItem(const ItemListView* parent, size_t index, const std::string& name,
                 const std::string& description, const std::string& finePrint, const Colour& colour,
                 const Texture* itemTexture, bool isLocked, bool hasBeenViewed);
		~BlammopediaListItem();

        void Tick(double dT);
        void DrawAsActivated(const Camera& camera);
        void TurnOffNewLabel();
        void DrawNewLabel(float bottomLeftX, float bottomLeftY, size_t width, size_t height, float alpha, float scale);

        Colour GetColour() const { return this->colour; }
        TextLabel2DFixedWidth* GetDescriptionLbl() const { return this->descriptionLbl; }
        TextLabel2DFixedWidth* GetFinePrintLbl() const { return this->finePrintLbl; }

	private:
        Colour colour;
        TextLabel2DFixedWidth* descriptionLbl;
        TextLabel2DFixedWidth* finePrintLbl;
        TextLabel2D* newLbl;

        AnimationMultiLerp<Colour> newColourFlashAnimation;

		DISALLOW_COPY_AND_ASSIGN(BlammopediaListItem);
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

        Colour GetColour() const { return Colour(1.0f, 1.0f, 1.0f); }

    private:
        PopupTutorialHint* tutorialPopup;

        DISALLOW_COPY_AND_ASSIGN(TutorialListItem);
    };


	ItemListView(size_t width, size_t height);
	~ItemListView();


	void Draw(double dT, const Camera& camera);
    void DrawPost(const Camera& camera);

    ItemListView::ListItem* AddBlammopediaItem(const std::string& name, const std::string& description, const std::string& finePrint,
                                    const Colour& colour, const Texture* itemTexture, bool isLocked, bool hasBeenViewed);
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
    AnimationLerp<float> blackBorderAnim;
    //AnimationLerp<float> revealAnim;
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