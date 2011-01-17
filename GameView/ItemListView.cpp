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
selectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX), 
itemIsActivated(false), nonActivatedItemsFadeAnim(1), activatedItemFadeAnim(1), activatedItemGrowAnim(1) {
	assert(width > 0);
	
	this->horizontalBorder	= 30;
	this->verticalBorder	= 30;
	this->horizontalGap		= 30;
	this->verticalGap		= 20;
    this->numItemsPerRow    = ItemListView::DEFAULT_NUM_ITEMS_PER_ROW;

	// Figure out an optimal width and height for items and gaps for the given list width and height...
	this->itemPixelWidth		= this->AdjustItemWidthAndListLayout(width);
	this->itemPixelHeight		= this->itemPixelWidth / 2;

    // Setup the selection animation
    std::vector<double> timeValues;
    timeValues.push_back(0.0);
    timeValues.push_back(0.25);
    timeValues.push_back(0.5);
    std::vector<float> alphaValues;
    alphaValues.push_back(0.2f);
    alphaValues.push_back(1.0f);
    alphaValues.push_back(0.2f);
    this->selectionAlphaOrangeAnim.SetLerp(timeValues, alphaValues);
    this->selectionAlphaOrangeAnim.SetRepeat(true);

    timeValues.clear();
    timeValues.push_back(0.0);
    timeValues.push_back(1.0);
    std::vector<float> scaleValues;
    scaleValues.push_back(1.0f);
    scaleValues.push_back(2*this->GetSmallestBorderSize());
    this->selectionBorderAddYellowAnim.SetLerp(timeValues, scaleValues);
    this->selectionBorderAddYellowAnim.SetRepeat(true);

    alphaValues.clear();
    alphaValues.push_back(1.0f);
    alphaValues.push_back(0.2f);
    this->selectionAlphaYellowAnim.SetLerp(timeValues, alphaValues);
    this->selectionAlphaYellowAnim.SetRepeat(true);

    this->lockedSelectionAnim.ClearLerp();
    this->lockedSelectionAnim.SetInterpolantValue(0);

    this->nonActivatedItemsFadeAnim.SetInterpolantValue(1.0f);
    this->activatedItemFadeAnim.SetInterpolantValue(1.0f);
    this->activatedItemGrowAnim.SetInterpolantValue(1.0f);
}

ItemListView::~ItemListView() {
	for (std::vector<ListItem*>::iterator iter = this->items.begin(); iter != this->items.end(); ++iter) {
		ListItem* currItem = *iter;
		delete currItem;
		currItem = NULL;
	}
}

void ItemListView::Draw(double dT, const Camera& camera) {
    this->selectionAlphaOrangeAnim.Tick(dT);
    this->selectionBorderAddYellowAnim.Tick(dT);
    this->selectionAlphaYellowAnim.Tick(dT);
    this->lockedSelectionAnim.Tick(dT);
    this->nonActivatedItemsFadeAnim.Tick(dT);
    this->activatedItemFadeAnim.Tick(dT);
    this->activatedItemGrowAnim.Tick(dT);

	glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);

	float xTranslation = 0;
	float yTranslation = 0;

    if (this->nonActivatedItemsFadeAnim.GetInterpolantValue() > 0.0f) {
	    for (int i = 0; i < static_cast<int>(this->items.size()); i++) {
            if (i == this->selectedItemIndex) { continue; }
            ListItem* currItem = this->items[i];
                
            // Translate the OpenGL matrix to the item's bottom-left
            this->GetTranslationToItemAtIndex(i, xTranslation, yTranslation);
            glPushMatrix();
            glTranslatef(xTranslation, yTranslation, 0);
		    currItem->DrawItem(dT, camera, this->itemPixelWidth, this->itemPixelHeight, 
                               this->nonActivatedItemsFadeAnim.GetInterpolantValue(), 1);
		    glPopMatrix();
	    }
    }

    // Draw the selected item last - that way the selection indicator is on top of all other items!
    float selectedItemAlpha = this->activatedItemFadeAnim.GetInterpolantValue();
    if (this->selectedItemIndex >= 0 && selectedItemAlpha > 0.0f) {
        glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ListItem* currItem = this->GetSelectedItem();
        assert(currItem != NULL);

        // Translate the OpenGL matrix to the item's bottom-left
        this->GetTranslationToItemAtIndex(this->selectedItemIndex, xTranslation, yTranslation);
        glPushMatrix();
        glTranslatef(xTranslation, yTranslation, 0);

        float scalingFactor = this->activatedItemGrowAnim.GetInterpolantValue();

	    // Draw the selection indicator around the item...
        currItem->DrawSelection(dT, camera, this->itemPixelWidth, this->itemPixelHeight, 
            this->selectionAlphaOrangeAnim.GetInterpolantValue() * selectedItemAlpha, 
            this->selectionAlphaYellowAnim.GetInterpolantValue() * selectedItemAlpha,
            this->selectionBorderAddYellowAnim.GetInterpolantValue(), scalingFactor);

        // Draw the item first
        glTranslatef(this->lockedSelectionAnim.GetInterpolantValue(), 0, 0);
        currItem->DrawItem(dT, camera, this->itemPixelWidth, this->itemPixelHeight, 
                           selectedItemAlpha, scalingFactor);
        glPopMatrix();
    }

    if (this->GetIsItemActivated()) {
        // Draw the description, picture, title of the currently activated blammopedia item entry...
        // TODO
    }

    glPopAttrib();
}

// Adds a new list item to the end of the current list 
ItemListView::ListItem* ItemListView::AddItem(const std::string& name, const Texture* itemTexture, bool isLocked) {
	ItemListView::ListItem* newItem = new ItemListView::ListItem(this, name, itemTexture, isLocked);
	this->items.push_back(newItem);
	return newItem;
}

void ItemListView::ButtonPressed(const GameControl::ActionButton& pressedButton) {

    // Movement buttons move the selection around the array of items - this wraps
    switch (pressedButton) {
        case GameControl::UpButtonAction:
            this->MoveSelectionY(true);
            break;
        case GameControl::DownButtonAction:
            this->MoveSelectionY(false);
            break;
        case GameControl::RightButtonAction:
            this->MoveSelectionX(true);
            break;
        case GameControl::LeftButtonAction:
            this->MoveSelectionX(false);
            break;
        case GameControl::EnterButtonAction:
            this->ItemActivated();
            break;
        case GameControl::EscapeButtonAction:
            this->ItemDeactivated();
            break;
        default:
            break;
    }
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

void ItemListView::MoveSelectionX(bool right) {
    if (this->GetSelectedItem() == NULL || this->itemIsActivated) { return; }
    int x = right ? 1 : -1;
    int currRowIndex  = this->selectedItemIndex / this->numItemsPerRow;
    int numItemsOnRow = this->GetNumItemsOnRow(currRowIndex);
    int wrapAroundX   = (numItemsOnRow + (this->selectedItemIndex - currRowIndex * this->numItemsPerRow) + x) % numItemsOnRow;
    this->SetSelection(this->numItemsPerRow * currRowIndex + wrapAroundX);

    assert(this->GetSelectedItem() != NULL);
}

void ItemListView::MoveSelectionY(bool up) {
    if (this->GetSelectedItem() == NULL || this->itemIsActivated) { return; }
    int y = up ? 1 : -1;

    int numRows = (this->items.size() / this->numItemsPerRow) + 1;
    int rowsAllFilledNumItems = (numRows * this->numItemsPerRow);
    int newSelectedIndex = (rowsAllFilledNumItems + this->selectedItemIndex - y * this->numItemsPerRow) % rowsAllFilledNumItems;
    if (newSelectedIndex >= static_cast<int>(this->items.size())) {
        if (up) {
            newSelectedIndex -= this->numItemsPerRow;
        }
        else {
            newSelectedIndex %= this->numItemsPerRow;
        }
    }
    this->SetSelection(newSelectedIndex);

    assert(this->GetSelectedItem() != NULL);
}

int ItemListView::GetNumItemsOnRow(int rowIdx) {
    int temp = static_cast<int>(this->items.size()) - (static_cast<int>(this->numItemsPerRow) * rowIdx);
    return std::min<int>(temp, this->numItemsPerRow);
}

void ItemListView::ItemActivated() {
    ListItem* selectedItem = this->GetSelectedItem();
    if (selectedItem == NULL) { return; }

    if (selectedItem->GetIsLocked()) {
        this->StartLockedAnimation();
    }
    else {
        this->itemIsActivated = true;

        this->nonActivatedItemsFadeAnim.SetLerp(0.0, 0.5, 1.0f, 0.0f);
        this->nonActivatedItemsFadeAnim.SetRepeat(false);
        this->nonActivatedItemsFadeAnim.SetInterpolantValue(1.0f);

        this->activatedItemFadeAnim.SetLerp(0.0, 1.0, 1.0f, 0.0f);
        this->activatedItemFadeAnim.SetRepeat(false);
        this->activatedItemFadeAnim.SetInterpolantValue(1.0f);

        this->activatedItemGrowAnim.SetLerp(0.0, 1.0, 1.0f, 2.0f);
        this->activatedItemGrowAnim.SetRepeat(false);
        this->activatedItemGrowAnim.SetInterpolantValue(1.0f);
    }   
}

void ItemListView::ItemDeactivated() {
    if (this->itemIsActivated) {
        this->itemIsActivated = false;

        // Make all previously non-activated items visible again
        this->nonActivatedItemsFadeAnim.SetLerp(0.5, 1.0f);
        this->nonActivatedItemsFadeAnim.SetRepeat(false);

        this->activatedItemFadeAnim.SetLerp(0.5, 1.0f);
        this->activatedItemFadeAnim.SetRepeat(false);

        this->activatedItemGrowAnim.SetLerp(0.5, 1.0f);
        this->activatedItemGrowAnim.SetRepeat(false);
    }
}

// The item shakes if it's activated when locked...
void ItemListView::StartLockedAnimation() {
    std::vector<double> timeValues;
    timeValues.push_back(0.0);
    timeValues.push_back(0.01);
    timeValues.push_back(0.025);
    timeValues.push_back(0.07);
    timeValues.push_back(0.10);
    timeValues.push_back(0.13);
    timeValues.push_back(0.18);
    timeValues.push_back(0.2);
    std::vector<float> movementValues;
    float maxMove = 0.8f * this->horizontalBorder;
    movementValues.push_back(0.0f);
    movementValues.push_back(maxMove * 0.9f);
    movementValues.push_back(-maxMove * 0.1f);
    movementValues.push_back(-maxMove);
    movementValues.push_back(maxMove * 0.5f);
    movementValues.push_back(maxMove);
    movementValues.push_back(-0.2f * maxMove);
    movementValues.push_back(0.0f);
    this->lockedSelectionAnim.SetLerp(timeValues, movementValues);
    this->lockedSelectionAnim.SetRepeat(false);
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

void ItemListView::SetSelection(int index) {
    ListItem* prevSelection  = this->GetSelectedItem();
    if (prevSelection != NULL) {
        prevSelection->SetSelected(false);
    }
    
    this->selectedItemIndex = index;
    ListItem* newSelection = this->GetSelectedItem();
    if (newSelection != NULL) {
        newSelection->SetSelected(true);
    }
}

// ListItem Methods --------------------------------------------------------------------------

ItemListView::ListItem::ListItem(const ItemListView* parent, const std::string& name, 
                                 const Texture* itemTexture, bool isLocked) : 
name(name), texture(itemTexture), isLocked(isLocked) {
	assert(parent != NULL);
    assert(itemTexture != NULL);

    this->halfSelectionBorderSize = static_cast<float>(parent->GetSmallestBorderSize()) / 2.0f;

    this->sizeAnimation = AnimationLerp<float>(1.0f);
    this->sizeAnimation.SetInterpolantValue(1.0f);
    this->sizeAnimation.SetRepeat(false);

}

ItemListView::ListItem::~ListItem() {
}

void ItemListView::ListItem::DrawSelection(double dT, const Camera& camera, 
                                           size_t width, size_t height, 
                                           float alphaOrange, float alphaYellow, 
                                           float scale, float otherScale) {
	UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);
    
    float sizeAnimationScaling = this->sizeAnimation.GetInterpolantValue() * otherScale;

    glBindTexture(GL_TEXTURE_2D, 0);
 
    glPushMatrix();
    glTranslatef(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f, 0.0f);
    glScalef(sizeAnimationScaling, sizeAnimationScaling, 1);

    glColor4f(1, 1, 0, alphaYellow);
    this->DrawItemQuadCenter(width + scale, height + scale);

    glColor4f(1, 0.65f, 0, alphaOrange);
	this->DrawItemQuadCenter(width + 2*this->halfSelectionBorderSize, height + 2*this->halfSelectionBorderSize);

    glPopMatrix();
}

// Draws this item with its local origin in its bottom left corner with the given width and height
void ItemListView::ListItem::DrawItem(double dT, const Camera& camera, size_t width, size_t height, float alpha, float scale) {
	UNUSED_PARAMETER(camera);
    
    this->sizeAnimation.Tick(dT);
    float totalScale = this->sizeAnimation.GetInterpolantValue() * scale;

	this->texture->BindTexture();

    glPushMatrix();
    
    float halfWidth  = width / 2.0f;
    float halfHeight = height / 2.0f;
    glTranslatef(halfWidth, halfHeight, 0);
    glScalef(totalScale, totalScale, 1);

	glColor4f(1, 1, 1, alpha);
    this->DrawItemQuadCenter(width, height);

	glLineWidth(2.0f);
	glColor4f(0, 0, 0, alpha);
	glBegin(GL_LINE_LOOP);
	glVertex2f(halfWidth, -halfHeight);
	glVertex2f(halfWidth, halfHeight);
	glVertex2f(-halfWidth, halfHeight);
	glVertex2f(-halfWidth, -halfHeight);
	glEnd();
    glPopMatrix();
}

void ItemListView::ListItem::SetSelected(bool isSelected) {
    if (isSelected) {
        this->sizeAnimation.SetLerp(0.2, 1.1f);
        this->sizeAnimation.SetRepeat(false);
    }
    else {
        this->sizeAnimation.SetLerp(0.2, 1.0f);
        this->sizeAnimation.SetRepeat(false);
    }
}

void ItemListView::ListItem::DrawItemQuadBottomLeft(size_t width, size_t height) {
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
}

void ItemListView::ListItem::DrawItemQuadCenter(float width, float height) {
    float halfWidth  = width / 2.0f;
    float halfHeight = height / 2.0f;
	glBegin(GL_QUADS);
	glTexCoord2i(1, 0);
	glVertex2f(halfWidth, -halfHeight);
	glTexCoord2i(1, 1);
	glVertex2f(halfWidth, halfHeight);
	glTexCoord2i(0, 1);
	glVertex2f(-halfWidth, halfHeight);
    glTexCoord2i(0, 0);
	glVertex2f(-halfWidth, -halfHeight);
	glEnd();
}