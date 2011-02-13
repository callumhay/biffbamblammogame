/**
 * SelectLevelMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SelectLevelMenuState.h"
#include "SelectWorldMenuState.h"
#include "KeyboardHelperLabel.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"

SelectLevelMenuState::SelectLevelMenuState(GameDisplay* display, const GameWorld* world) : 
DisplayState(display), worldLabel(NULL), world(world), pressEscAlphaAnim(0.0f), 
goBackToWorldSelectMenu(false), goBackMenuMoveAnim(0.0f), goBackMenuAlphaAnim(1.0f) {
    // Build a framebuffer object for the menu
    const Camera& camera = this->display->GetCamera();
	this->menuFBO = new FBObj(camera.GetWindowWidth(), camera.GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);

	// Create the new bloom effect and set its parameters appropriately
	this->bloomEffect = new CgFxBloom(this->menuFBO);
	this->bloomEffect->SetHighlightThreshold(0.4f);
	this->bloomEffect->SetSceneIntensity(0.70f);
	this->bloomEffect->SetGlowIntensity(0.3f);
	this->bloomEffect->SetHighlightIntensity(0.1f);

    Colour titleColour(0.4f, 0.6f, 0.8f); // Steel blue
    std::stringstream worldLabelTxt;
    worldLabelTxt << "World " << this->world->GetWorldNumber() << ": " << this->world->GetName();
    this->worldLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), worldLabelTxt.str());
    this->worldLabel->SetColour(titleColour);
    this->worldLabel->SetDropShadow(Colour(0,0,0), 0.1f);

    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(0,0,0));

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

    this->fadeAnimation.SetLerp(0.0, 0.5f, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

    this->SetupLevelItems();
}

SelectLevelMenuState::~SelectLevelMenuState() {
    delete this->menuFBO;
    this->menuFBO = NULL;
    delete this->bloomEffect;
    this->bloomEffect = NULL;

    delete this->worldLabel;
    this->worldLabel = NULL;

    delete this->keyEscLabel;
    this->keyEscLabel = NULL;
}

void SelectLevelMenuState::RenderFrame(double dT) {
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
    this->worldLabel->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, camera.GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldLabel->Draw();

    // Draw the title strip
    this->DrawTitleStrip(camera);

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    // TODO


	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone) {
        this->DrawFadeOverlay(camera.GetWindowWidth(), camera.GetWindowHeight(), this->fadeAnimation.GetInterpolantValue());
    }

    glPopMatrix();
    Camera::PopWindowCoords();
  
	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu and draw it
	this->bloomEffect->Draw(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);

    if (this->goBackToWorldSelectMenu) {
        bool animDone = this->goBackMenuAlphaAnim.Tick(dT);
        animDone &= this->goBackMenuMoveAnim.Tick(dT);
        
        float moveAmt  = this->goBackMenuMoveAnim.GetInterpolantValue();  
        float alphaAmt = this->goBackMenuAlphaAnim.GetInterpolantValue();

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
            glVertex2i(camera.GetWindowWidth() + moveAmt, 0);
	        glTexCoord2i(1, 1);
            glVertex2i(camera.GetWindowWidth() + moveAmt, camera.GetWindowHeight());
	        glTexCoord2i(0, 1);
	        glVertex2i(moveAmt, camera.GetWindowHeight());
            glTexCoord2i(0, 0);
	        glVertex2i(moveAmt, 0);
        glEnd();
        this->menuFBO->GetFBOTexture()->UnbindTexture();
        
        glPopAttrib();
        glPopMatrix();
        Camera::PopWindowCoords();

        if (animDone) {
            // Switch states back to the world select menu...
            this->display->SetCurrentState(new SelectWorldMenuState(this->display, this->world));
            
            debug_opengl_state();
            return;
        }
    }
    else {
	    this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();
    }

    debug_opengl_state();
}

void SelectLevelMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    switch (pressedButton) {
        case GameControl::EscapeButtonAction:
            this->GoBackToWorldSelectMenu();
            break;

        default:
            break;
    }
}

void SelectLevelMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

void SelectLevelMenuState::DrawTitleStrip(const Camera& camera) const {

    static const float STRIP_HEIGHT      = 20;
    static const float DROP_AMT          = STRIP_HEIGHT / 5.0f;
    static const float TEXT_TO_STRIP_GAP = 20;
    static const float SIDE_TO_STRIP_GAP = 20;

    Point2D stripStartPt = this->worldLabel->GetTopLeftCorner() + 
        Vector2D(this->worldLabel->GetLastRasterWidth() + TEXT_TO_STRIP_GAP, -static_cast<int>(this->worldLabel->GetHeight()/2));

    // Drop shadow of the title strip
    glPushMatrix();
    glTranslatef(DROP_AMT, -DROP_AMT, 0);
    glBegin(GL_QUADS);
    glColor4f(0, 0, 0, 1);
    glVertex3f(stripStartPt[0], stripStartPt[1], 0.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(camera.GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(camera.GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1], 0.0f);
    glEnd();
    glPopMatrix();

    glBegin(GL_QUADS);
    glColor4f(0.4f, 0.6f, 0.8f, 1.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1], 0.0f);
    glVertex3f(stripStartPt[0], stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(camera.GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1] - STRIP_HEIGHT, 0.0f);
    glVertex3f(camera.GetWindowWidth()-SIDE_TO_STRIP_GAP, stripStartPt[1], 0.0f);
    glEnd();
}

/** 
 * Call this to indicate that we will be going back to the world select menu.
 */
void SelectLevelMenuState::GoBackToWorldSelectMenu() {
    const Camera& camera = this->display->GetCamera();
    this->goBackMenuMoveAnim.SetLerp(0.5, camera.GetWindowWidth());
    this->goBackMenuMoveAnim.SetRepeat(false);
    this->goBackMenuAlphaAnim.SetLerp(0.5, 0.0f);
    this->goBackMenuAlphaAnim.SetRepeat(false);

    this->goBackToWorldSelectMenu = true;
}

void SelectLevelMenuState::SetupLevelItems() {
    // TODO
}

const float SelectLevelMenuState::LevelMenuItem::NUM_TO_NAME_GAP = 5;

SelectLevelMenuState::LevelMenuItem::LevelMenuItem(int levelNum, const GameLevel* level, 
                                                   float width, 
                                                   const Point2D& topLeftCorner) : 
level(level), topLeftCorner(topLeftCorner) {
    assert(level != NULL);
    assert(levelNum > 0);

    std::stringstream levelNumStr;
    levelNumStr << levelNum;

    this->numLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Big), levelNumStr.str());
    this->numLabel->SetDropShadow(Colour(0.4f, 0.4f, 0.4f), 0.08f);
    this->numLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
    this->numLabel->SetTopLeftCorner(this->topLeftCorner);

    float nameLabelWidth = width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();
    this->nameLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Medium), nameLabelWidth, level->GetName());
    this->nameLabel->SetColour(Colour(0,0,0));
    float nameXPos = this->topLeftCorner[0] + NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth();
    float nameYPos = this->topLeftCorner[1] - ((this->numLabel->GetHeight() - this->nameLabel->GetHeight()) / 2.0f);
    this->nameLabel->SetTopLeftCorner(nameXPos, nameYPos);
}

SelectLevelMenuState::LevelMenuItem::~LevelMenuItem() {
    delete this->numLabel;
    this->numLabel = NULL;
    delete this->nameLabel;
    this->nameLabel = NULL;
}

void SelectLevelMenuState::LevelMenuItem::Draw(const Camera& camera, double dT) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    this->numLabel->Draw();
    this->nameLabel->Draw();
}