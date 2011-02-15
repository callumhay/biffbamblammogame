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
#include "GameViewConstants.h"
#include "GameDisplay.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/StringHelper.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"

SelectLevelMenuState::SelectLevelMenuState(GameDisplay* display, const GameWorld* world) : 
DisplayState(display), worldLabel(NULL), world(world), pressEscAlphaAnim(0.0f), 
goBackToWorldSelectMenu(false), goBackMenuMoveAnim(0.0f), goBackMenuAlphaAnim(1.0f), starTexture(NULL) {

    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);

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

    for (size_t i = 0; i < this->levelItems.size(); i++) {
        LevelMenuItem* levelItem = this->levelItems.at(i);
        delete levelItem;
        levelItem = NULL;
    }
    this->levelItems.clear();

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
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
    this->worldLabel->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, camera.GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldLabel->Draw();

    // Draw the title strip
    this->DrawTitleStrip(camera);

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    this->menuFBO->UnbindFBObj();
	// Do bloom on most of the screen then bind the FBO again and draw the stuff we don't
    // want the bloom applied to...
	this->bloomEffect->Draw(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);
    this->menuFBO->BindFBObj();
    // Draw the menu
    this->DrawLevelSelectMenu(camera, dT);

	// Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone) {
        this->DrawFadeOverlay(camera.GetWindowWidth(), camera.GetWindowHeight(), this->fadeAnimation.GetInterpolantValue());
    }

    glPopMatrix();
    Camera::PopWindowCoords();
    this->menuFBO->UnbindFBObj();

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
 * Draw the menu of level selection items, including the selected item which has a glowing border
 * drawn around it.
 */
void SelectLevelMenuState::DrawLevelSelectMenu(const Camera& camera, double dT) {
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the currently selected level item's selection border
    LevelMenuItem* selectedItem = this->levelItems[this->selectedItem];
    assert(selectedItem != NULL);
    static const float BORDER_GAP = 10;
    float leftX  = selectedItem->GetTopLeftCorner()[0] - BORDER_GAP;
    float rightX = selectedItem->GetTopLeftCorner()[0] + selectedItem->GetWidth() + BORDER_GAP;
    float topY = selectedItem->GetTopLeftCorner()[1] + BORDER_GAP;
    float bottomY = selectedItem->GetTopLeftCorner()[1] - selectedItem->GetHeight() - BORDER_GAP;

    // TODO... animation...


    glColor4f(0, 0, 0, 1);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(leftX, topY);
    glVertex2f(leftX, bottomY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX, topY);
    glEnd();


    // Draw all the level selection items
    for (size_t i = 0; i < this->levelItems.size(); i++) {
        this->levelItems[i]->Draw(camera, dT);
    }
    glPopAttrib();
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
    const Camera& camera = this->display->GetCamera();

    static const int TITLE_TO_ITEM_Y_GAP_SIZE = 80;
    static const int SIDE_TO_ITEM_GAP_SIZE    = 60;
    static const int ITEM_X_GAP_SIZE          = 40;
    static const int ITEM_Y_GAP_SIZE          = 50;
    static const int MIN_ITEM_SIZE            = 180;

    float amtForItems = camera.GetWindowWidth() - 2 * SIDE_TO_ITEM_GAP_SIZE;
    
    float itemWidth = 0;
    int numItemsPerRow = 5; 
    while (itemWidth < MIN_ITEM_SIZE && numItemsPerRow >= 2) {
        numItemsPerRow--;
        itemWidth = static_cast<float>(amtForItems - (ITEM_X_GAP_SIZE * (numItemsPerRow-1))) / 
                    static_cast<float>(numItemsPerRow);
    }
    assert(numItemsPerRow >= 2);
    
    float itemX = SIDE_TO_ITEM_GAP_SIZE;
    float itemY = camera.GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - TITLE_TO_ITEM_Y_GAP_SIZE;

    // Build the menu items for level selection
    int tempCount = 0;
    const std::vector<GameLevel*>& levels = this->world->GetAllLevelsInWorld();
    for (size_t levelIdx = 0; levelIdx < levels.size(); levelIdx++) {
        const GameLevel* currLevel = levels.at(levelIdx);
        assert(currLevel != NULL);

        LevelMenuItem* levelItem = new LevelMenuItem(levelIdx+1, currLevel, itemWidth, 
            Point2D(itemX, itemY), this->starTexture);
        this->levelItems.push_back(levelItem);
        
        tempCount++;
        if (tempCount == numItemsPerRow) {
            itemY -= ITEM_Y_GAP_SIZE + levelItem->GetHeight();
            tempCount = 0;
        }
        itemX += itemWidth + ITEM_X_GAP_SIZE;
    }
    
    assert(!this->levelItems.empty());
    this->selectedItem = 0;
}

const float SelectLevelMenuState::LevelMenuItem::NUM_TO_NAME_GAP = 8;
const float SelectLevelMenuState::LevelMenuItem::NUM_TO_HIGH_SCORE_Y_GAP = 5;
const float SelectLevelMenuState::LevelMenuItem::HIGH_SCORE_TO_STAR_Y_GAP = 3;

SelectLevelMenuState::LevelMenuItem::LevelMenuItem(int levelNum, const GameLevel* level, 
                                                   float width, const Point2D& topLeftCorner, 
                                                   const Texture* starTexture) : 
level(level), topLeftCorner(topLeftCorner), starTexture(starTexture), width(width) {
    assert(level != NULL);
    assert(levelNum > 0);

    std::stringstream levelNumStr;
    levelNumStr << levelNum;

    this->numLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), levelNumStr.str());
    this->numLabel->SetDropShadow(Colour(0.4f, 0.4f, 0.4f), 0.04f);
    this->numLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
    this->numLabel->SetTopLeftCorner(this->topLeftCorner);

    float nameLabelWidth = width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();
    this->nameLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), nameLabelWidth, level->GetName());
    this->nameLabel->SetColour(Colour(0,0,0));
    this->nameLabel->SetLineSpacing(4.0f);
    float nameXPos = this->topLeftCorner[0] + NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth();
    float nameYPos = this->topLeftCorner[1] - std::max<float>(0, ((this->numLabel->GetHeight() - this->nameLabel->GetHeight()) / 2.0f));
    this->nameLabel->SetTopLeftCorner(nameXPos, nameYPos);

    std::stringstream highScoreNumStr;
    highScoreNumStr << this->level->GetHighScore();
    
    std::stringstream highScoreStr;
    highScoreStr << "High Score: " << stringhelper::AddNumberCommas(highScoreNumStr.str()) ;

    this->highScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), highScoreStr.str());
    this->highScoreLabel->SetColour(Colour(0.25, 0.25, 0.25));
    //this->highScoreLabel->SetTopLeftCorner(this->nameLabel->GetTopLeftCorner() - Vector2D(0, NAME_TO_HIGH_SCORE_Y_GAP + this->nameLabel->GetHeight()));
    this->highScoreLabel->SetTopLeftCorner(this->nameLabel->GetTopLeftCorner()[0],
        this->numLabel->GetTopLeftCorner()[1] - (NUM_TO_HIGH_SCORE_Y_GAP + this->numLabel->GetHeight()));

    this->starDisplayList = glGenLists(1);
	glNewList(this->starDisplayList, GL_COMPILE);

    glPushAttrib(GL_CURRENT_BIT);
    glPushMatrix();

    static const float STAR_GAP = 5;
    
    this->starSize = (nameLabelWidth - STAR_GAP * (GameLevel::MAX_STARS_PER_LEVEL-1)) / static_cast<float>(GameLevel::MAX_STARS_PER_LEVEL);
    float halfStarSize = starSize / 2.0f;

    glTranslatef(this->highScoreLabel->GetTopLeftCorner()[0], 
        this->highScoreLabel->GetTopLeftCorner()[1] - this->highScoreLabel->GetHeight() - HIGH_SCORE_TO_STAR_Y_GAP, 0.0f);
    glTranslatef(-STAR_GAP - halfStarSize, -halfStarSize, 0);
    this->starTexture->BindTexture();
    glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        glTranslatef(STAR_GAP + starSize, 0, 0);

	    glBegin(GL_QUADS);
		    glNormal3i(0, 0, 1);
		    glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
		    glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
		    glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
		    glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
	    glEnd();
    }
    this->starTexture->UnbindTexture();

    glPopMatrix();
    glPopAttrib();

    glEndList();
}

SelectLevelMenuState::LevelMenuItem::~LevelMenuItem() {
    delete this->numLabel;
    this->numLabel = NULL;
    delete this->nameLabel;
    this->nameLabel = NULL;
    delete this->highScoreLabel;
    this->highScoreLabel = NULL;

    glDeleteLists(this->starDisplayList, 1);
    this->starDisplayList = 0;
}

float SelectLevelMenuState::LevelMenuItem::GetHeight() const {
    return this->numLabel->GetHeight() + NUM_TO_HIGH_SCORE_Y_GAP +
           this->highScoreLabel->GetHeight() + HIGH_SCORE_TO_STAR_Y_GAP + this->starSize;
}

void SelectLevelMenuState::LevelMenuItem::Draw(const Camera& camera, double dT) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    // Draw the level number and name
    this->numLabel->Draw();
    this->nameLabel->Draw();

    // Draw the high score for the level
    this->highScoreLabel->Draw();
    
    // Draw the number of stars earned for the level
    glCallList(this->starDisplayList);
}