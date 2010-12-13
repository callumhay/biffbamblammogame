/**
 * ItemListView.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ItemListView.h"

// ItemListView Methods --------------------------------------------------------------------------

const int ItemListView::NO_ITEM_SELECTED_INDEX = -1;

ItemListView::ItemListView(size_t topRightX, size_t topRightY, size_t width) : 
selectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX) {
	assert(width > 0);
	
	this->horizontalBorder	= 10;
	this->verticalBorder		= 10;
	this->horizontalGap			= 26;
	this->verticalGap				= this->horizontalGap / 2;

	// Figure out an optimal width and height for items and gaps for the given list width and height...
	this->itemPixelWidth		= this->GetItemWidthForListWidth(width);
	this->itemPixelHeight		= this->itemPixelWidth / 2;
}

ItemListView::~ItemListView() {
	for (std::vector<ListItem*>::iterator iter = this->items.begin(); iter != this->items.end(); ++iter) {
		ListItem* currItem = *iter;
		delete currItem;
		currItem = NULL;
	}
}

void ItemListView::Draw(double dT, const Camera& camera) {
	glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
	glTranslatef(this->horizontalBorder, this->verticalBorder, 0);
	
	bool isSelected = false;
	size_t xTranslation = 0;
	size_t yTranslation = 0;
	for (size_t i = 0; i < this->items.size(); i++) {
		ListItem* currItem = *iter;

		// If the current item is selected then draw an appropriate selection border around it
		isSelected = (i == this->selectedItemIndex);
		if (isSelected) {
			// TODO
		}		
		
		currItem->Draw(dT, camera, this->itemPixelWidth, this->itemPixelHeight);

		// Figure out the translation required to get to the next item in the list
		if (i % 4 == 0) {
			xTranslation = -(4*this->itemPixelWidth + 3*this->horizontalGap);
			yTranslation = -(this->itemPixelHeight + this->verticalGap);
		}
		else {
			xTranslation = this->itemPixelWidth + this->horizontalGap;
			yTranslation = 0;
		}
		glTranslatef(xTranslation, yTranslation, 0);

	}

	glPopMatrix();
}

// Adds a new list item to the end of the current list 
ItemListView::ListItem* ItemListView::AddItem(const std::string& name, const Texture2D* itemTexture, bool isLocked) {
	ItemListView::ListItem* newItem = new ItemListView::ListItem(name, itemTexture, isLocked);
	this->items.push_back(newItem);
	return newItem;
}

// Calculate the correct item width for a given list widget width
size_t ItemListView::GetItemWidthForListWidth(size_t width) const {
	int itemWidth = (width - 2 * this->horizontalBorder - 3 * this->horizontalGap) / 4;
	if (itemWidth < 0) {
		assert(false);
		return 0;
	}
	return static_cast<size_t>(itemWidth);
}

// ListItem Methods --------------------------------------------------------------------------

ItemListView::ListItem::ListItem(const std::string& name, const Texture2D* itemTexture, bool isLocked) : 
name(name), texture(itemTexture), isLocked(isLocked) {
	assert(itemTexture != NULL);
}

ItemListView::ListItem::~ListItem() {
}

// Draws this item with its local origin in its bottom left corner with the given width and height
void ItemListView::ListItem::Draw(double dT, const Camera& camera, size_t width, size_t height) {
	this->texture->BindTexture();
	
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2i(0, 0);
	glTexCoord2i(0, 1);
	glVertex2i(0, height);
	glTexCoord2i(1, 1);
	glVertex2i(width, height);
	glTexCoord2i(1, 0);
	glVertex2i(width, 0);
	glEnd();

	glLineWidth(2.0f);
	glColor4f(0,0,0,1);
	glBegin(GL_LINE_LOOP);
	glTexCoord2i(0, 0);
	glVertex2i(0, 0);
	glTexCoord2i(0, 1);
	glVertex2i(0, height);
	glTexCoord2i(1, 1);
	glVertex2i(width, height);
	glTexCoord2i(1, 0);
	glVertex2i(width, 0);
	glEnd();
}