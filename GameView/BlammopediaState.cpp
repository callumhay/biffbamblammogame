/**
 * BlammopediaState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BlammopediaState.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "PopupTutorialHint.h"
#include "GameAssets.h"
#include "GameTutorialAssets.h"
#include "GameFontAssetsManager.h"

#include "../ResourceManager.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../GameModel/GameItemFactory.h"

const int BlammopediaState::ITEM_NAME_BORDER_SIZE = 10;
const int BlammopediaState::TOTAL_MENU_HEIGHT     = 60;

const Colour BlammopediaState::SELECTION_COLOUR(1.0f, 1.0f, 0.0f);
const Colour BlammopediaState::IDLE_COLOUR(0.75f, 0.75f, 0.75f);
const float BlammopediaState::SELECTION_SCALE = 1.1f;

const float BlammopediaState::NO_SELECTION_DROP_SHADOW_AMT = 0.1f;
const float BlammopediaState::SELECTION_DROP_SHADOW_AMT    = 0.15f;

const char* BlammopediaState::LOCKED_NAME = "Locked !";

const float BlammopediaState::LABEL_ITEM_GAP = 50;

BlammopediaState::BlammopediaState(GameDisplay* display) : 
DisplayState(display), currMenuItemIndex(NO_MENU_ITEM_INDEX), currListViewIndex(0), 
goBackToMainMenu(false) {

    this->itemSelTabAnim.SetInterpolantValue(0.0f);
    this->itemSelTabAnim.SetRepeat(false);

    const Camera& camera = this->display->GetCamera();

    this->fadeAnimation.SetLerp(0.0, 0.5f, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

    Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
    assert(blammopedia != NULL);
	this->listViews.reserve(3);
    this->listViews.push_back(this->BuildGameplayListView());
	this->listViews.push_back(this->BuildGameItemsListView(blammopedia));
	this->listViews.push_back(this->BuildGameBlockListView(blammopedia));

    this->selectedItemNameLbl.SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium));
    this->selectedItemNameLbl.SetColour(Colour(1, 0.65f, 0));
    this->selectedItemNameLbl.SetDropShadow(Colour(0,0,0), 0.125f);

    TextLabel2D* gameplayMenuItem = new TextLabel2D();
    gameplayMenuItem->SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    gameplayMenuItem->SetText("Gameplay");
    gameplayMenuItem->SetColour(IDLE_COLOUR);
    gameplayMenuItem->SetDropShadow(Colour(0,0,0), NO_SELECTION_DROP_SHADOW_AMT);

    float gameplayMenuItemScaledWidth  = BlammopediaState::SELECTION_SCALE * gameplayMenuItem->GetLastRasterWidth();

    TextLabel2D* itemListMenuItem = new TextLabel2D();
    itemListMenuItem->SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    itemListMenuItem->SetText("Items");
    itemListMenuItem->SetColour(IDLE_COLOUR);
    itemListMenuItem->SetDropShadow(Colour(0,0,0), NO_SELECTION_DROP_SHADOW_AMT);

    float itemListMenuItemScaledWidth  = BlammopediaState::SELECTION_SCALE * itemListMenuItem->GetLastRasterWidth();

    TextLabel2D* blockMenuItem = new TextLabel2D();
    blockMenuItem->SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    blockMenuItem->SetText("Blocks");
    blockMenuItem->SetColour(IDLE_COLOUR);
    blockMenuItem->SetDropShadow(Colour(0,0,0), NO_SELECTION_DROP_SHADOW_AMT);

    float blockMenuItemScaledWidth = BlammopediaState::SELECTION_SCALE *  blockMenuItem->GetLastRasterWidth();

    float startMenuItemsX = (camera.GetWindowWidth() - gameplayMenuItemScaledWidth - itemListMenuItemScaledWidth - blockMenuItemScaledWidth - 2 * LABEL_ITEM_GAP)/2;
    float topLeftY = 0.0f;
    
    topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - gameplayMenuItem->GetHeight()) / 2.0f + gameplayMenuItem->GetHeight();
    gameplayMenuItem->SetTopLeftCorner(Point2D(startMenuItemsX, topLeftY));
    startMenuItemsX += gameplayMenuItem->GetLastRasterWidth() + LABEL_ITEM_GAP;

    topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - itemListMenuItem->GetHeight()) / 2.0f + itemListMenuItem->GetHeight();
    itemListMenuItem->SetTopLeftCorner(Point2D(startMenuItemsX, topLeftY));
    startMenuItemsX += itemListMenuItem->GetLastRasterWidth() + LABEL_ITEM_GAP;

    topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - blockMenuItem->GetHeight()) / 2.0f + blockMenuItem->GetHeight();
    blockMenuItem->SetTopLeftCorner(Point2D(startMenuItemsX, topLeftY));
    startMenuItemsX += blockMenuItem->GetLastRasterWidth() + LABEL_ITEM_GAP;

    TextLabel2D* backMenuItem = new TextLabel2D();
    backMenuItem->SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    backMenuItem->SetText("Back");
    backMenuItem->SetColour(IDLE_COLOUR);
    backMenuItem->SetDropShadow(Colour(0,0,0), NO_SELECTION_DROP_SHADOW_AMT);
    topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - backMenuItem->GetHeight()) / 2.0f + backMenuItem->GetHeight();
    backMenuItem->SetTopLeftCorner(BlammopediaState::ITEM_NAME_BORDER_SIZE, topLeftY);

    static const double TIME_INTERVAL_AMT = 0.15;
    static const float  WIGGLE_INTERVAL_AMT = 4.0f;
    std::vector<double> timeVals;
    timeVals.push_back(0.0);
    timeVals.push_back(TIME_INTERVAL_AMT);
    timeVals.push_back(2*TIME_INTERVAL_AMT);
    timeVals.push_back(3*TIME_INTERVAL_AMT);
    timeVals.push_back(4*TIME_INTERVAL_AMT);
    std::vector<float> wiggleVals;
    wiggleVals.push_back(0.0f);
    wiggleVals.push_back(WIGGLE_INTERVAL_AMT);
    wiggleVals.push_back(0.0f);
    wiggleVals.push_back(-WIGGLE_INTERVAL_AMT);
    wiggleVals.push_back(0.0f);
    this->itemHighlightWiggle.SetLerp(timeVals, wiggleVals);
    this->itemHighlightWiggle.SetRepeat(true);
    this->itemHighlightWiggle.SetInterpolantValue(0.0f);

    this->blammoMenuLabels.resize(TOTAL_NUM_MENU_ITEMS);
    this->blammoMenuLabels[GAMEPLAY_MENU_ITEM_INDEX] = gameplayMenuItem;
    this->blammoMenuLabels[ITEMS_MENU_ITEM_INDEX]    = itemListMenuItem;
    this->blammoMenuLabels[BLOCK_MENU_ITEM_INDEX]    = blockMenuItem;
    this->blammoMenuLabels[BACK_MENU_ITEM_INDEX]     = backMenuItem;

    this->SetBlammoMenuItemHighlighted(GAMEPLAY_MENU_ITEM_INDEX);
    this->SetBlammoMenuItemSelection();

    debug_opengl_state();
}

BlammopediaState::~BlammopediaState() {
	for (std::vector<ItemListView*>::iterator iter = this->listViews.begin(); iter != this->listViews.end(); ++iter) {
		ItemListView* currListView = *iter;
		delete currListView;
		currListView = NULL;
	}
	this->listViews.clear();

    for (std::vector<TextLabel2D*>::iterator iter = this->blammoMenuLabels.begin(); iter != this->blammoMenuLabels.end(); ++iter) {
        TextLabel2D* label = *iter;
        assert(label != NULL);
        delete label;
        label = NULL;
    }
    this->blammoMenuLabels.clear();
}

void BlammopediaState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();
    
    ItemListView* currListView = this->GetCurrentListView();
    assert(currListView != NULL);
    //currListView->Tick(dT);

    ItemListView::ListItem* currItem = currListView->GetSelectedItem();

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    static const float LINE_WIDTH = 2.0f;
    glLineWidth(LINE_WIDTH);

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    this->itemSelTabAnim.Tick(dT);

    // Check to see if an item is activated from the current blammopedia list, if it is then it will
    // draw over the entire screen and we don't need to draw all the other stuff
    if (!currListView->GetIsItemActivated()) {

        // Draw the bottom menu background
        glColor4f(1, 0.65f, 0, 1.0f);
        glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(camera.GetWindowWidth(), 0);
        glVertex2i(camera.GetWindowWidth(), TOTAL_MENU_HEIGHT);
        glVertex2i(0, TOTAL_MENU_HEIGHT);
        glEnd();
        glColor4f(0, 0, 0, 1.0f);
        glBegin(GL_LINES);
        glVertex2i(camera.GetWindowWidth(), TOTAL_MENU_HEIGHT);
        glVertex2i(0, TOTAL_MENU_HEIGHT);
        glEnd();

        if (this->currMenuItemIndex < static_cast<int>(this->blammoMenuLabels.size())) {
            float tabHeightScale = this->itemSelTabAnim.GetInterpolantValue();

            TextLabel2D* selectedLabel = this->blammoMenuLabels[this->currListViewIndex];
            float quadLeftX  = selectedLabel->GetTopLeftCorner()[0] - LABEL_ITEM_GAP/2.0f;
            float quadRightX = selectedLabel->GetTopLeftCorner()[0] + selectedLabel->GetLastRasterWidth() + LABEL_ITEM_GAP/2.0f;
            float bottomY = selectedLabel->GetTopLeftCorner()[1] - selectedLabel->GetHeight() - 10;
            float animatedBottomY = (1.0f - tabHeightScale) * (TOTAL_MENU_HEIGHT + LINE_WIDTH) + tabHeightScale * bottomY;

            glColor4f(1,1,1,1);
            glBegin(GL_QUADS);
            glVertex2i(quadLeftX,  animatedBottomY);
            glVertex2i(quadRightX, animatedBottomY);
            glVertex2i(quadRightX, (TOTAL_MENU_HEIGHT + LINE_WIDTH));
            glVertex2i(quadLeftX,  (TOTAL_MENU_HEIGHT + LINE_WIDTH));
            glEnd();

            animatedBottomY = (1.0f - tabHeightScale) * TOTAL_MENU_HEIGHT + tabHeightScale * bottomY;
            glColor4f(0, 0, 0, 1.0f);
            glBegin(GL_LINES);
            glVertex2i(quadLeftX,  animatedBottomY);
            glVertex2i(quadRightX, animatedBottomY);

            glVertex2i(quadRightX, animatedBottomY);
            glVertex2i(quadRightX, TOTAL_MENU_HEIGHT);

            glVertex2i(quadLeftX, TOTAL_MENU_HEIGHT);
            glVertex2i(quadLeftX,  animatedBottomY);
            glEnd();
        }

        if (currItem != NULL) {
            // Draw the name of the currently selected item in the active blammopedia list...
            // If the item is locked we change the colour value to something that looks neutral
            if (currItem->GetIsLocked()) {
                this->selectedItemNameLbl.SetColour(Colour(0.66f, 0.66f, 0.66f));
            }
            else {
                this->selectedItemNameLbl.SetColour(currItem->GetColour());
            }
            this->selectedItemNameLbl.SetText(currItem->GetNameLbl()->GetText());
            this->selectedItemNameLbl.SetTopLeftCorner(camera.GetWindowWidth() - 
                this->selectedItemNameLbl.GetLastRasterWidth() - BlammopediaState::ITEM_NAME_BORDER_SIZE,
                this->selectedItemNameLbl.GetHeight() + BlammopediaState::ITEM_NAME_BORDER_SIZE);
            this->selectedItemNameLbl.Draw();
        }

        // Animate any highlighted labels...
        if (this->currMenuItemIndex != NO_MENU_ITEM_INDEX) {
            this->itemHighlightWiggle.Tick(dT);
            float wiggleAmt = this->itemHighlightWiggle.GetInterpolantValue();
            TextLabel2D* highlightedItem = this->blammoMenuLabels[this->currMenuItemIndex];
            Point2D defaultTopLeftCorner(highlightedItem->GetTopLeftCorner()[0], 
                (BlammopediaState::TOTAL_MENU_HEIGHT - highlightedItem->GetHeight()) / 2.0f + highlightedItem->GetHeight());
            highlightedItem->SetTopLeftCorner(defaultTopLeftCorner + Vector2D(0, wiggleAmt));
        }
        
        // Draw the labels
        for (std::vector<TextLabel2D*>::iterator iter = this->blammoMenuLabels.begin();
             iter != this->blammoMenuLabels.end(); ++iter) {

            TextLabel2D* label = *iter;
            label->Draw();
        }
    }

    currListView->DrawPost(camera);

    // Now draw the currently selected list of the blammopedia...
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, camera.GetWindowHeight(), 0);
    currListView->Draw(dT, camera);
    glPopMatrix();

    glPopMatrix();
    Camera::PopWindowCoords();
    glPopAttrib();

	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone || this->goBackToMainMenu) {
        this->DrawFadeOverlay(camera.GetWindowWidth(), camera.GetWindowHeight(), this->fadeAnimation.GetInterpolantValue());
    }
  
    if (fadeDone && this->goBackToMainMenu) {
        // Go back to the main menu now
        this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::MainMenu, this->display));
        return;
    }

    debug_opengl_state();
}

void BlammopediaState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    if (this->goBackToMainMenu) {
        return;
    }

    ItemListView* currList = this->GetCurrentListView();
    if (currList == NULL) {
        return;
    }
    bool itemWasActivated = false;

    // Only give the list a button event when the list has focus and not the blammopedia menu
    if (this->currMenuItemIndex == NO_MENU_ITEM_INDEX) {
        itemWasActivated = currList->GetIsItemActivated();

        // In the case where down is pressed on the last row of the selected list, we exit the list
        if (!itemWasActivated && pressedButton == GameControl::DownButtonAction &&
            currList->GetIsSelectedItemOnLastRow()) {
            
            // Turn off the currently active list from showing a selected item
            currList->ButtonPressed(GameControl::EscapeButtonAction);
            currList->SetSelectedItemIndex(-1);
            this->SetBlammoMenuItemDeselection();
        }
        else {
            currList->ButtonPressed(pressedButton);
            if (pressedButton == GameControl::EnterButtonAction || currList->GetIsItemActivated()) {
                ItemListView::ListItem* selectedItem = currList->GetSelectedItem();
                if (selectedItem != NULL) {
                    if (!selectedItem->GetIsLocked()) {
                
                        std::map<ItemListView::ListItem*, Blammopedia::Entry*>::iterator findIter = this->itemToEntryMap.find(selectedItem);
                        if (findIter != this->itemToEntryMap.end()) {
                            // Check if it's an item or block type of blammopedia entry...
                            findIter->second->SetHasBeenViewed(true);
                        }
                        selectedItem->TurnOffNewLabel();
                        
                    }
                }
            }
        }
    }
    else {
        // Turn off the currently active list from showing a selected item
        currList->SetSelectedItemIndex(-1);

        // TODO: select the appropriate blammopedia menu entry and change the list being displayed...
        switch (pressedButton) {
            case GameControl::EnterButtonAction:
            case GameControl::UpButtonAction:
                this->SetBlammoMenuItemSelection();
                break;

            case GameControl::LeftButtonAction: {
                    int tempIndex = this->currMenuItemIndex - 1;
                    if (tempIndex < 0) {
                        tempIndex = TOTAL_NUM_MENU_ITEMS - 1;
                    }
                    this->SetBlammoMenuItemHighlighted(tempIndex);
                }
                break;

            case GameControl::RightButtonAction:
                this->SetBlammoMenuItemHighlighted((this->currMenuItemIndex + 1) % TOTAL_NUM_MENU_ITEMS);
                break;

            case GameControl::EscapeButtonAction:
                // This case is dealt with later in the function
            default:
                break;
        }
    }

    // If we escape from a list then we go into the blammopedia menu, if we're already in the menu
    // then we go to the 'back' option, if we're already on the back option then we go back to the main menu
    if (!itemWasActivated && (pressedButton == GameControl::EscapeButtonAction)) {
        if (this->currMenuItemIndex == NO_MENU_ITEM_INDEX) {
            this->SetBlammoMenuItemDeselection();
        }
        else {
            if (this->currMenuItemIndex != BACK_MENU_ITEM_INDEX) {
                this->SetBlammoMenuItemHighlighted(BACK_MENU_ITEM_INDEX);
            }
            else {
                // Go back to the main menu...
                this->SetBlammoMenuItemSelection();
            }   
        }
    }
}

void BlammopediaState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

ItemListView* BlammopediaState::BuildGameplayListView() {

    GameTutorialAssets* tutorialAssets = this->display->GetAssets()->GetTutorialAssets();

    const Camera& camera = this->display->GetCamera();
    ItemListView* itemsListView = new ItemListView(camera.GetWindowWidth(), camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);

    static const float TITLE_TEXT_SCALE = 1.33f;
    static const float BODY_TEXT_SCALE  = 0.75f;
    static const size_t POPUP_TUTORIAL_HINT_WIDTH = 700;

    // Tutorial item: Life and Death
    PopupTutorialHint* lifePopupHint = new PopupTutorialHint(POPUP_TUTORIAL_HINT_WIDTH);
    DecoratorOverlayPane* lifePopupPane = lifePopupHint->GetPane();
    lifePopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    lifePopupPane->AddText("Life and Death", Colour(1,1,1), TITLE_TEXT_SCALE);
    lifePopupPane->SetLayoutType(DecoratorOverlayPane::TwoColumn);
    lifePopupPane->AddText(
        std::string("Each heart in the top-left represents a life. ") +
        std::string("You will begin every level with three. You will lose one when your ball(s) have left play. ") +
        std::string("If you lose all your lives you will be forced to restart the level."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* lifeHUDImg = tutorialAssets->GetLifeTutorialHUDTexture();
    lifePopupPane->AddImage(256, lifeHUDImg);

    lifePopupPane->AddText(
        std::string("If you're running low on life, you can drop a Life-UP item by achieving a 4x multiplier."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* dropingLifeItemImg = tutorialAssets->GetLifeTutorialItemDropTexture();
    lifePopupPane->AddImage(256, dropingLifeItemImg);

    ItemListView::ListItem* lifeTutorialItem = itemsListView->AddTutorialItem(
        "Tutorial: Life and Death", tutorialAssets->GetLifeTutorialItemTexture(), lifePopupHint);
    assert(lifeTutorialItem != NULL);
    UNUSED_VARIABLE(lifeTutorialItem);

    // Tutorial item: Boosting
    PopupTutorialHint* boostPopupHint = new PopupTutorialHint(POPUP_TUTORIAL_HINT_WIDTH);
    DecoratorOverlayPane* boostPopupPane = boostPopupHint->GetPane();
    boostPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    boostPopupPane->AddText("Ball Boosting", Colour(1,1,1), TITLE_TEXT_SCALE);
    boostPopupPane->SetLayoutType(DecoratorOverlayPane::TwoColumn);
    boostPopupPane->AddText(
        std::string("During gameplay the boost gauge in the top-left will fill up and make boosts available."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* boostHUDImg = tutorialAssets->GetBoostTutorialHUDTexture();
    boostPopupPane->AddImage(256, boostHUDImg);

    boostPopupPane->AddText(
        std::string("Activate a boost by holding down the right analog stick or the left mouse button, letting go will cancel the boost.") +
        std::string("When activated, time will be slowed down momentarily, allowing you to redirect the ball."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* boostDirImg = tutorialAssets->GetBoostTutorialDirTexture();
    boostPopupPane->AddImage(256, boostDirImg);

    boostPopupPane->AddText(
        std::string("While time is slowed, you may immediately activate the boost by pressing the left/right trigger or the right mouse button."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    ItemListView::ListItem* boostingTutorialItem = itemsListView->AddTutorialItem(
        "Tutorial: Boosting", tutorialAssets->GetBoostTutorialItemTexture(), boostPopupHint);
    assert(boostingTutorialItem != NULL);
    UNUSED_VARIABLE(boostingTutorialItem);
    
    // Tutorial item: Multipliers
    PopupTutorialHint* multPopupHint  = new PopupTutorialHint(POPUP_TUTORIAL_HINT_WIDTH);
    DecoratorOverlayPane* multPopupPane = multPopupHint->GetPane();
    multPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    multPopupPane->AddText(std::string("Points and Multipliers"), Colour(1,1,1), TITLE_TEXT_SCALE);
    multPopupPane->SetLayoutType(DecoratorOverlayPane::TwoColumn);
    multPopupPane->AddText(
        std::string("Maximize your pointage by consecutively destroying blocks to earn higher multipliers."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* multiplierImg = tutorialAssets->GetMultiplierTutorialTexture();
    multPopupPane->AddImage(256, multiplierImg);

    multPopupPane->AddText(
        std::string("Any multiplier will reset when a ball hits the paddle or when you lose your balls."),
        Colour(1,1,1), BODY_TEXT_SCALE);
    multPopupPane->AddText(
        std::string("Though tempting to use, certain beneficial items (lasers!) will limit your score."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    ItemListView::ListItem* multiplierTutorialItem = itemsListView->AddTutorialItem(
        "Tutorial: Points and Multipliers", tutorialAssets->GetMultiplierTutorialItemTexture(), multPopupHint);
    assert(multiplierTutorialItem != NULL);
    UNUSED_VARIABLE(multiplierTutorialItem);
    
    itemsListView->SetSelectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX);
    itemsListView->AdjustSizeToHeight(camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);
	return itemsListView;
}

ItemListView* BlammopediaState::BuildGameItemsListView(Blammopedia* blammopedia) {
	const Camera& camera = this->display->GetCamera();
    ItemListView* itemsListView = new ItemListView(camera.GetWindowWidth(), camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);
	
	// Add each item in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
    Colour itemColour;
    std::string currName;
    std::string currDesc;
    std::string finePrint;
    const Blammopedia::ItemEntryMap& itemEntries = blammopedia->GetItemEntries();
    for (Blammopedia::ItemEntryMapConstIter iter = itemEntries.begin(); iter != itemEntries.end(); ++iter) {
        Blammopedia::ItemEntry* itemEntry = iter->second;
        const Texture2D* texture = blammopedia->GetLockedItemTexture();
        if (!itemEntry->GetIsLocked()) {

            texture   = itemEntry->GetItemTexture();
            currName  = itemEntry->GetName();
            currDesc  = itemEntry->GetDescription();
            finePrint = itemEntry->GetFinePrint();
            
            GameItem::ItemDisposition itemDisposition = GameItemFactory::GetInstance()->GetItemTypeDisposition(iter->first);
            itemColour = GameViewConstants::GetInstance()->GetItemColourFromDisposition(itemDisposition);
        }
        else {
            itemColour = Colour(0.5, 0.5, 0.5);
            currName = LOCKED_NAME;
            currDesc = "";
        }
        ItemListView::ListItem* listItem = itemsListView->AddBlammopediaItem(currName, currDesc, finePrint, itemColour, texture,
            itemEntry->GetIsLocked(), itemEntry->GetHasBeenViewed());

        this->itemToEntryMap.insert(std::make_pair(listItem, itemEntry));
    }

    itemsListView->SetSelectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX);
    itemsListView->AdjustSizeToHeight(camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);
	return itemsListView;
}

ItemListView* BlammopediaState::BuildGameBlockListView(Blammopedia* blammopedia) {
	const Camera& camera = this->display->GetCamera();
	ItemListView* blockListView = new ItemListView(camera.GetWindowWidth(), camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);

	// Add each block in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
    std::string currName;
    std::string currDesc;
    std::string finePrint;
    const Blammopedia::BlockEntryMap& blockEntries = blammopedia->GetBlockEntries();
    for (Blammopedia::BlockEntryMapConstIter iter = blockEntries.begin(); iter != blockEntries.end(); ++iter) {
        Blammopedia::BlockEntry* blockEntry = iter->second;
        const Texture2D* texture = blammopedia->GetLockedItemTexture();
        if (!blockEntry->GetIsLocked()) {
            texture   = blockEntry->GetBlockTexture();
            currName  = blockEntry->GetName();
            currDesc  = blockEntry->GetDescription();
            finePrint = blockEntry->GetFinePrint();
        }
        else {
            currName = LOCKED_NAME;
        }
        ItemListView::ListItem* listItem = blockListView->AddBlammopediaItem(currName, currDesc, finePrint, Colour(0.3f, 0.6f, 0.85f), texture,
            blockEntry->GetIsLocked(), blockEntry->GetHasBeenViewed());

        this->itemToEntryMap.insert(std::make_pair(listItem, blockEntry));
    }

	blockListView->SetSelectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX);
    blockListView->AdjustSizeToHeight(camera.GetWindowHeight() - TOTAL_MENU_HEIGHT);
	return blockListView;
}

// Sets the fade to fade to white and says that after the fade is done we'll switch
// back to the main menu state
void BlammopediaState::GoBackToMainMenu() {
    this->fadeAnimation.SetLerp(0.0, 0.5f, 0.0f, 1.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(0.0f);
    this->goBackToMainMenu = true;
}

void BlammopediaState::SetBlammoMenuItemHighlighted(int menuItemIndex) {
    // Unhighlight the previously selected item (if there was one)...
    if (this->currMenuItemIndex != NO_MENU_ITEM_INDEX) {
        TextLabel2D* prevSelection = this->blammoMenuLabels[this->currMenuItemIndex];
        prevSelection->SetScale(1.0f);
        prevSelection->SetDropShadowAmount(NO_SELECTION_DROP_SHADOW_AMT);
        float newTopLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - prevSelection->GetHeight()) / 2.0f + prevSelection->GetHeight();
        prevSelection->SetTopLeftCorner(prevSelection->GetTopLeftCorner()[0], newTopLeftY);
        prevSelection->SetColour(IDLE_COLOUR);
    }

    // Highlight the newly selected item...
    if (menuItemIndex != NO_MENU_ITEM_INDEX) {
        TextLabel2D* newSelection = this->blammoMenuLabels[menuItemIndex];
        newSelection->SetScale(SELECTION_SCALE);
        newSelection->SetDropShadowAmount(SELECTION_DROP_SHADOW_AMT);
        float newTopLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - newSelection->GetHeight()) / 2.0f + newSelection->GetHeight();
        newSelection->SetTopLeftCorner(newSelection->GetTopLeftCorner()[0], newTopLeftY);
        newSelection->SetColour(SELECTION_COLOUR);
    }

    this->currMenuItemIndex = menuItemIndex;
    // Now set the proper list as well...
    if (this->currMenuItemIndex < static_cast<int>(this->listViews.size())) {
      this->currListViewIndex = this->currMenuItemIndex;
    }
}

void BlammopediaState::SetBlammoMenuItemSelection() {
    if (this->currMenuItemIndex == BACK_MENU_ITEM_INDEX) {
        this->GoBackToMainMenu();
    }
    else {
        ItemListView* prevListSelected = this->listViews[this->currListViewIndex];
        assert(prevListSelected != NULL);
        prevListSelected->SetSelectedItemIndex(0);
        this->currMenuItemIndex = NO_MENU_ITEM_INDEX;

        // Reset the proper y coordinate for the label (since it was animating)
        TextLabel2D* newSelection = this->blammoMenuLabels[this->currListViewIndex];
        float newTopLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - newSelection->GetHeight()) / 2.0f + newSelection->GetHeight();
        newSelection->SetTopLeftCorner(newSelection->GetTopLeftCorner()[0], newTopLeftY);

        // Setup the animation for the tab...
        this->itemSelTabAnim.SetLerp(0.33f, 1.0f);
        this->itemSelTabAnim.SetInterpolantValue(0.0f);
        this->itemSelTabAnim.SetRepeat(false);
    }
}

void BlammopediaState::SetBlammoMenuItemDeselection() {
    ItemListView* prevListSelected = this->listViews[this->currListViewIndex];
    assert(prevListSelected != NULL);
    prevListSelected->SetSelectedItemIndex(ItemListView::NO_ITEM_SELECTED_INDEX);
    this->currMenuItemIndex = this->currListViewIndex;

    // Setup the animation for the tab...
    this->itemSelTabAnim.SetLerp(0.33f, 0.0f);
    this->itemSelTabAnim.SetInterpolantValue(1.0f);
    this->itemSelTabAnim.SetRepeat(false);
}

