/**
 * SelectWorldMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SelectWorldMenuState.h"
#include "KeyboardHelperLabel.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"

const char* SelectWorldMenuState::WORLD_SELECT_TITLE = "Select a World...";

SelectWorldMenuState::SelectWorldMenuState(GameDisplay* display) : DisplayState(display), pressEscAlphaAnim(0.0f),
worldSelectTitleLbl(NULL), keyEscLabel(NULL), goBackToMainMenu(false), menuFBO(NULL), bloomEffect(NULL) {

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

    static const float MAX_ITEM_SIZE = 300;
    static const float MENU_ITEM_HORIZ_GAP = 30;
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

    for (std::vector<WorldSelectItem*>::iterator iter = this->worldItems.begin(); iter != this->worldItems.end(); ++iter) {
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
    static const int VERTICAL_TITLE_GAP   = 20;
    this->worldSelectTitleLbl->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, camera.GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldSelectTitleLbl->Draw();

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    // Draw the menu
    for (std::vector<WorldSelectItem*>::iterator iter = this->worldItems.begin(); 
         iter != this->worldItems.end(); ++iter) {

        WorldSelectItem* worldItem = *iter;
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
	this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

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



SelectWorldMenuState::WorldSelectItem::WorldSelectItem(const GameWorld* world, size_t worldNumber, float size) : 
worldNumber(worldNumber), gameWorld(world), image(NULL), label(NULL), selectedLabel(NULL), size(size) {
    assert(world != NULL);

    this->image = ResourceManager::GetInstance()->GetImgTextureResource(world->GetImageFilepath(), Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->image != NULL);
    
    std::stringstream labelTextStream;
    labelTextStream << "World " << worldNumber << ": " << world->GetName();
    
    this->label = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        size, labelTextStream.str());
    this->label->SetColour(Colour(0, 0, 0));
    
    this->selectedLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),
        size, labelTextStream.str());
    this->selectedLabel->SetDropShadow(Colour(0,0,0), 0.08f);
    this->selectedLabel->SetColour(Colour(1.0f, 0.4f, 0.0f));

}

SelectWorldMenuState::WorldSelectItem::~WorldSelectItem() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->image);
    assert(success);
    
    delete this->label;
    this->label = NULL;
    delete this->selectedLabel;
    this->selectedLabel = NULL;
}

void SelectWorldMenuState::WorldSelectItem::Draw(const Camera& camera, double dT) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(this->topLeftCorner[0], this->topLeftCorner[1], 0);

    float halfSize = this->size  / 2;
    glTranslatef(halfSize, -halfSize, 0);
    glScalef(this->size, this->size, 1);

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

    static const float VERTICAL_GAP = 10;
    float labelX = this->topLeftCorner[0] + (size - this->label->GetWidth()) / 2.0f;
    float labelY = this->topLeftCorner[1] - size - VERTICAL_GAP;
    this->label->SetTopLeftCorner(labelX, labelY);
    this->label->Draw();

    glPopAttrib();
}