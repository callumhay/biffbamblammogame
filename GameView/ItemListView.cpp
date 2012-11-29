/**
 * ItemListView.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ItemListView.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"
#include "KeyboardHelperLabel.h"
#include "PopupTutorialHint.h"

#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/TextLabel.h"

#include "../ResourceManager.h"

// ItemListView Methods --------------------------------------------------------------------------

const int ItemListView::NO_ITEM_SELECTED_INDEX      = -1;
const int ItemListView::DEFAULT_NUM_ITEMS_PER_ROW   = 4;
const int ItemListView::MAX_ITEM_WIDTH              = 512;
const int ItemListView::MIN_ITEM_WIDTH              = 70;
const int ItemListView::MAX_ITEMS_PER_ROW           = 8;

const int ItemListView::BLACK_BORDER_HEIGHT         = 100;
const int ItemListView::HORIZ_ITEM_ACTIVATED_BORDER = 50;

ItemListView::ItemListView(size_t width, size_t height) : listWidth(width), listHeight(height),
selectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX), 
itemIsActivated(false), nonActivatedItemsFadeAnim(1), activatedItemFadeAnim(1), 
activatedItemGrowAnim(1), blackBorderAnim(0.0f), activatedItemXPicAnim(width + 1), keyLabel(NULL),
activatedItemAlphaAnim(0.0f), pressEscAlphaAnim(0.0f) {
	assert(width > 0);
	
    this->keyLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));

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
    this->blackBorderAnim.SetInterpolantValue(0.0f);
    this->activatedItemXPicAnim.SetInterpolantValue(width + this->itemPixelWidth);
    this->activatedItemAlphaAnim.SetInterpolantValue(0.0f);
    this->pressEscAlphaAnim.SetInterpolantValue(0.0f);

    timeValues.clear();
    timeValues.push_back(0.0);
    timeValues.push_back(0.75);
    timeValues.push_back(1.5);
    std::vector<ColourRGBA> colourValues;
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 0.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 0.0f));

    this->arrowFlashAnim.SetLerp(timeValues, colourValues);
    this->arrowFlashAnim.SetRepeat(true);

}

ItemListView::~ItemListView() {
	for (std::vector<ListItem*>::iterator iter = this->items.begin(); iter != this->items.end(); ++iter) {
		ListItem* currItem = *iter;
		delete currItem;
		currItem = NULL;
	}

    delete this->keyLabel;
    this->keyLabel = NULL;
}

void ItemListView::Tick(double dT) {
    this->selectionAlphaOrangeAnim.Tick(dT);
    this->selectionBorderAddYellowAnim.Tick(dT);
    this->selectionAlphaYellowAnim.Tick(dT);
    this->lockedSelectionAnim.Tick(dT);
    this->nonActivatedItemsFadeAnim.Tick(dT);
    this->activatedItemFadeAnim.Tick(dT);
    this->activatedItemGrowAnim.Tick(dT);
    this->blackBorderAnim.Tick(dT);
    this->activatedItemXPicAnim.Tick(dT);
    this->activatedItemAlphaAnim.Tick(dT);
    this->pressEscAlphaAnim.Tick(dT);
    this->arrowFlashAnim.Tick(dT);

    for (int i = 0; i < static_cast<int>(this->items.size()); i++) {
        ListItem* currItem = this->items[i];
        currItem->Tick(dT);
    }
}

void ItemListView::Draw(double dT, const Camera& camera) {
    this->Tick(dT);

	glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		    currItem->DrawItem(camera, this->itemPixelWidth, this->itemPixelHeight, 
                               this->nonActivatedItemsFadeAnim.GetInterpolantValue(), 1);
		    glPopMatrix();
            currItem->DrawNewLabel(xTranslation, camera.GetWindowHeight() + yTranslation,
                this->itemPixelWidth, this->itemPixelHeight, 
                this->nonActivatedItemsFadeAnim.GetInterpolantValue(), 1);
	    }
    }

    // Draw the selected item last - that way the selection indicator is on top of all other items!
    float selectedItemAlpha = this->activatedItemFadeAnim.GetInterpolantValue();
    if (this->selectedItemIndex >= 0 && selectedItemAlpha > 0.0f) {
        ListItem* currItem = this->GetSelectedItem();
        assert(currItem != NULL);

        // Translate the OpenGL matrix to the item's bottom-left
        this->GetTranslationToItemAtIndex(this->selectedItemIndex, xTranslation, yTranslation);
        glPushMatrix();
        glTranslatef(xTranslation, yTranslation, 0);

        float scalingFactor = this->activatedItemGrowAnim.GetInterpolantValue();

	    // Draw the selection indicator around the item...
        currItem->DrawSelection(camera, this->itemPixelWidth, this->itemPixelHeight, 
            this->selectionAlphaOrangeAnim.GetInterpolantValue() * selectedItemAlpha, 
            this->selectionAlphaYellowAnim.GetInterpolantValue() * selectedItemAlpha,
            this->selectionBorderAddYellowAnim.GetInterpolantValue(), scalingFactor);

        glTranslatef(this->lockedSelectionAnim.GetInterpolantValue(), 0, 0);
        currItem->DrawItem(camera, this->itemPixelWidth, this->itemPixelHeight, 
                           selectedItemAlpha, scalingFactor);
        glPopMatrix();

        currItem->DrawNewLabel(xTranslation, camera.GetWindowHeight() + yTranslation, 
            this->itemPixelWidth, this->itemPixelHeight, selectedItemAlpha, scalingFactor);
    }

    float activatedItemAlpha = this->activatedItemAlphaAnim.GetInterpolantValue();
    if (activatedItemAlpha > 0 && this->GetSelectedItem() != NULL) {
        this->GetSelectedItem()->DrawAsActivated(camera);
    }

    glPopAttrib();
}

void ItemListView::DrawPost(const Camera& camera) {

    float blackBorderAmt = this->blackBorderAnim.GetInterpolantValue();
    if (blackBorderAmt > 0) {

        // Only draw the border if the item allows it
        ListItem* selectedItem = this->GetSelectedItem();
        if (selectedItem == NULL) {
            assert(false);        
            return;
        }

        glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT, GL_FILL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        glLoadIdentity();

        if (selectedItem->DrawBorderOnActivation()) {
            float screenWidth = camera.GetWindowWidth();
            float screenHeight = camera.GetWindowHeight();
            float topBorderBottomY = screenHeight - blackBorderAmt;



            glColor4f(0, 0, 0, 1);
            glBegin(GL_QUADS);

            // Top Black Border
            glVertex2i(screenWidth, screenHeight);
            glVertex2i(0, screenHeight);
            glVertex2i(0, topBorderBottomY);
            glVertex2i(screenWidth, topBorderBottomY);
            
            // Bottom Black Border
            glVertex2i(screenWidth, blackBorderAmt);
            glVertex2i(0, blackBorderAmt);
            glVertex2i(0, 0);
            glVertex2i(screenWidth, 0);

            glEnd();

            this->keyLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));
        }
        else {
            this->keyLabel->SetBeforeAndAfterTextColour(Colour(0,0,0));
        }

        // Draw the keyboard key for escaping the entry pop-up
        static const float X_TEXT_LOC = 20;
        const float Y_TEXT_LOC = this->keyLabel->GetHeight();
        float activatedItemAlpha = this->activatedItemAlphaAnim.GetInterpolantValue() * this->pressEscAlphaAnim.GetInterpolantValue();
        this->keyLabel->SetAlpha(activatedItemAlpha);
        this->keyLabel->SetTopLeftCorner(X_TEXT_LOC, Y_TEXT_LOC);
        this->keyLabel->Draw();

        glPopMatrix();
        glPopAttrib();
    }
}

// Adds a new list item to the end of the current list 
ItemListView::ListItem* ItemListView::AddBlammopediaItem(const std::string& name, const std::string& description, 
                                                         const std::string& finePrint, const Colour& colour,
                                                         const Texture* itemTexture, bool isLocked, bool hasBeenViewed) {

    ItemListView::ListItem* newItem = 
        new ItemListView::BlammopediaListItem(this, this->items.size(), name,
        description, finePrint, colour, itemTexture, isLocked, hasBeenViewed);
	this->items.push_back(newItem);

    if (!isLocked) {
        this->nonLockedItemsMap.insert(std::make_pair(newItem->GetIndex(), newItem));
    }

	return newItem;
}

ItemListView::ListItem* ItemListView::AddTutorialItem(const std::string& name, const Texture* itemTexture,
                                                      PopupTutorialHint* item) {

    ItemListView::ListItem* newItem = new ItemListView::TutorialListItem(this, 
        this->items.size(), name, item, itemTexture);
	this->items.push_back(newItem);

    this->nonLockedItemsMap.insert(std::make_pair(newItem->GetIndex(), newItem));

	return newItem;
}

void ItemListView::AdjustSizeToHeight(size_t height) {
    // Adjust the number of items per row, pixel width and thus pixel height so that
    // all the items fit into the height provided
    while ((ceilf(static_cast<float>(this->items.size())/static_cast<float>(this->numItemsPerRow)) * this->itemPixelHeight) > height) {
        this->numItemsPerRow++;

        int itemWidth = (this->listWidth - 2 * this->horizontalBorder - (this->numItemsPerRow -1) * this->horizontalGap) / this->numItemsPerRow;
	    if (itemWidth < 0) {
		    assert(false);
		    return;
	    }
        // Make sure the item width does not exceed the maximum - if it does then we need to scale the item width down...
        while (itemWidth > MAX_ITEM_WIDTH) {
            this->numItemsPerRow++;
            itemWidth = (this->listWidth - 2 * this->horizontalBorder - (this->numItemsPerRow -1) * this->horizontalGap) / this->numItemsPerRow;
        }
        
        this->itemPixelWidth = itemWidth;
        this->itemPixelHeight = this->itemPixelWidth / 2;
        if (this->numItemsPerRow > MAX_ITEMS_PER_ROW) {
            break;
        }
    }
    
    if (static_cast<int>(this->items.size()) < this->numItemsPerRow) {
        // Change the sizes to fill in the row...
        int totalItemWidth = this->listWidth - (static_cast<int>(this->items.size()) + 1) * this->horizontalGap - 2 * this->horizontalBorder;
        this->itemPixelWidth = totalItemWidth / static_cast<int>(this->items.size());
        if (this->itemPixelWidth > MAX_ITEM_WIDTH) {
            this->itemPixelWidth = MAX_ITEM_WIDTH;
        }
        
        this->itemPixelHeight = this->itemPixelWidth / 2;

        // Center horizontally if there's only one row...
        this->horizontalBorder = (this->listWidth - (static_cast<int>(this->items.size()) - 1) * this->horizontalGap -
            this->items.size() * this->itemPixelWidth) / 2;
    }

    // Center the menu vertically
    int numRows = ceilf(static_cast<float>(this->items.size())/static_cast<float>(this->numItemsPerRow));
    this->verticalBorder = (this->listHeight - (numRows - 1) * this->verticalGap - numRows * this->itemPixelHeight) / 2;
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
    if (this->GetSelectedItem() == NULL) { return; }

    int wrapAroundX;
    if (this->itemIsActivated) {
        // Can't go to another item if only one is unlocked.
        if (this->nonLockedItemsMap.size() == 1) {
            return;
        }

        // When an item is activated we want the selection to traverse the entire list for non-locked items
        std::map<size_t, ListItem*>::const_iterator findIter = this->nonLockedItemsMap.find(this->GetSelectedItem()->GetIndex());
        assert(findIter != this->nonLockedItemsMap.end());
        
        if (right) {
            // Use a forward iterator to get to the next item...
            ++findIter;
            if (findIter == this->nonLockedItemsMap.end()) {
                findIter = this->nonLockedItemsMap.begin();
            }
        }
        else {
            --findIter;
            if (findIter == this->nonLockedItemsMap.end()) {
                --findIter;
            }
        }
        
        this->SetSelection(findIter->second->GetIndex());
    }
    else {
        int x = right ? 1 : -1;
        int currRowIndex  = this->selectedItemIndex / this->numItemsPerRow;
        int numItemsOnRow = this->GetNumItemsOnRow(currRowIndex);
        wrapAroundX = (numItemsOnRow + (this->selectedItemIndex - currRowIndex * this->numItemsPerRow) + x) % numItemsOnRow;
        this->SetSelection(this->numItemsPerRow * currRowIndex + wrapAroundX);
    }

    assert(this->GetSelectedItem() != NULL);
}

void ItemListView::MoveSelectionY(bool up) {
    if (this->GetSelectedItem() == NULL || this->itemIsActivated) { return; }
    int y = up ? 1 : -1;

    // Ignore if we're at the top row and we're trying to move up
    if (y == 1 && this->selectedItemIndex < this->numItemsPerRow) {
        return;
    }

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
    this->arrowFlashAnim.ResetToStart();
    ListItem* selectedItem = this->GetSelectedItem();
    if (selectedItem == NULL) { return; }

    if (selectedItem->GetIsLocked()) {
        this->StartLockedAnimation();
    }
    else {
        this->itemIsActivated = true;

        this->nonActivatedItemsFadeAnim.SetLerp(0.5, 0.0f);
        this->nonActivatedItemsFadeAnim.SetRepeat(false);

        this->activatedItemFadeAnim.SetLerp(1.0, 0.0f);
        this->activatedItemFadeAnim.SetRepeat(false);

        this->activatedItemGrowAnim.SetLerp(0.0, 1.0, this->activatedItemGrowAnim.GetInterpolantValue(), 2.0f);
        this->activatedItemGrowAnim.SetRepeat(false);

        this->blackBorderAnim.SetLerp(0.8, 1.2, this->blackBorderAnim.GetInterpolantValue(), ItemListView::BLACK_BORDER_HEIGHT);
        this->blackBorderAnim.SetRepeat(false);

        this->activatedItemXPicAnim.SetLerp(1.0, 1.75, this->activatedItemXPicAnim.GetInterpolantValue(), ItemListView::HORIZ_ITEM_ACTIVATED_BORDER);
        this->activatedItemXPicAnim.SetRepeat(false);

        this->activatedItemAlphaAnim.SetLerp(1.0, 1.75, this->activatedItemAlphaAnim.GetInterpolantValue(), 1.0f);
        this->activatedItemAlphaAnim.SetRepeat(false);

        std::vector<float> alphaVals;
        alphaVals.push_back(0.15f);
        alphaVals.push_back(1.0f);
        alphaVals.push_back(0.15f);
        std::vector<double> timeVals;
        timeVals.push_back(0.0);
        timeVals.push_back(2.0);
        timeVals.push_back(4.0);
        this->pressEscAlphaAnim.SetLerp(timeVals, alphaVals);
        this->pressEscAlphaAnim.SetRepeat(true);

        selectedItem->Activated();
    }   
}

void ItemListView::ItemDeactivated() {
    this->arrowFlashAnim.ResetToStart();
    if (this->itemIsActivated) {
        this->itemIsActivated = false;

        // Make all previously non-activated items visible again
        this->nonActivatedItemsFadeAnim.SetLerp(0.5, 1.0f);
        this->nonActivatedItemsFadeAnim.SetRepeat(false);

        this->activatedItemFadeAnim.SetLerp(0.5, 1.0f);
        this->activatedItemFadeAnim.SetRepeat(false);

        this->activatedItemGrowAnim.SetLerp(0.5, 1.0f);
        this->activatedItemGrowAnim.SetRepeat(false);

        this->blackBorderAnim.SetLerp(0.5, 0.0f);
        this->blackBorderAnim.SetRepeat(false);

        this->activatedItemXPicAnim.SetLerp(0.5, listWidth + this->itemPixelWidth);
        this->activatedItemXPicAnim.SetRepeat(false);

        this->activatedItemAlphaAnim.SetLerp(0.5, 0.0f);
        this->activatedItemAlphaAnim.SetRepeat(false);

        this->pressEscAlphaAnim.ClearLerp();
        this->pressEscAlphaAnim.SetRepeat(false);
        this->pressEscAlphaAnim.SetInterpolantValue(0.0f);

        this->GetSelectedItem()->Deactivated();
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

ItemListView::ListItem::ListItem(const ItemListView* parent, size_t index, const std::string& name,
                                 const Texture* itemTexture, bool isLocked) : 
parent(parent), texture(itemTexture), index(index), isLocked(isLocked),
nameLbl(new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), name))
{
	assert(parent != NULL);
    assert(itemTexture != NULL);

    this->nameLbl->SetColour(Colour(1,1,1));
    this->nameLbl->SetDropShadow(Colour(0,0,0), 0.1f);

    this->halfSelectionBorderSize = static_cast<float>(parent->GetSmallestBorderSize()) / 2.0f;

    this->sizeAnimation = AnimationLerp<float>(1.0f);
    this->sizeAnimation.SetInterpolantValue(1.0f);
    this->sizeAnimation.SetRepeat(false);
}

ItemListView::ListItem::~ListItem() {
    delete this->nameLbl;
    this->nameLbl = NULL;
}

void ItemListView::ListItem::Tick(double dT) {
    this->sizeAnimation.Tick(dT);
}

void ItemListView::ListItem::DrawSelection(const Camera& camera, 
                                           size_t width, size_t height, 
                                           float alphaOrange, float alphaYellow, 
                                           float scale, float otherScale) {
	UNUSED_PARAMETER(camera);
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
void ItemListView::ListItem::DrawItem(const Camera& camera, size_t width, size_t height, float alpha, float scale) {
	UNUSED_PARAMETER(camera);
    
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

void ItemListView::ListItem::DrawItem(const Camera& camera, size_t width, size_t height, float alpha) {
    UNUSED_PARAMETER(camera);

	this->texture->BindTexture();

    glPushMatrix();
    float halfWidth  = width / 2.0f;
    float halfHeight = height / 2.0f;
    glTranslatef(halfWidth, halfHeight, 0);
    glScalef(1, 1, 1);

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

void ItemListView::ListItem::DrawNewLabel(float, float,size_t, size_t, float, float) {
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

ItemListView::BlammopediaListItem::BlammopediaListItem(const ItemListView* parent, size_t index, const std::string& name, 
                                                       const std::string& description, const std::string& finePrint, const Colour& colour,
                                                       const Texture* itemTexture, bool isLocked, bool hasBeenViewed) :
ListItem(parent, index, name, itemTexture, isLocked), colour(colour), newLbl(NULL),
descriptionLbl(new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
               GameFontAssetsManager::Big), parent->GetListWidth() - 2*ItemListView::HORIZ_ITEM_ACTIVATED_BORDER, description)),
finePrintLbl(new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
             GameFontAssetsManager::Medium), parent->GetListWidth() - 2*ItemListView::HORIZ_ITEM_ACTIVATED_BORDER, finePrint))
{


    this->descriptionLbl->SetColour(Colour(0,0,0));
    this->descriptionLbl->SetLineSpacing(10.0f);

    if (!isLocked && !hasBeenViewed) {
        this->newLbl = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
            GameFontAssetsManager::Medium), "NEW");

	    std::vector<double> timeVals;
	    timeVals.reserve(3);
	    timeVals.push_back(0.0);
	    timeVals.push_back(0.5);
	    timeVals.push_back(1.0);
	    std::vector<Colour> colourVals;
	    colourVals.reserve(3);
	    colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	    colourVals.push_back(Colour(1.0f, 0.8f, 0.0f));
	    colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	    this->newColourFlashAnimation.SetLerp(timeVals, colourVals);
	    this->newColourFlashAnimation.SetRepeat(true);

        this->newLbl->SetColour(this->newColourFlashAnimation.GetInterpolantValue());
        this->newLbl->SetDropShadow(Colour(0,0,0), 0.1f);
    }
}

ItemListView::BlammopediaListItem::~BlammopediaListItem() {
    delete this->descriptionLbl;
    this->descriptionLbl = NULL;
    delete this->finePrintLbl;
    this->finePrintLbl = NULL;

    if (this->newLbl != NULL) {
        delete this->newLbl;
        this->newLbl = NULL;
    }
}

void ItemListView::BlammopediaListItem::Tick(double dT) {
    ListItem::Tick(dT);
    if (this->newLbl != NULL) {
        this->newColourFlashAnimation.Tick(dT);
        this->newLbl->SetColour(this->newColourFlashAnimation.GetInterpolantValue());
    }
}

void ItemListView::BlammopediaListItem::DrawAsActivated(const Camera& camera) {

    static const float PIC_TITLE_GAP = 40;
    static const float FINE_PRINT_DESC_GAP = 30;
    static const float STRIPE_BORDER = 20;
    
    float itemPicX = this->parent->activatedItemXPicAnim.GetInterpolantValue();
    float itemAndTitleYTopLeftCorner = camera.GetWindowHeight() - 2*ItemListView::BLACK_BORDER_HEIGHT;
    float itemAndTitleYUnderPic      = itemAndTitleYTopLeftCorner - this->parent->itemPixelHeight;

    float activatedItemAlpha = this->parent->activatedItemAlphaAnim.GetInterpolantValue();

    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_TEXTURE_2D);

    // Draw the stripe background for the title
    glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), activatedItemAlpha);
    glBegin(GL_QUADS);
    glVertex2f(0, itemAndTitleYTopLeftCorner + STRIPE_BORDER);
    glVertex2f(0, itemAndTitleYUnderPic - STRIPE_BORDER);
    glVertex2f(camera.GetWindowWidth(), itemAndTitleYUnderPic - STRIPE_BORDER);
    glVertex2f(camera.GetWindowWidth(), itemAndTitleYTopLeftCorner + STRIPE_BORDER);
    glEnd();

    glLineWidth(3.0f);
    glColor4f(0, 0, 0, activatedItemAlpha);
    glBegin(GL_LINES);
    glVertex2f(0, itemAndTitleYTopLeftCorner + STRIPE_BORDER);
    glVertex2f(camera.GetWindowWidth(), itemAndTitleYTopLeftCorner + STRIPE_BORDER);
    glVertex2f(0, itemAndTitleYUnderPic - STRIPE_BORDER);
    glVertex2f(camera.GetWindowWidth(), itemAndTitleYUnderPic - STRIPE_BORDER);
    glEnd();

    // Draw next arrows if there are other items the player could select
    if (this->parent->nonLockedItemsMap.size() > 1) {

        static const float HORIZ_BORDER = 10.0f;
        static const float VERT_BORDER  = 10.0f;
        static const float TRI_SIZE_HORIZ = 30.0f;
        static const float TRI_SIZE_VERT  = 60.0f;

        const ColourRGBA& arrowColour = this->parent->arrowFlashAnim.GetInterpolantValue();

        glBegin(GL_TRIANGLES);
        glColor4f(arrowColour.R(), arrowColour.G(), arrowColour.B(), arrowColour.A()*activatedItemAlpha);

        glVertex2f(HORIZ_BORDER + TRI_SIZE_HORIZ, ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER + TRI_SIZE_VERT);
        glVertex2f(HORIZ_BORDER, ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER + TRI_SIZE_VERT/2);
        glVertex2f(HORIZ_BORDER + TRI_SIZE_HORIZ, ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER);
        
        glVertex2f(camera.GetWindowWidth() - (HORIZ_BORDER + TRI_SIZE_HORIZ), ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER);
        glVertex2f(camera.GetWindowWidth() - HORIZ_BORDER, ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER + TRI_SIZE_VERT/2);
        glVertex2f(camera.GetWindowWidth() - (HORIZ_BORDER + TRI_SIZE_HORIZ), ItemListView::BLACK_BORDER_HEIGHT + VERT_BORDER + TRI_SIZE_VERT);

        glEnd();
    }

    glPushMatrix();
    glTranslatef(itemPicX, itemAndTitleYUnderPic, 0);
    this->parent->GetSelectedItem()->DrawItem(camera, this->parent->itemPixelWidth, this->parent->itemPixelHeight, activatedItemAlpha);
    glPopMatrix();

    this->nameLbl->SetTopLeftCorner(itemPicX + this->parent->itemPixelWidth + PIC_TITLE_GAP, itemAndTitleYUnderPic + 1.2f * nameLbl->GetHeight());
    this->nameLbl->SetAlpha(activatedItemAlpha);
    this->nameLbl->Draw();

    this->descriptionLbl->SetTopLeftCorner(itemPicX, itemAndTitleYUnderPic - PIC_TITLE_GAP);
    this->descriptionLbl->SetAlpha(activatedItemAlpha);
    this->descriptionLbl->Draw();

    this->finePrintLbl->SetTopLeftCorner(itemPicX, itemAndTitleYUnderPic - PIC_TITLE_GAP - this->descriptionLbl->GetHeight() - FINE_PRINT_DESC_GAP);
    this->finePrintLbl->SetAlpha(activatedItemAlpha);
    this->finePrintLbl->Draw();

    glPopMatrix();
}

void ItemListView::BlammopediaListItem::DrawNewLabel(float bottomLeftX, float bottomLeftY,
                                          size_t width, size_t height,
                                          float alpha, float scale) {
    UNUSED_PARAMETER(height);
    if (this->newLbl == NULL) {
        return;
    }

    this->newLbl->SetTopLeftCorner(bottomLeftX + width - this->newLbl->GetLastRasterWidth(),
                                   bottomLeftY + this->newLbl->GetHeight());
    this->newLbl->SetAlpha(alpha);
    this->newLbl->SetScale(scale);

    this->newLbl->Draw();
}

void ItemListView::BlammopediaListItem::TurnOffNewLabel() {
    if (this->newLbl != NULL) {
        delete this->newLbl;
        this->newLbl = NULL;
    }
}

ItemListView::TutorialListItem::TutorialListItem(const ItemListView* parent, size_t index, const std::string& name,
                                                 PopupTutorialHint* tutorialPopup, const Texture* itemTexture) :
ListItem(parent, index, name, itemTexture, false), tutorialPopup(tutorialPopup) {
    assert(tutorialPopup != NULL);

}

ItemListView::TutorialListItem::~TutorialListItem() {
    delete this->tutorialPopup;
    this->tutorialPopup = NULL;
}

void ItemListView::TutorialListItem::Activated() {
    this->tutorialPopup->Show(1.0, 0.75);
}

void ItemListView::TutorialListItem::Deactivated() {
    this->tutorialPopup->Unshow(0.0, 0.6);
}

void ItemListView::TutorialListItem::Tick(double dT) {
    ListItem::Tick(dT);
    
    this->tutorialPopup->Tick(dT);
}

void ItemListView::TutorialListItem::DrawAsActivated(const Camera& camera) {
    this->tutorialPopup->Draw(camera);
}