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
#include "GameViewConstants.h"

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
bloomEffect(NULL), itemActivated(false), starryBG(NULL), padlockTex(NULL),
goToLevelSelectMoveAnim(0.0f), goToLevelSelectAlphaAnim(1.0f), starTexture(NULL) {
    this->Init(0);
}

SelectWorldMenuState::SelectWorldMenuState(GameDisplay* display, const GameWorld* selectedWorld) : 
DisplayState(display), pressEscAlphaAnim(0.0f),
worldSelectTitleLbl(NULL), keyEscLabel(NULL), starryBG(NULL), padlockTex(NULL),
goBackToMainMenu(false), menuFBO(NULL), bloomEffect(NULL), itemActivated(false),
goToLevelSelectMoveAnim(0.0f), goToLevelSelectAlphaAnim(1.0f), starTexture(NULL) {
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

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockTex);
    assert(success);
    UNUSED_VARIABLE(success);
}

void SelectWorldMenuState::RenderFrame(double dT) {
    this->pressEscAlphaAnim.Tick(dT);
    const Camera& camera = this->display->GetCamera();

    // Bind the menu's FrameBufferObject - we use this to do Bloom on the menu
	this->menuFBO->BindFBObj();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the starry background...
    this->starryBG->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()));
    this->starryBG->UnbindTexture();

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the title...
    static const int HORIZONTAL_TITLE_GAP = 60;
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

	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

        float screenWidth = camera.GetWindowWidth();
        float screenHeight = camera.GetWindowHeight();
        float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(this->starryBG->GetWidth()));
        float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(this->starryBG->GetHeight()));

        this->starryBG->BindTexture();
        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
	        glVertex2f(-moveAmt, 0);
            glTexCoord2f(2*totalTexU, 0);
	        glVertex2f(2*camera.GetWindowWidth() - moveAmt, 0);
            glTexCoord2f(2*totalTexU, totalTexV);
            glVertex2f(2*camera.GetWindowWidth() - moveAmt, camera.GetWindowHeight());
	        glTexCoord2f(0, totalTexV);
            glVertex2f(-moveAmt, camera.GetWindowHeight());
        glEnd();
        this->starryBG->UnbindTexture();

        /// <summary> Default constructor. </summary>
        /// <remarks> Beowulf, 07/01/2013. </remarks>
        this->menuFBO->GetFBOTexture()->BindTexture();
        glColor4f(1,1,1,alphaAmt);
        glBegin(GL_QUADS);
	        glTexCoord2f(1, 0);
            glVertex2f(camera.GetWindowWidth() - moveAmt, 0);
	        glTexCoord2f(1, 1);
            glVertex2f(camera.GetWindowWidth() - moveAmt, camera.GetWindowHeight());
	        glTexCoord2f(0, 1);
	        glVertex2f(-moveAmt, camera.GetWindowHeight());
            glTexCoord2f(0, 0);
	        glVertex2f(-moveAmt, 0);
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

void SelectWorldMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

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

    // Load background texture
    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear));
    assert(this->starryBG != NULL);

    this->padlockTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_PADLOCK, Texture::Trilinear));
    assert(this->padlockTex != NULL);

    this->starTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->starTexture != NULL);

    this->worldSelectTitleLbl = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), SelectWorldMenuState::WORLD_SELECT_TITLE);
    this->worldSelectTitleLbl->SetColour(Colour(0.4f, 0.6f, 0.8f)); // Steel blue
    this->worldSelectTitleLbl->SetDropShadow(Colour(1,1,1), 0.05f);
    
    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));

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

    int furthestWorldIdx, furthestLevelIdx;
    this->display->GetModel()->GetFurthestProgressWorldAndLevel(furthestWorldIdx, furthestLevelIdx);

    this->worldItems.reserve(gameWorlds.size());
    for (int i = 0; i < static_cast<int>(gameWorlds.size()); i++) {

        const GameWorld* currGameWorld = gameWorlds[i];
        bool isLocked = (furthestWorldIdx < i);

        WorldSelectItem* menuItem = new WorldSelectItem(this, currGameWorld, i+1, menuItemSize, isLocked);
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

SelectWorldMenuState::WorldSelectItem::WorldSelectItem(SelectWorldMenuState* state, const GameWorld* world,
                                                       size_t worldNumber, float size, bool isLocked) :

state(state), worldNumber(worldNumber), gameWorld(world), image(NULL), label(NULL), starTotalLabel(NULL),
selectedLabel(NULL), baseSize(size), isSelected(false), isLocked(isLocked), sizeAnim(size) {

    assert(world != NULL);
    
    std::string labelTextStr;
    Colour labelColour(1.0f, 0.65f, 0.0f);
    Colour labelSelectedColour(1, 1, 0);

    if (this->isLocked) {
        labelTextStr = std::string("???");
        labelColour = Colour(0.5f, 0.5f, 0.5f);
        labelSelectedColour = Colour(0.5f, 0.5f, 0.5f);
    }
    else {
        labelTextStr = world->GetName();
    }

    float slightlyBiggerSize = size + 0.75f * SelectWorldMenuState::MENU_ITEM_HORIZ_GAP;
    this->label = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        slightlyBiggerSize, labelTextStr);
    this->label->SetColour(labelColour);
    
    this->selectedLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        slightlyBiggerSize, labelTextStr);
    this->selectedLabel->SetDropShadow(Colour(0,0,0), 0.08f);
    this->selectedLabel->SetColour(labelSelectedColour);
    this->selectedLabel->SetScale(1.1f);

    this->sizeAnim.SetInterpolantValue(this->baseSize);
    this->sizeAnim.SetRepeat(false);

    // Create a label for the total number of stars collected in the world represented by this item
    int totalNumStarsCollected = world->GetNumStarsCollectedInWorld();
    int totalNumStars = world->GetTotalAchievableStarsInWorld();

    std::stringstream totalNumStarsTxt;
    totalNumStarsTxt << totalNumStarsCollected << "/" << totalNumStars;
    this->starTotalLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium), totalNumStarsTxt.str());
    this->starTotalLabel->SetColour(Colour(1,1,1));
    this->starTotalLabel->SetScale(0.85f);

    this->image = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(world->GetImageFilepath(), Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->image != NULL);

    Colour outlineColour(0,0,0);
    if (this->isLocked) {
        outlineColour = Colour(0.2f, 0.2f, 0.2f);
    }
}

SelectWorldMenuState::WorldSelectItem::~WorldSelectItem() {

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->image);
    UNUSED_VARIABLE(success);
    assert(success);
    
    delete this->label;
    this->label = NULL;
    delete this->selectedLabel;
    this->selectedLabel = NULL;
    delete this->starTotalLabel;
    this->starTotalLabel = NULL;
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

    TextLabel2DFixedWidth* currLabel = NULL;
    if (this->isSelected) {
        currLabel = this->selectedLabel;
    }
    else {
        currLabel = this->label;
    }

    this->sizeAnim.Tick(dT);
    float currSize = this->sizeAnim.GetInterpolantValue();


    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1,1,1,1);

    glPushMatrix();
    glTranslatef(this->topLeftCorner[0] + this->baseSize/2, this->topLeftCorner[1] - this->baseSize/2, 0);

    glPushMatrix();
    glScalef(currSize, currSize, 1);
    
    // Draw the world image
    this->state->greyscaleEffect.SetColourTexture(this->image);
    this->state->greyscaleEffect.Draw(camera, GeometryMaker::GetInstance()->GetQuadDL());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Colour outlineColour(0,0,0);
    if (this->isLocked) {
        outlineColour = Colour(0.2f, 0.2f, 0.2f);
    }

    // Draw the outlines
    glLineWidth(2.0f);
    glColor4f(outlineColour.R(), outlineColour.G(), outlineColour.B(), 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.5, -0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(-0.5, -0.5);
	glEnd();

    glPopMatrix();

    if (this->isLocked) {
        // When locked, draw the padlock over the world image
        glColor4f(1,1,1,1);
        glPushMatrix();
        float fractSize = currSize / 1.75f;
        glScalef(fractSize, fractSize, 1);
        this->state->padlockTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        this->state->padlockTex->UnbindTexture();
        glPopMatrix();
    }

    glPopMatrix();

    static const float TITLE_VERTICAL_GAP = 10;
    float diff = (currSize - this->baseSize) / 2.0f;
    float labelX = (this->topLeftCorner[0] - diff) + ((currSize - currLabel->GetWidth()) / 2.0f);
    float labelY = (this->topLeftCorner[1] + diff) - currSize - TITLE_VERTICAL_GAP;
    currLabel->SetTopLeftCorner(labelX, labelY);
    currLabel->Draw();

    if (!this->isLocked) {
        const Colour& starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;

        static const float STAR_VERTICAL_GAP = 1.75f * TITLE_VERTICAL_GAP;
        static const float STAR_ICON_HORIZONTAL_GAP = 5;
        float starSize   = this->starTotalLabel->GetHeight();
        float starLabelX = (this->topLeftCorner[0] - diff) + 
            ((currSize - (this->starTotalLabel->GetLastRasterWidth() + STAR_ICON_HORIZONTAL_GAP + starSize)) / 2.0f);
        float starLabelY = (this->topLeftCorner[1] + diff) - currSize - TITLE_VERTICAL_GAP - currLabel->GetHeight() - STAR_VERTICAL_GAP;
        
        this->state->starTexture->BindTexture();
        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
        glPushMatrix();
        glTranslatef(starLabelX, starLabelY - starSize/2.0f, 0);
        glScalef(starSize, starSize, 1.0f); 
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
        this->state->starTexture->UnbindTexture();

        this->starTotalLabel->SetTopLeftCorner(starLabelX + STAR_ICON_HORIZONTAL_GAP + starSize, starLabelY);
        this->starTotalLabel->Draw();
    }

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