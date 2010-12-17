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

class Texture;
class Camera;

class ItemListView {
public:
	static const int NO_ITEM_SELECTED_INDEX;
    static const int DEFAULT_NUM_ITEMS_PER_ROW;
    static const int MAX_ITEM_WIDTH;

	class ListItem {
	public:
		ListItem(const std::string& name, const Texture* itemTexture, bool isLocked);
		~ListItem();

		void Draw(double dT, const Camera& camera, size_t width, size_t height);

		//void SetIntData(int data) { this->intData = data; }
		//int GetIntData() const { return this->intData; }
		const std::string& GetName() { return this->name; }

	private:
		//int bottomLeftX, bottomLeftY;	// Relative to parent container
		
		bool isLocked;
		std::string name;
		const Texture* texture;
		//int intData;

		DISALLOW_COPY_AND_ASSIGN(ListItem);
	};


	ItemListView(size_t width);
	~ItemListView();

	void Draw(double dT, const Camera& camera);

	ItemListView::ListItem* AddItem(const std::string& name, const Texture* itemTexture, bool isLocked);
    void SetSelectedItemIndex(int index);
	ItemListView::ListItem* GetSelectedItem() const;

private:
	// Location and dimension information
	size_t horizontalBorder, verticalBorder;
	size_t horizontalGap, verticalGap;
	size_t itemPixelWidth, itemPixelHeight;
    int numItemsPerRow;

	// Item information
	int selectedItemIndex;
	std::vector<ListItem*> items;	// List items in order of rendering from
																// the top left corner of the list to the bottom right
    
    void GetTranslationToItemAtIndex(int index, float& xTranslation, float& yTranslation);
	size_t AdjustItemWidthAndListLayout(size_t width);

	DISALLOW_COPY_AND_ASSIGN(ItemListView);
};

inline void ItemListView::SetSelectedItemIndex(int index) {
    if (index == -1 || (index >= 0 && index < static_cast<int>(this->items.size()))) {
        this->selectedItemIndex = index;
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

#endif // __ITEMLISTVIEW_H__