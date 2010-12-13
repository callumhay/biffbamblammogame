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

class ItemListView {
public:
	static const int NO_ITEM_SELECTED_INDEX;

	class ListItem {
		ListItem(const std::string& name, const Texture2D* itemTexture, bool isLocked);
		~ListItem();

		void Draw(double dT, const Camera& camera, size_t width, size_t height);

		//void SetIntData(int data) { this->intData = data; }
		//int GetIntData() const { return this->intData; }
		const std::string& GetName() { return this->name; }

	private:
		size_t bottomLeftX, bottomLeftY;	// Relative to parent container
		
		bool isLocked;
		std::string name;
		const Texture2D* texture;
		//int intData;

		DISALLOW_COPY_AND_ASSIGN(ListItem);
	};


	ItemListView(size_t topRightX, size_t topRightY, size_t width);
	~ItemListView();

	void Draw(double dT, const Camera& camera);

	ItemListView::ListItem* AddItem(const std::string& name, const Texture2D* itemTexture, bool isLocked);
	ItemListView::ListItem* GetSelectedItem() const;

private:

	// Location and dimension information
	size_t horizontalBorder, verticalBorder;
	size_t horizontalGap, verticalGap;
	size_t itemPixelWidth, itemPixelHeight;

	// Item information
	int selectedItemIndex;
	std::vector<ListItem*> items;	// List items in order of rendering from
																// the top left corner of the list to the bottom right

	size_t GetItemWidthForListWidth(size_t width) const;

	DISALLOW_COPY_AND_ASSIGN(ItemListView);
};

// Gets the currently selected item in this list view
inline ItemListView::ListItem* ItemListView::GetSelectedItem() const {
	if (this->selectedItemIndex >= this->items.size() || this->selectedItemIndex < 0) {
		return NULL;
	}
	return this->items[this->selectedItemIndex];
}

#endif // __ITEMLISTVIEW_H__