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

#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Camera.h"

// ItemListView Methods --------------------------------------------------------------------------

const int ItemListView::NO_ITEM_SELECTED_INDEX      = -1;
const int ItemListView::DEFAULT_NUM_ITEMS_PER_ROW   = 4;
const int ItemListView::MAX_ITEM_WIDTH              = 200;

ItemListView::ItemListView(size_t width) : 
selectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX) {
	assert(width > 0);
	
	this->horizontalBorder	= 15;
	this->verticalBorder	= 15;
	this->horizontalGap		= 30;
	this->verticalGap		= 20;
    this->numItemsPerRow    = ItemListView::DEFAULT_NUM_ITEMS_PER_ROW;

	// Figure out an optimal width and height for items and gaps for the given list width and height...
	this->itemPixelWidth		= this->AdjustItemWidthAndListLayout(width);
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
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	
	bool isSelected = false;
	float xTranslation = 0;
	float yTranslation = 0;
	for (int i = 0; i < static_cast<int>(this->items.size()); i++) {
		ListItem* currItem = this->items[i];

		// If the current item is selected then draw an appropriate selection border around it
		isSelected = (i == this->selectedItemIndex);
		if (isSelected) {
			// Draw the selection indicator around the item...

		}		
		
        this->GetTranslationToItemAtIndex(i, xTranslation, yTranslation);

        glPushMatrix();
        glTranslatef(xTranslation, yTranslation, 0);
		currItem->Draw(dT, camera, this->itemPixelWidth, this->itemPixelHeight);
		glPopMatrix();
	}

    glPopAttrib();
}

// Adds a new list item to the end of the current list 
ItemListView::ListItem* ItemListView::AddItem(const std::string& name, const Texture* itemTexture, bool isLocked) {
	ItemListView::ListItem* newItem = new ItemListView::ListItem(name, itemTexture, isLocked);
	this->items.push_back(newItem);
	return newItem;
}

// Calculate the correct item width for a given list widget width
size_t ItemListView::AdjustItemWidthAndListLayout(size_t width) {
	int itemWidth = (width - 2 * this->horizontalBorder - (this->numItemsPerRow -1) * this->horizontalGap) / this->numItemsPerRow;
	if (itemWidth < 0) {
		assert(false);
		return 0;
	}
    
    // Make sure the item width does not exceed the maximum - if it does then we need to scale the item width down...
    while (itemWidth > MAX_ITEM_WIDTH) {
        this->numItemsPerRow++;
        itemWidth = (width - 2 * this->horizontalBorder - (this->numItemsPerRow -1) * this->horizontalGap) / this->numItemsPerRow;
    }

	return static_cast<size_t>(itemWidth);
}

void ItemListView::GetTranslationToItemAtIndex(int index, float& xTranslation, float& yTranslation) {
    assert(index >= 0 && index < static_cast<int>(this->items.size()));
    int numItemsIntoRow = index % this->numItemsPerRow;
    xTranslation  = (this->horizontalBorder + (numItemsIntoRow * this->itemPixelWidth) + 
                     numItemsIntoRow * static_cast<float>(this->horizontalGap));
    
    int numRowsDown    = (index / this->numItemsPerRow) + 1;
    yTranslation = -(this->verticalBorder + (numRowsDown * this->itemPixelHeight) +
                     std::max<float>(0.0f, (numRowsDown-1) * static_cast<float>(this->verticalGap)));
}

// ListItem Methods --------------------------------------------------------------------------

ItemListView::ListItem::ListItem(const std::string& name, const Texture* itemTexture, bool isLocked) : 
name(name), texture(itemTexture), isLocked(isLocked) {
	assert(itemTexture != NULL);
}

ItemListView::ListItem::~ListItem() {
}

// Draws this item with its local origin in its bottom left corner with the given width and height
void ItemListView::ListItem::Draw(double dT, const Camera& camera, size_t width, size_t height) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(camera);

	this->texture->BindTexture();

	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 0);
	glVertex2i(width, 0);
	glTexCoord2i(1, 1);
	glVertex2i(width, height);
	glTexCoord2i(0, 1);
	glVertex2i(0, height);
    glTexCoord2i(0, 0);
	glVertex2i(0, 0);
	glEnd();

	glLineWidth(2.0f);
	glColor4f(0,0,0,1);
	glBegin(GL_LINE_LOOP);
	glVertex2i(width, 0);
	glVertex2i(width, height);
	glVertex2i(0, height);
	glVertex2i(0, 0);
	glEnd();
}