/**
 * SelectWorldMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SelectWorldMenuState.h"
#include "SelectLevelMenuState.h"
#include "KeyboardHelperLabel.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameProgressIO.h"

const char* SelectWorldMenuState::WORLD_SELECT_TITLE = "Select a Movement...";
const float SelectWorldMenuState::MENU_ITEM_HORIZ_GAP = 50;

SelectWorldMenuState::SelectWorldMenuState(GameDisplay* display) : 
DisplayState(display), pressEscAlphaAnim(0.0f),
worldSelectTitleLbl(NULL), keyEscLabel(NULL), 
goBackToMainMenu(false), menuFBO(NULL), 
bloomEffect(NULL), itemActivated(false), 
goToLevelSelectMoveAnim(0.0f), goToLevelSelectAlphaAnim(1.0f) {
    this->Init(0);
}

SelectWorldMenuState::SelectWorldMenuState(GameDisplay* display, const GameWorld* selectedWorld) : 
DisplayState(display), pressEscAlphaAnim(0.0f),
worldSelectTitleLbl(NULL), keyEscLabel(NULL), 
goBackToMainMenu(false), menuFBO(NULL), bloomEffect(NULL), itemActivated(false),
goToLevelSelectMoveAnim(0.0f), goToLevelSelectAlphaAnim(1.0f) {
    assert(selectedWorld != NULL);
    this->Init(selectedWorld->GetWorldNumber()-1);
}

SelectWorldMenuState::~SelectWorldMenuState() {
    delete this->worldSelectTitleLbl;
    this->worldSelectTitleLbl = NULL;

    delete this->keyEscLabel;
    this->keyEscLabel = NULL;

    delete this->menuFBO;
    this->menuFBO = NULL;
    delete this->bloomEffect;
    this->bloomEffect = NULL;

    for (std::vector<WorldSelectItem*>::iterator iter = this->worldItems.begin(); 
         iter != this->worldItems.end(); ++iter) {
        WorldSelectItem* item = *iter;
        delete item;
        item = NULL;
    }
    this->worldItems.clear();
}

void SelectWorldMenuState::RenderFrame(double dT) {
    this->pressEscAlphaAnim.Tick(dT);
    const Camera& camera = this->display->GetCamera();

    // Bind the menu's FrameBufferObject - we use this to do Bloom on the menu
	this->menuFBO->BindFBObj();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the title...
    static const int HORIZONTAL_TITLE_GAP = 20;
    static const int VERTICAL_TITLE_GAP   = 30;
    this->worldSelectTitleLbl->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, camera.GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldSelectTitleLbl->Draw();

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    // Draw the menu
    this->selectionAlphaOrangeAnim.Tick(dT);   
    this->selectionAlphaYellowAnim.Tick(dT);   
    this->selectionBorderAddYellowAnim.Tick(dT); 

    this->worldItems[this->selectedItemIdx]->DrawSelectionBorder(camera, dT, 
        this->selectionAlphaOrangeAnim.GetInterpolantValue(),
        this->selectionAlphaYellowAnim.GetInterpolantValue(),
        this->selectionBorderAddYellowAnim.GetInterpolantValue());  

    for (size_t i = 0; i < this->worldItems.size(); i++) {
        WorldSelectItem* worldItem = this->worldItems[i];
        worldItem->Draw(camera, dT);
    }

	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone || this->goBackToMainMenu) {
        this->DrawFadeOverlay(camera.GetWindowWidth(), camera.GetWindowHeight(), this->fadeAnimation.GetInterpolantValue());
    }

    glPopMatrix();
    Camera::PopWindowCoords();
  
	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu and draw it
	this->bloomEffect->Draw(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);
	

    if (this->itemActivated) {
        bool animDone = this->goToLevelSelectAlphaAnim.Tick(dT);
        animDone &= this->goToLevelSelectMoveAnim.Tick(dT);

        float moveAmt  = this->goToLevelSelectMoveAnim.GetInterpolantValue();  
        float alphaAmt = this->goToLevelSelectAlphaAnim.GetInterpolantValue();

	    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera::PushWindowCoords();
	    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
	    glLoadIdentity();

        glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->menuFBO->GetFBOTexture()->BindTexture();
        glColor4f(1,1,1,alphaAmt);
        glBegin(GL_QUADS);
	        glTexCoord2i(1, 0);
            glVertex2i(camera.GetWindowWidth() - moveAmt, 0);
	        glTexCoord2i(1, 1);
            glVertex2i(camera.GetWindowWidth() - moveAmt, camera.GetWindowHeight());
	        glTexCoord2i(0, 1);
	        glVertex2i(-moveAmt, camera.GetWindowHeight());
            glTexCoord2i(0, 0);
	        glVertex2i(-moveAmt, 0);
        glEnd();
        this->menuFBO->GetFBOTexture()->UnbindTexture();
        
        glPopAttrib();
        glPopMatrix();
        Camera::PopWindowCoords();
        
        if (animDone) {
            // Go to the level select for the currently selected world
            this->display->SetCurrentState(new SelectLevelMenuState(this->display, this->worldItems[this->selectedItemIdx]->GetWorld()));
            return;
        }
    }
    else {
        this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();
    }

    if (fadeDone && this->goBackToMainMenu) {
        // Go back to the main menu now
        this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::MainMenu, this->display));
        return;
    }

    debug_opengl_state();
}

void SelectWorldMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    switch (pressedButton) {
        case GameControl::EscapeButtonAction:
            this->GoBackToMainMenu();
            break;

        case GameControl::LeftButtonAction:
            this->worldItems[this->selectedItemIdx]->SetIsSelected(false);
            this->selectedItemIdx -= 1;
            if (this->selectedItemIdx < 0) {
                this->selectedItemIdx = this->worldItems.size()-1;
            }
            this->worldItems[this->selectedItemIdx]->SetIsSelected(true);
            break;

        case GameControl::RightButtonAction:
            this->worldItems[this->selectedItemIdx]->SetIsSelected(false);
            this->selectedItemIdx += 1;
            this->selectedItemIdx %= this->worldItems.size();
            this->worldItems[this->selectedItemIdx]->SetIsSelected(true);
            break;

        case GameControl::EnterButtonAction:
            {
                const Camera& camera = this->display->GetCamera();
                this->itemActivated = true;
                this->goToLevelSelectAlphaAnim.SetLerp(0.5, 0.0);
                this->goToLevelSelectAlphaAnim.SetRepeat(false);
                this->goToLevelSelectMoveAnim.SetLerp(0.5, camera.GetWindowWidth());
                this->goToLevelSelectMoveAnim.SetRepeat(false);
            }

            break;
#ifdef _DEBUG
        case GameControl::SpecialCheatButtonAction:
            // Unlock all levels in the game...
            // (You need to restart the game for this to take effect)
            GameProgressIO::SaveFullProgressOfGame(this->display->GetModel());
            break;
#endif

        default:
            break;
    }

}

void SelectWorldMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

void SelectWorldMenuState::GoBackToMainMenu() {
    this->fadeAnimation.SetLerp(0.0, 0.5f, 0.0f, 1.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(0.0f);
    this->goBackToMainMenu = true;
}

void SelectWorldMenuState::Init(int selectedIdx) {

    this->worldSelectTitleLbl = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), SelectWorldMenuState::WORLD_SELECT_TITLE);
    this->worldSelectTitleLbl->SetColour(Colour(0.4f, 0.6f, 0.8f)); // Steel blue
    this->worldSelectTitleLbl->SetDropShadow(Colour(0,0,0), 0.1f);
    
    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(0,0,0));

    this->fadeAnimation.SetLerp(0.0, 0.5f, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

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

    // Setup the selection animation
    timeVals.clear();
    timeVals.push_back(0.0);
    timeVals.push_back(0.5);
    timeVals.push_back(1.0);
    alphaVals.clear();
    alphaVals.push_back(0.0f);
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaOrangeAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaOrangeAnim.SetRepeat(true);

    // Build a framebuffer object for the menu
    const Camera& camera = this->display->GetCamera();
	this->menuFBO = new FBObj(camera.GetWindowWidth(), camera.GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);

	// Create the new bloom effect and set its parameters appropriately
	this->bloomEffect = new CgFxBloom(this->menuFBO);
	this->bloomEffect->SetHighlightThreshold(0.4f);
	this->bloomEffect->SetSceneIntensity(0.70f);
	this->bloomEffect->SetGlowIntensity(0.3f);
	this->bloomEffect->SetHighlightIntensity(0.1f);

    // Setup the world items for the GUI menu
    const GameModel* gameModel = this->display->GetModel();
    const std::vector<GameWorld*>& gameWorlds = gameModel->GetGameWorlds();
    assert(!gameWorlds.empty());

    static const float MAX_ITEM_SIZE = 256;
    
    static const float MENU_VERT_GAP = 50;
    float menuItemSize = std::min<float>((camera.GetWindowWidth() / gameWorlds.size()) - MENU_ITEM_HORIZ_GAP * (1 + gameWorlds.size()), 
        (camera.GetWindowHeight() - this->worldSelectTitleLbl->GetHeight() - this->keyEscLabel->GetHeight() - 2*MENU_VERT_GAP));
    menuItemSize = std::min<float>(menuItemSize, MAX_ITEM_SIZE);

    float yCoord = camera.GetWindowHeight() - ((camera.GetWindowHeight() - menuItemSize) / 2.0f);
    float xCoord = (camera.GetWindowWidth() - gameWorlds.size()*menuItemSize - MENU_ITEM_HORIZ_GAP * (gameWorlds.size()-1)) / 2.0f;

    this->worldItems.reserve(gameWorlds.size());
    for (size_t i = 0; i < gameWorlds.size(); i++) {
        const GameWorld* currGameWorld = gameWorlds[i];
        WorldSelectItem* menuItem = new WorldSelectItem(currGameWorld, i+1, menuItemSize);
        menuItem->SetTopLeftCorner(xCoord, yCoord);
        this->worldItems.push_back(menuItem);

        xCoord += menuItemSize + MENU_ITEM_HORIZ_GAP;
    }
    this->selectedItemIdx = selectedIdx;
    this->worldItems[this->selectedItemIdx]->SetIsSelected(true);

    timeVals.clear();
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    std::vector<float> scaleVals;
    scaleVals.push_back(0.0f);
    scaleVals.push_back(2*MENU_ITEM_HORIZ_GAP);
    this->selectionBorderAddYellowAnim.SetLerp(timeVals, scaleVals);
    this->selectionBorderAddYellowAnim.SetRepeat(true);

    alphaVals.clear();
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaYellowAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaYellowAnim.SetRepeat(true);
}

SelectWorldMenuState::WorldSelectItem::WorldSelectItem(const GameWorld* world, size_t worldNumber, float size) : 
worldNumber(worldNumber), gameWorld(world), image(NULL), label(NULL), 
selectedLabel(NULL), baseSize(size), isSelected(false), sizeAnim(size) {
    assert(world != NULL);

    this->image = ResourceManager::GetInstance()->GetImgTextureResource(world->GetImageFilepath(), Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->image != NULL);
    
    std::stringstream labelTextStream;
    labelTextStream << world->GetName();
    
    float slightlyBiggerSize = size + 0.75f * SelectWorldMenuState::MENU_ITEM_HORIZ_GAP;
    this->label = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        slightlyBiggerSize, labelTextStream.str());
    this->label->SetColour(Colour(0, 0, 0));
    
    this->selectedLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        slightlyBiggerSize, labelTextStream.str());
    this->selectedLabel->SetDropShadow(Colour(0,0,0), 0.08f);
    this->selectedLabel->SetColour(Colour(1.0f, 0.4f, 0.0f));
    this->selectedLabel->SetScale(1.1f);

    this->sizeAnim.SetInterpolantValue(this->baseSize);
    this->sizeAnim.SetRepeat(false);
}

SelectWorldMenuState::WorldSelectItem::~WorldSelectItem() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->image);
    UNUSED_VARIABLE(success);
    assert(success);
    
    delete this->label;
    this->label = NULL;
    delete this->selectedLabel;
    this->selectedLabel = NULL;
}

void SelectWorldMenuState::WorldSelectItem::SetIsSelected(bool isSelected) {
    this->isSelected = isSelected;
    
    if (this->isSelected) {
        this->sizeAnim.SetLerp(0.35, this->baseSize + SelectWorldMenuState::MENU_ITEM_HORIZ_GAP);
    }
    else {
        this->sizeAnim.SetLerp(0.35, this->baseSize);
    }
    this->sizeAnim.SetRepeat(false);
}

void SelectWorldMenuState::WorldSelectItem::Draw(const Camera& camera, double dT) {
    UNUSED_PARAMETER(camera);

    this->sizeAnim.Tick(dT);
    float currSize = this->sizeAnim.GetInterpolantValue();

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(this->topLeftCorner[0] + this->baseSize/2, this->topLeftCorner[1] - this->baseSize/2, 0);
    glScalef(currSize, currSize, 1);

    // Draw the image for the world
    this->image->BindTexture();
    glColor4f(1,1,1,1);
    GeometryMaker::GetInstance()->DrawQuad();
    this->image->UnbindTexture();

    // Draw the outlines
    glLineWidth(2.0f);
	glColor4f(0, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.5, -0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(-0.5, -0.5);
	glEnd();
    glPopMatrix();

    TextLabel2DFixedWidth* currLabel = NULL;
    if (this->isSelected) {
        currLabel = this->selectedLabel;
    }
    else {
        currLabel = this->label;
    }

    static const float VERTICAL_GAP = 10;
    float diff = (currSize - this->baseSize) / 2.0f;
    float labelX = (this->topLeftCorner[0] - diff) + ((currSize - currLabel->GetWidth()) / 2.0f);
    float labelY = (this->topLeftCorner[1] + diff) - currSize - VERTICAL_GAP;
    currLabel->SetTopLeftCorner(labelX, labelY);
    currLabel->Draw();

    glPopAttrib();
}

void SelectWorldMenuState::WorldSelectItem::DrawSelectionBorder(const Camera& camera, double dT, 
                                                                float orangeAlpha, float yellowAlpha, 
                                                                float sizeAmt) {
	UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    float currSize = this->sizeAnim.GetInterpolantValue();

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(this->topLeftCorner[0] + this->baseSize/2, this->topLeftCorner[1] - this->baseSize/2, 0);

    glPushMatrix();
    glScalef(currSize + sizeAmt, currSize + sizeAmt, 1.0f);
    glColor4f(1, 1, 0, yellowAlpha);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
    
    glScalef(currSize + sizeAmt/2, currSize + sizeAmt/2, 1.0f);
    glColor4f(1, 0.65f, 0, orangeAlpha);
    GeometryMaker::GetInstance()->DrawQuad();

    glPopMatrix();

    glPopAttrib();
}