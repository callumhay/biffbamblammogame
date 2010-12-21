/**
 * BlammopediaState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BlammopediaState.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "ItemListView.h"
#include "GameFontAssetsManager.h"

#include "../ResourceManager.h"
#include "../Blammopedia.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../GameModel/GameItemFactory.h"

const int BlammopediaState::ITEM_NAME_BORDER_SIZE = 10;
const int BlammopediaState::TOTAL_MENU_HEIGHT     = 60;

BlammopediaState::BlammopediaState(GameDisplay* display) : 
DisplayState(display), currMenuItemIndex(NO_MENU_ITEM_INDEX), goBackToMainMenu(false) {
    const Camera& camera = this->display->GetCamera();

    this->fadeAnimation.SetLerp(0.0, 0.5f, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

    Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
    assert(blammopedia != NULL);
	this->listViews.reserve(3);
	this->listViews.push_back(this->BuildGameItemsListView(blammopedia));
	this->listViews.push_back(this->BuildGameBlockListView(blammopedia));
	this->listViews.push_back(this->BuildStatusEffectListView(blammopedia));
    this->currListViewIndex = 0;

    this->selectedItemNameLbl.SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium));
    this->selectedItemNameLbl.SetColour(Colour(1, 0.65f, 0));
    this->selectedItemNameLbl.SetDropShadow(Colour(0,0,0), 0.1f);

    this->backMenuItem.SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    this->backMenuItem.SetText("Back");
    this->backMenuItem.SetColour(Colour(0.5f, 0.5f, 0.5f));
    this->backMenuItem.SetDropShadow(Colour(0,0,0), 0.07f);
    float topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - this->backMenuItem.GetHeight()) / 2.0f + this->backMenuItem.GetHeight();
    this->backMenuItem.SetTopLeftCorner(Point2D(BlammopediaState::ITEM_NAME_BORDER_SIZE, topLeftY));

    this->itemListMenuItem.SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small));
    this->itemListMenuItem.SetText("Items");
    this->itemListMenuItem.SetColour(Colour(0.5f, 0.5f, 0.5f));
    this->itemListMenuItem.SetDropShadow(Colour(0,0,0), 0.07f);
    float startMenuItemsX = camera.GetWindowWidth() / 2 - this->itemListMenuItem.GetLastRasterWidth();
    topLeftY = (BlammopediaState::TOTAL_MENU_HEIGHT - this->itemListMenuItem.GetHeight()) / 2.0f + this->itemListMenuItem.GetHeight();
    this->itemListMenuItem.SetTopLeftCorner(Point2D(startMenuItemsX, topLeftY));

    debug_opengl_state();
}

BlammopediaState::~BlammopediaState() {
	for (std::vector<ItemListView*>::iterator iter = this->listViews.begin(); iter != this->listViews.end(); ++iter) {
		ItemListView* currListView = *iter;
		delete currListView;
		currListView = NULL;
	}
	this->listViews.clear();
}

void BlammopediaState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();
    ItemListView* currListView = this->GetCurrentListView();
    assert(currListView != NULL);
    ItemListView::ListItem* currItem = currListView->GetSelectedItem();
    assert(currItem != NULL);

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw the splash screen for the current world...
	Camera::PushWindowCoords();

	glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
	glLoadIdentity();

    // Now draw the currently selected list of the blammopedia...
    glPushMatrix();
    glTranslatef(0, camera.GetWindowHeight(), 0);
    currListView->Draw(dT, camera);
    glPopMatrix();


    // If the user is currently selecting stuff from the menu then we grey out the list view
    if (this->currMenuItemIndex != NO_MENU_ITEM_INDEX) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2i(0, TOTAL_MENU_HEIGHT);
        glVertex2i(camera.GetWindowWidth(), TOTAL_MENU_HEIGHT);
        glVertex2i(camera.GetWindowWidth(), camera.GetWindowHeight());
        glVertex2i(0, camera.GetWindowHeight());
        glEnd();
    }

    // Draw the bottom menu background
    glColor4f(0.49f, 0.98f, 1.0f, 0.75f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(camera.GetWindowWidth(), 0);
    glVertex2i(camera.GetWindowWidth(), TOTAL_MENU_HEIGHT);
    glVertex2i(0, TOTAL_MENU_HEIGHT);
    glEnd();
    glColor4f(0, 0, 0, 0.8f);
    glBegin(GL_LINES);
    glVertex2i(camera.GetWindowWidth(), TOTAL_MENU_HEIGHT);
    glVertex2i(0, TOTAL_MENU_HEIGHT);
    glEnd();

    // Draw the name of the currently selected item in the active blammopedia list...
    // If the item is locked we change the colour value to something that looks neutral
    if (currItem->GetIsLocked()) {
        this->selectedItemNameLbl.SetColour(Colour(0.66f, 0.66f, 0.66f));
    }
    else {
        this->selectedItemNameLbl.SetColour(Colour(1.0f, 0.65f, 0.0f));
    }

    this->selectedItemNameLbl.SetText(currItem->GetName());
    this->selectedItemNameLbl.SetTopLeftCorner(Point2D(camera.GetWindowWidth() - 
        this->selectedItemNameLbl.GetLastRasterWidth() - BlammopediaState::ITEM_NAME_BORDER_SIZE,
        this->selectedItemNameLbl.GetHeight() + BlammopediaState::ITEM_NAME_BORDER_SIZE));
    this->selectedItemNameLbl.Draw();
    
    this->backMenuItem.Draw();
    this->itemListMenuItem.Draw();

    glPopMatrix();

    Camera::PopWindowCoords();
    glPopAttrib();

	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone) {
		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
                                                         ColourRGBA(1, 1, 1, this->fadeAnimation.GetInterpolantValue()));
		glPopAttrib();
    }
    else if (this->goBackToMainMenu) {
        // Go back to the main menu now
        this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::MainMenu, this->display));
        return;
    }

    debug_opengl_state();
}

void BlammopediaState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    
    ItemListView* currList = this->GetCurrentListView();
    if (currList == NULL) {
        return;
    }

    // Only give the list a button event when the list has focus and not the blammopedia menu
    if (this->currMenuItemIndex == NO_MENU_ITEM_INDEX) {
        currList->ButtonPressed(pressedButton);
    }
    else {
        // Turn off the currently active list from showing a selected item
        currList->SetSelectedItemIndex(-1);

        // TODO: select the appropriate blammopedia menu entry and change the list being displayed...
        switch (pressedButton) {
            case GameControl::EnterButtonAction:
                currList->SetSelectedItemIndex(0);
                this->currMenuItemIndex = NO_MENU_ITEM_INDEX;
                break;
            case GameControl::EscapeButtonAction:
                // This case is dealt with later in the function
                break;
            case GameControl::LeftButtonAction:
                // TODO
                break;
            case GameControl::RightButtonAction:
                // TODO
                break;

            default:
                break;
        }
    }

    // If we escape from a list then we go into the blammopedia menu, if we're already in the menu
    // then we go to the 'back' option, if we're already on the back option then we go back to the main menu
    if (pressedButton == GameControl::EscapeButtonAction) {
        if (this->currMenuItemIndex == NO_MENU_ITEM_INDEX) {
            this->currMenuItemIndex = static_cast<int>(this->currListViewIndex);
        }
        else {
            if (this->currMenuItemIndex != BACK_MENU_ITEM_INDEX) {
                this->currMenuItemIndex = BACK_MENU_ITEM_INDEX;
            }
            else {
                // Go back to the main menu...
                this->GoBackToMainMenu();
            }   
        }
    }
}

void BlammopediaState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

ItemListView* BlammopediaState::BuildGameItemsListView(Blammopedia* blammopedia) const {
	const Camera& camera = this->display->GetCamera();
	ItemListView* itemsListView = new ItemListView(camera.GetWindowWidth());
	
	// Add each item in the game to the list... check each one to see if it has been unlocked,
	// if not then just place a 'locked' texture...
    std::string currName;
    const Blammopedia::ItemEntryMap& itemEntries = blammopedia->GetItemEntries();
    for (Blammopedia::ItemEntryMapConstIter iter = itemEntries.begin(); iter != itemEntries.end(); ++iter) {
        Blammopedia::ItemEntry* itemEntry = iter->second;
        const Texture2D* texture = blammopedia->GetLockedItemTexture();
        if (!itemEntry->GetIsLocked()) {
            texture  = itemEntry->GetItemTexture();
            currName = itemEntry->GetName();
        }
        else {
            currName = "Locked!";
        }
        itemsListView->AddItem(currName, texture, itemEntry->GetIsLocked());
    }

    itemsListView->SetSelectedItemIndex(0);
	return itemsListView;
}

ItemListView* BlammopediaState::BuildGameBlockListView(Blammopedia* blammopedia) const {
	const Camera& camera = this->display->GetCamera();
	ItemListView* blockListView = new ItemListView(camera.GetWindowWidth());


	//assert(false);
	return blockListView;
}

ItemListView* BlammopediaState::BuildStatusEffectListView(Blammopedia* blammopedia) const {
	const Camera& camera = this->display->GetCamera();
	ItemListView* statusListView = new ItemListView(camera.GetWindowWidth());


	//assert(false);
	return statusListView;
}

// Sets the fade to fade to white and says that after the fade is done we'll switch
// back to the main menu state
void BlammopediaState::GoBackToMainMenu() {
    this->fadeAnimation.SetLerp(0.0, 0.5f, 0.0f, 1.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(0.0f);
    this->goBackToMainMenu = true;
}