/**
 * SelectLevelMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
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
#include "../GameModel/GameProgressIO.h"

static const float BORDER_GAP = 10;

SelectLevelMenuState::SelectLevelMenuState(GameDisplay* display, const GameWorld* world) : 
DisplayState(display), worldLabel(NULL), world(world), pressEscAlphaAnim(0.0f), 
goBackToWorldSelectMenu(false), goToStartLevel(false), goBackMenuMoveAnim(0.0f), goBackMenuAlphaAnim(1.0f), starTexture(NULL),
bossIconTexture(NULL), arrowTexture(NULL), nextPgArrowEmitter(NULL), prevPgArrowEmitter(NULL), starryBG(NULL), padlockTexture(NULL),
selectionAlphaOrangeAnim(0.0f), selectionAlphaYellowAnim(0.0f), selectionBorderAddAnim(0.0f), totalNumStarsLabel(NULL) {

    this->bgSoundLoopID = this->display->GetSound()->PlaySound(GameSound::LevelMenuBackgroundLoop, true);

    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);
    this->bossIconTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOSS_ICON, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->bossIconTexture != NULL);
    this->arrowTexture = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_UP_ARROW, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->arrowTexture != NULL);

    // Load background texture
    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear));
    assert(this->starryBG != NULL);

    this->padlockTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_PADLOCK, Texture::Trilinear));
    assert(this->padlockTexture != NULL);

    std::vector<ColourRGBA> arrowColours;
    arrowColours.reserve(3);
    arrowColours.push_back(ColourRGBA(0,1,0,0));
    arrowColours.push_back(ColourRGBA(0,1,0,1));
    arrowColours.push_back(ColourRGBA(0,1,0,0));
    this->nextArrowFader.SetColours(arrowColours);

    static const int NUM_ARROWS = 10;
    static const float ARROW_LIFE_TIME_MIN = 2.2f;
    static const float ARROW_LIFE_TIME_MAX = 3.0f;
    static const float ARROW_SPAWN_DELTA   = 0.45f;
    static const float ARROW_WIDTH  = 25;
    static const float ARROW_HEIGHT = 35;
    static const float ARROW_SPD    = 30.0f;

    this->nextPgArrowEmitter = new ESPVolumeEmitter();
	this->nextPgArrowEmitter->SetSpawnDelta(ESPInterval(ARROW_SPAWN_DELTA));
	this->nextPgArrowEmitter->SetInitialSpd(ESPInterval(ARROW_SPD));
	this->nextPgArrowEmitter->SetParticleLife(ESPInterval(ARROW_LIFE_TIME_MIN, ARROW_LIFE_TIME_MAX));
    this->nextPgArrowEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	this->nextPgArrowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->nextPgArrowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->nextPgArrowEmitter->SetParticleRotation(ESPInterval(90));
    this->nextPgArrowEmitter->SetEmitDirection(Vector3D(1,0,0));
	this->nextPgArrowEmitter->SetParticleSize(ESPInterval(ARROW_WIDTH), ESPInterval(ARROW_HEIGHT));
    this->nextPgArrowEmitter->SetParticleColour(ESPInterval(0), ESPInterval(1), ESPInterval(0), ESPInterval(1));
    this->nextPgArrowEmitter->SetParticles(NUM_ARROWS, static_cast<Texture2D*>(this->arrowTexture));
    this->nextPgArrowEmitter->AddEffector(&nextArrowFader);

    this->prevPgArrowEmitter = new ESPVolumeEmitter();
	this->prevPgArrowEmitter->SetSpawnDelta(ESPInterval(ARROW_SPAWN_DELTA));
	this->prevPgArrowEmitter->SetInitialSpd(ESPInterval(ARROW_SPD));
	this->prevPgArrowEmitter->SetParticleLife(ESPInterval(ARROW_LIFE_TIME_MIN, ARROW_LIFE_TIME_MAX));
    this->prevPgArrowEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	this->prevPgArrowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->prevPgArrowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->prevPgArrowEmitter->SetParticleRotation(ESPInterval(-90));
    this->prevPgArrowEmitter->SetEmitDirection(Vector3D(-1,0,0));
	this->prevPgArrowEmitter->SetParticleSize(ESPInterval(ARROW_WIDTH), ESPInterval(ARROW_HEIGHT));
    this->prevPgArrowEmitter->SetParticleColour(ESPInterval(0), ESPInterval(1), ESPInterval(0), ESPInterval(1));
    this->prevPgArrowEmitter->SetParticles(NUM_ARROWS, static_cast<Texture2D*>(this->arrowTexture));
    this->prevPgArrowEmitter->AddEffector(&nextArrowFader);

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
    worldLabelTxt << "Movement " << this->world->GetWorldNumber() << ": " << this->world->GetName();
    this->worldLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), worldLabelTxt.str());
    this->worldLabel->SetColour(titleColour);
    this->worldLabel->SetDropShadow(Colour(1,1,1), 0.05f);

    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));

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

    this->SetupLevelPages();
}

SelectLevelMenuState::~SelectLevelMenuState() {
    this->display->GetSound()->StopSound(this->bgSoundLoopID, 0.5);

    delete this->menuFBO;
    this->menuFBO = NULL;
    delete this->bloomEffect;
    this->bloomEffect = NULL;

    delete this->worldLabel;
    this->worldLabel = NULL;

    delete this->totalNumStarsLabel;
    this->totalNumStarsLabel = NULL;

    delete this->keyEscLabel;
    this->keyEscLabel = NULL;

    this->ClearLevelPages();

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bossIconTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->arrowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockTexture);
    assert(success);
    UNUSED_VARIABLE(success);

    delete this->nextPgArrowEmitter;
    this->nextPgArrowEmitter = NULL;
    delete this->prevPgArrowEmitter;
    this->prevPgArrowEmitter = NULL;
}


void SelectLevelMenuState::RenderFrame(double dT) {
    this->pressEscAlphaAnim.Tick(dT);

    const Camera& camera = this->display->GetCamera();

    // Bind the menu's FrameBufferObject - we use this to do Bloom on the menu
	this->menuFBO->BindFBObj();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the starry background...
    if (!this->goBackToWorldSelectMenu) {
        this->starryBG->BindTexture();
        GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 
            GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
            GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()));
        this->starryBG->UnbindTexture();
    }

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the title...
    this->worldLabel->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, camera.GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldLabel->Draw();

    // Draw the title strip
    //this->DrawTitleStrip(camera);

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

    // Draw the label showing how many stars have been acquired for the world...
    this->DrawStarTotalLabel(camera);

    // Draw the page selection squares
    this->DrawPageSelection(camera);

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
        glDisable(GL_DEPTH_TEST);

        float screenWidth = camera.GetWindowWidth();
        float screenHeight = camera.GetWindowHeight();
        float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(this->starryBG->GetWidth()));
        float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(this->starryBG->GetHeight()));

        this->starryBG->BindTexture();
        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(-camera.GetWindowWidth() + moveAmt, 0);
            glTexCoord2f(2*totalTexU, 0);
	        glVertex2f(camera.GetWindowWidth() + moveAmt, 0);
            glTexCoord2f(2*totalTexU, totalTexV);
            glVertex2f(camera.GetWindowWidth() + moveAmt, camera.GetWindowHeight());
	        glTexCoord2f(0, totalTexV);
            glVertex2f(-camera.GetWindowWidth() + moveAmt, camera.GetWindowHeight());
        glEnd();

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

    // Handle the case where the player selected a level
    if (this->goToStartLevel && fadeDone) {

        // Start the currently selected level
        AbstractLevelMenuItem* selectedLevelItem = this->pages[this->selectedPage]->GetSelectedItem();
        const GameLevel* selectedLevel = selectedLevelItem->GetLevel();

		// Turn off all the sounds first (waiting for any unfinished sounds), then switch states
		//GameSound* sound = this->display->GetSound();
		//sound->StopAllSounds();

		// Load all the initial stuffs for the game - this will queue up the next states that we need to go to
        this->display->GetModel()->StartGameAtWorldAndLevel(this->world->GetWorldIndex(), selectedLevel->GetLevelIndex());
		// Place the view into the proper state to play the game	
		this->display->SetCurrentStateAsNextQueuedState();
    }

    debug_opengl_state();
}

void SelectLevelMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

    if (goBackToWorldSelectMenu || goToStartLevel) {
        return;
    }

    switch (pressedButton) {
        case GameControl::EscapeButtonAction:
            this->GoBackToWorldSelectMenu();
            break;

        case GameControl::EnterButtonAction:
            this->GoToStartLevel();
            break;

        case GameControl::LeftButtonAction:
            this->MoveSelectionX(false);
            break;

        case GameControl::RightButtonAction:
            this->MoveSelectionX(true);
            break;

        case GameControl::UpButtonAction:
            this->MoveSelectionY(true);
            break;

        case GameControl::DownButtonAction:
            this->MoveSelectionY(false);
            break;

#ifdef _DEBUG
        case GameControl::SpecialCheatButtonAction:
            // Unlock all levels in the current movement...
            // (You need to restart the game for this to take effect)
            GameProgressIO::SaveFullProgressOfWorld(this->display->GetModel(), this->world->GetStyle());
            break;
#endif

        default:
            break;
    }

}

void SelectLevelMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

// Event called whenever the selection of a level in the menu changes
void SelectLevelMenuState::LevelSelectionChanged() {
    this->display->GetSound()->PlaySound(GameSound::LevelMenuItemChangedSelectionEvent, false);
}
void SelectLevelMenuState::PageSelectionChanged() {
    this->display->GetSound()->PlaySound(GameSound::LevelMenuPageChangedSelectionEvent, false);

    this->nextPgArrowEmitter->Reset();
    this->prevPgArrowEmitter->Reset();
}

void SelectLevelMenuState::DrawStarTotalLabel(const Camera& camera) {
    float labelTopCornerX = camera.GetWindowWidth() - this->totalNumStarsLabel->GetLastRasterWidth() - HORIZONTAL_TITLE_GAP;
    float labelTopCornerY = this->totalNumStarsLabel->GetHeight() + VERTICAL_TITLE_GAP/2;

    this->totalNumStarsLabel->SetTopLeftCorner(labelTopCornerX, labelTopCornerY);
    this->totalNumStarsLabel->Draw();

    float diff      = 0.25f * this->totalNumStarsLabel->GetHeight();
    float starSize  = this->totalNumStarsLabel->GetHeight() + 2*diff;
    float starRightX  = labelTopCornerX - diff;
    float starLeftX   = starRightX - starSize;
    float starTopY    = labelTopCornerY + 1.5f * diff;
    float starBottomY = starTopY - starSize;

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Colour& starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
    this->starTexture->BindTexture();
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 1);
    glVertex2f(starLeftX,  starTopY);
    glTexCoord2i(0, 0);
    glVertex2f(starLeftX,  starBottomY);
    glTexCoord2i(1, 0);
    glVertex2f(starRightX, starBottomY);
    glTexCoord2i(1, 1);
    glVertex2f(starRightX, starTopY);
    glEnd();

    glPopAttrib();
}

void SelectLevelMenuState::DrawTitleStrip(const Camera& camera) const {

    static const float STRIP_HEIGHT      = 10;
    static const float DROP_AMT          = STRIP_HEIGHT / 5.0f;
    static const float TEXT_TO_STRIP_GAP = 20;
    static const float SIDE_TO_STRIP_GAP = 20;

    Point2D stripStartPt = this->worldLabel->GetTopLeftCorner() + 
        Vector2D(this->worldLabel->GetLastRasterWidth() + TEXT_TO_STRIP_GAP, -static_cast<int>(this->worldLabel->GetHeight()/2));


    // Drop shadow of the title strip
    glPushMatrix();
    glTranslatef(DROP_AMT, -DROP_AMT, 0);
    glBegin(GL_QUADS);
    glColor4f(1, 1, 1, 1);
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

void SelectLevelMenuState::DrawPageSelection(const Camera& camera) const {
    // Don't bother drawing anything if there's only one page
    if (this->pages.size() <= 1) {
        return;
    }

    static const int SQUARE_SIZE         = 16;
    static const int SQUARE_GAP_SIZE     = 5;
    static const int SQUARE_BOTTOM_Y_POS = VERTICAL_TITLE_GAP;
    static const int SQUARE_TOP_Y_POS    = SQUARE_BOTTOM_Y_POS + SQUARE_SIZE;
    
    int totalWidth = SQUARE_SIZE * this->pages.size() + SQUARE_GAP_SIZE * (this->pages.size()-1);
    int currX = (camera.GetWindowWidth() - totalWidth) / 2;

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    // Draw a square for each page, the selected page is coloured in, the rest are grey
    glBegin(GL_QUADS);
    for (size_t i = 0; i < this->pages.size(); i++) {
        if (i == static_cast<size_t>(this->selectedPage)) {
            glColor4f(1, 0.65f, 0, 1);
        }
        else {
            glColor4f(0.66f, 0.66f, 0.66f, 1);
        }

        glVertex2f(currX, SQUARE_TOP_Y_POS);
        glVertex2f(currX, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_TOP_Y_POS);

        currX += SQUARE_SIZE + SQUARE_GAP_SIZE;
    }
    glEnd();
    
    // Draw outlines...
    glLineWidth(1.0f);
    glColor4f(0, 0, 0, 1);
    currX = (camera.GetWindowWidth() - totalWidth) / 2;
    for (size_t i = 0; i < this->pages.size(); i++) {
        
        glBegin(GL_LINE_LOOP);
        glVertex2f(currX, SQUARE_TOP_Y_POS);
        glVertex2f(currX, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_BOTTOM_Y_POS);
        glVertex2f(currX+SQUARE_SIZE, SQUARE_TOP_Y_POS);
        glEnd();

        currX += SQUARE_SIZE + SQUARE_GAP_SIZE;
    }
    
    glPopMatrix();
    glPopAttrib();
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

    AbstractLevelMenuItem* selectedItem = this->pages[this->selectedPage]->GetSelectedItem();
    assert(selectedItem != NULL);
    
    float leftX  = selectedItem->GetTopLeftCorner()[0] - BORDER_GAP;
    float rightX = selectedItem->GetTopLeftCorner()[0] + selectedItem->GetWidth() + BORDER_GAP;
    float topY = selectedItem->GetTopLeftCorner()[1] + BORDER_GAP/2;
    float bottomY = selectedItem->GetTopLeftCorner()[1] - selectedItem->GetHeight() - BORDER_GAP;

    float centerX = (leftX + rightX) / 2.0f;
    float centerY = (topY + bottomY) / 2.0f;
    float selectionWidth  = selectedItem->GetWidth() + 2*BORDER_GAP;
    float selectionHeight = selectedItem->GetHeight() + 1.5*BORDER_GAP;

    this->selectionAlphaOrangeAnim.Tick(dT);
    this->selectionAlphaYellowAnim.Tick(dT);
    this->selectionBorderAddAnim.Tick(dT);

    float yellowAlpha  = this->selectionAlphaYellowAnim.GetInterpolantValue();
    float orangeAlpha  = this->selectionAlphaOrangeAnim.GetInterpolantValue();
    float selectionAdd = this->selectionBorderAddAnim.GetInterpolantValue();


    // Draw any indicator arrows to show that the selection can be moved to another page
    if (this->selectedPage != static_cast<int>(this->pages.size())-1) {
        const AbstractLevelMenuItem* lastItem = this->pages[this->selectedPage]->GetLastItem();
        this->nextPgArrowEmitter->SetEmitVolume(Point3D(lastItem->GetTopLeftCorner()[0] + lastItem->GetWidth(), 
                                                        lastItem->GetTopLeftCorner()[1] - lastItem->GetHeight()+5, 0),
                                                Point3D(lastItem->GetTopLeftCorner()[0] + lastItem->GetWidth(),
                                                        lastItem->GetTopLeftCorner()[1]-5, 0));

        this->nextPgArrowEmitter->Tick(dT);
        this->nextPgArrowEmitter->Draw(camera);
    }

    if (this->selectedPage != 0) {
        const AbstractLevelMenuItem* firstItem = this->pages[this->selectedPage]->GetFirstItem();
        this->prevPgArrowEmitter->SetEmitVolume(Point3D(firstItem->GetTopLeftCorner()[0], 
                                                        firstItem->GetTopLeftCorner()[1] - firstItem->GetHeight()+5, 0),
                                                Point3D(firstItem->GetTopLeftCorner()[0],
                                                        firstItem->GetTopLeftCorner()[1]-5, 0));
        this->prevPgArrowEmitter->Tick(dT);
        this->prevPgArrowEmitter->Draw(camera);
    }

     // Draw the currently selected level item's selection border
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    
    glPushMatrix();
    // Draw the yellow border
    glColor4f(1, 1, 0, yellowAlpha);
    glScalef(selectionWidth + selectionAdd, selectionHeight + selectionAdd, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    // Draw the orange border
    glColor4f(1, 0.65f, 0, orangeAlpha);
    glScalef(selectionWidth + selectionAdd/2, selectionHeight + selectionAdd/2, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPopMatrix();

    // Draw the yellowish highlight background
    glColor4f(0.98f, 1.0f, 0.66f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(leftX, topY);
    glVertex2f(leftX, bottomY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX, topY);
    glEnd();

    // Draw the black outline around the selection
    glColor4f(0, 0, 0, 1);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(leftX, topY);
    glVertex2f(leftX, bottomY);
    glVertex2f(rightX, bottomY);
    glVertex2f(rightX, topY);
    glEnd();

    // Draw the current page of level items
    this->pages[this->selectedPage]->Draw(camera, dT);

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

void SelectLevelMenuState::GoToStartLevel() {
    GameSound* sound = this->display->GetSound();
    
    AbstractLevelMenuItem* selectedItem = this->pages[this->selectedPage]->GetSelectedItem();
    assert(selectedItem != NULL);
    if (selectedItem->GetIsEnabled()) {
        sound->PlaySound(GameSound::LevelMenuItemSelectEvent, false);

        // Finishing animation for starting the level
        this->fadeAnimation.SetLerp(0.5f, 1.0f);
	    this->fadeAnimation.SetRepeat(false);

        this->goToStartLevel = true;
    }
    else {
        sound->PlaySound(GameSound::LevelMenuItemLockedEvent, false);

        // Play animation to indicate/show that the level is disabled
        selectedItem->ExecuteLockedAnimation();
    }
}

void SelectLevelMenuState::SetupLevelPages() {
    this->ClearLevelPages();

    const Camera& camera = this->display->GetCamera();

    static const int TITLE_TO_ITEM_Y_GAP_SIZE = 40;
    static const int SIDE_TO_ITEM_GAP_SIZE    = HORIZONTAL_TITLE_GAP;
    static const int ITEM_X_GAP_SIZE          = 40;
    static const int ITEM_Y_GAP_SIZE          = 40;
    static const int MIN_ITEM_SIZE            = 180;

    float amtForItems = camera.GetWindowWidth() - 2 * SIDE_TO_ITEM_GAP_SIZE;
    
    float itemWidth = 0;
    this->numItemsPerRow = 5; 
    while (itemWidth < MIN_ITEM_SIZE && numItemsPerRow >= 2) {
        this->numItemsPerRow--;
        itemWidth = static_cast<float>(amtForItems - (ITEM_X_GAP_SIZE * (numItemsPerRow-1))) / 
                    static_cast<float>(this->numItemsPerRow);
    }
    assert(this->numItemsPerRow >= 2);
    
    float itemX = SIDE_TO_ITEM_GAP_SIZE;
    float itemY = camera.GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - TITLE_TO_ITEM_Y_GAP_SIZE;

    // Build the menu items for level selection
    int totalNumStarsCollected = this->world->GetNumStarsCollectedInWorld();
    int totalNumStars = this->world->GetTotalAchievableStarsInWorld();

    const std::vector<GameLevel*>& levels = this->world->GetAllLevelsInWorld();
    this->pages.push_back(new LevelMenuPage());
    AbstractLevelMenuItem* levelItem   = NULL;
    int numRows = static_cast<int>(ceil(static_cast<float>(levels.size()) / static_cast<float>(numItemsPerRow)));
    bool noScoreEncountered = false;

    float standardHeight = 0.0f;

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numItemsPerRow; col++) {
            int currLevelIdx = row * numItemsPerRow + col;
            if (currLevelIdx >= static_cast<int>(levels.size())) {
                break;
            }

            const GameLevel* currLevel = levels.at(currLevelIdx);
            assert(currLevel != NULL);

            if (currLevel->GetHasBoss()) {
                levelItem = new BossLevelMenuItem(this, currLevelIdx+1, currLevel, itemWidth, standardHeight,
                    Point2D(itemX, itemY), !noScoreEncountered, this->bossIconTexture);
            }
            else {
                levelItem = new LevelMenuItem(this, currLevelIdx+1, currLevel, itemWidth, 
                    Point2D(itemX, itemY), !noScoreEncountered, this->starTexture);
                standardHeight = levelItem->GetHeight();
            }

            this->pages[this->pages.size()-1]->AddLevelItem(levelItem);

            if (!currLevel->GetIsLevelPassedWithScore()) {
                noScoreEncountered = true;
            }

            itemX += itemWidth + ITEM_X_GAP_SIZE;
        }
 
        assert(levelItem != NULL);
        itemX  = SIDE_TO_ITEM_GAP_SIZE;
        itemY -= ITEM_Y_GAP_SIZE + levelItem->GetHeight();

        // Check to see if we've exceeded the size of the page so that we make the next page...
        if (row != numRows-1 && (itemY - levelItem->GetHeight()) <= (this->keyEscLabel->GetHeight() + VERTICAL_TITLE_GAP)) {
            itemY = camera.GetWindowHeight() - this->worldLabel->GetHeight() - VERTICAL_TITLE_GAP - TITLE_TO_ITEM_Y_GAP_SIZE;
            this->pages.push_back(new LevelMenuPage());
        }
    }

    assert(!this->pages.empty());
    this->selectedPage = 0;

    // Setup the selection animation
    std::vector<double> timeVals;
    timeVals.push_back(0.0);
    timeVals.push_back(0.5);
    timeVals.push_back(1.0);
    std::vector<float> alphaVals;
    alphaVals.push_back(0.0f);
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaOrangeAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaOrangeAnim.SetRepeat(true);

    timeVals.clear();
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    alphaVals.clear();
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaYellowAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaYellowAnim.SetRepeat(true);

    std::vector<float> scaleVals;
    scaleVals.push_back(0.0f);
    scaleVals.push_back(2*ITEM_X_GAP_SIZE);
    this->selectionBorderAddAnim.SetLerp(timeVals, scaleVals);
    this->selectionBorderAddAnim.SetRepeat(true);

    std::stringstream totalNumStarsTxt;
    totalNumStarsTxt << totalNumStarsCollected << "/" << totalNumStars;
    this->totalNumStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium), totalNumStarsTxt.str());
    this->totalNumStarsLabel->SetColour(Colour(1,1,1));
}

void SelectLevelMenuState::ClearLevelPages() {
    for (size_t i = 0; i < this->pages.size(); i++) {
        LevelMenuPage* page = this->pages[i];
        delete page;
        page = NULL;
    }
    this->pages.clear();
}

void SelectLevelMenuState::MoveSelectionX(bool right) {
    int x = right ? 1 : -1;

    // Check to see if we're spilling over the edge of the current page - if there's a page to either
    // side then the selection will move to that new page...
    size_t selectedItem = this->pages[this->selectedPage]->GetSelectedItemIndex();
    
    size_t prevSelectedItem = selectedItem;
    int prevSelectedPg   = this->selectedPage;

    assert(this->numItemsPerRow-1 > 0);

    bool isFirstItem      = selectedItem == 0;
    bool isLastItem       = selectedItem == (this->pages[this->selectedPage]->GetNumLevelItems()-1);

    bool fallThrough = false;
    if (this->pages.size() > 1 && (isFirstItem || isLastItem)) {
        if (isFirstItem && !right) {
            // The user is moving the selection off the page to the left
            size_t newPageNum = (this->pages.size() + (static_cast<int>(this->selectedPage) - 1)) % this->pages.size();
            this->selectedPage = newPageNum;
            LevelMenuPage* currPage = this->pages[this->selectedPage];
            currPage->SetSelectedItemIndex(currPage->GetNumLevelItems()-1);
        }
        else if (isLastItem && right) {
            // The user is moving the selection off the page to the right
            size_t newPageNum = (this->selectedPage + 1) % this->pages.size();
            this->selectedPage = newPageNum;
            LevelMenuPage* currPage = this->pages[this->selectedPage];
            currPage->SetSelectedItemIndex(0);
        }
        else {
            fallThrough = true;
        }
    }
    else {
        fallThrough = true;
    }

    if (fallThrough) {
        int currRowIndex  = selectedItem / this->numItemsPerRow;
        int wrapAroundX   = (this->numItemsPerRow + (selectedItem - currRowIndex * this->numItemsPerRow) + x) % this->numItemsPerRow;

        bool isFirstItemInRow = (selectedItem % this->numItemsPerRow) == 0;
        bool isLastItemInRow  = (selectedItem % this->numItemsPerRow) == static_cast<size_t>(this->numItemsPerRow-1);

        if (right && !isLastItem && isLastItemInRow) {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * (currRowIndex+1) + wrapAroundX);
        }
        else if(!right && currRowIndex != 0 && isFirstItemInRow) {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * (currRowIndex-1) + wrapAroundX);
        }
        else {
            this->pages[this->selectedPage]->SetSelectedItemIndex(this->numItemsPerRow * currRowIndex + wrapAroundX);
        }
    }

    // Check to see if we should raise a level selection changed event
    if (this->selectedPage != prevSelectedPg) {
        this->PageSelectionChanged();
        this->LevelSelectionChanged();
    }
    else if (this->pages[this->selectedPage]->GetSelectedItemIndex() != prevSelectedItem) {
        this->LevelSelectionChanged();
    }
}

void SelectLevelMenuState::MoveSelectionY(bool up) {
    int y = up ? 1 : -1;

    int selectedItem  = this->pages[this->selectedPage]->GetSelectedItemIndex();
    size_t numLevelItems = this->pages[this->selectedPage]->GetNumLevelItems();

    int numRows = (numLevelItems / this->numItemsPerRow) + 1;
    int rowsAllFilledNumItems = (numRows * this->numItemsPerRow);
    int newSelectedIndex = (rowsAllFilledNumItems + selectedItem - y * this->numItemsPerRow) % rowsAllFilledNumItems;
    if (newSelectedIndex >= static_cast<int>(numLevelItems)) {
        if (up) {
            newSelectedIndex -= this->numItemsPerRow;
        }
        else {
            newSelectedIndex %= this->numItemsPerRow;
        }
    }
    this->pages[this->selectedPage]->SetSelectedItemIndex(newSelectedIndex);

    // Check to see if we should raise a level selection changed event
    if (selectedItem != newSelectedIndex) {
        this->LevelSelectionChanged();
    }
}

int SelectLevelMenuState::GetNumItemsOnRow(int rowIdx) {
    size_t numLevelItems = this->pages[this->selectedPage]->GetNumLevelItems();
    int temp = static_cast<int>(numLevelItems) - (static_cast<int>(this->numItemsPerRow) * rowIdx);
    return std::min<int>(temp, this->numItemsPerRow);
}

const float SelectLevelMenuState::AbstractLevelMenuItem::NUM_TO_NAME_GAP = 8;
const float SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT = 0.78f;
const float SelectLevelMenuState::AbstractLevelMenuItem::PADLOCK_SCALE = 0.85f;

SelectLevelMenuState::AbstractLevelMenuItem::AbstractLevelMenuItem(SelectLevelMenuState* state,
                                                                   int levelNum, const GameLevel* level, float width,
                                                                   const Point2D& topLeftCorner, bool isEnabled) :
state(state), level(level), topLeftCorner(topLeftCorner), width(width), isEnabled(isEnabled) {

    assert(state != NULL);
    assert(level != NULL);
    assert(levelNum > 0);

    std::stringstream levelNumStr;
    levelNumStr << levelNum;

    const Colour DISABLED_COLOUR(DISABLED_GREY_AMT, DISABLED_GREY_AMT, DISABLED_GREY_AMT);

    this->numLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), levelNumStr.str());
    this->numLabel->SetTopLeftCorner(this->topLeftCorner);

    float nameLabelWidth = width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();
    
    std::string levelName;
    if (isEnabled) {
        levelName = level->GetName();
    }
    else {
        levelName = std::string("???");
    }
    
    this->nameLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), nameLabelWidth, levelName);
    this->nameLabel->SetLineSpacing(4.0f);
    float nameXPos = this->topLeftCorner[0] + NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth();
    float nameYPos = this->topLeftCorner[1] - std::max<float>(0, ((this->numLabel->GetHeight() - this->nameLabel->GetHeight()) / 2.0f));
    this->nameLabel->SetTopLeftCorner(nameXPos, nameYPos);

    if (isEnabled) {
        this->nameLabel->SetColour(Colour(0,0,0));
        this->numLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
        this->numLabel->SetDropShadow(Colour(0.0f, 0.0f, 0.0f), 0.04f);
    }
    else {
        this->nameLabel->SetColour(DISABLED_COLOUR);
        this->numLabel->SetColour(DISABLED_COLOUR);
    }

    this->lockedAnim.ClearLerp();
    this->lockedAnim.SetInterpolantValue(0);
}

SelectLevelMenuState::AbstractLevelMenuItem::~AbstractLevelMenuItem() {
    delete this->numLabel;
    this->numLabel = NULL;
    delete this->nameLabel;
    this->nameLabel = NULL;
}

void SelectLevelMenuState::AbstractLevelMenuItem::DrawBG(bool isSelected) {
    if (!isSelected) {
        float leftX   = this->GetTopLeftCorner()[0] - BORDER_GAP;
        float rightX  = this->GetTopLeftCorner()[0] + this->GetWidth() + BORDER_GAP;
        float topY    = this->GetTopLeftCorner()[1] + BORDER_GAP/2;
        float bottomY = this->GetTopLeftCorner()[1] - this->GetHeight() - BORDER_GAP;

        glColor4f(1,1,1,1);
        glBegin(GL_QUADS);
        glVertex2f(leftX,  bottomY);
        glVertex2f(rightX, bottomY);
        glVertex2f(rightX, topY);
        glVertex2f(leftX,  topY);
        glEnd();

        glLineWidth(2.0f);
        glColor4f(0.4f, 0.6f, 0.8f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(leftX,  bottomY);
        glVertex2f(rightX, bottomY);
        glVertex2f(rightX, topY);
        glVertex2f(leftX,  topY);
        glEnd();
    }
}

void SelectLevelMenuState::AbstractLevelMenuItem::DrawPadlock(double dT) {
    this->lockedAnim.Tick(dT);
    float lockMoveX = this->lockedAnim.GetInterpolantValue();
    
    glPushAttrib(GL_CURRENT_BIT);
        
    glColor4f(1,1,1,1);
    glPushMatrix();
    glTranslatef(static_cast<int>(lockMoveX + this->topLeftCorner[0] + this->GetWidth()/2), 
                 static_cast<int>(this->topLeftCorner[1] - this->GetHeight()/2), 0.0f);
    
    float padlockScale = static_cast<int>(PADLOCK_SCALE * this->GetHeight());
    glScalef(padlockScale, padlockScale, 1.0f);
    
    this->state->padlockTexture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->state->padlockTexture->UnbindTexture();

    glPopMatrix();

    glPopAttrib();
}

void SelectLevelMenuState::AbstractLevelMenuItem::ExecuteLockedAnimation() {
    
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
    float maxMove = 0.25f * PADLOCK_SCALE * this->GetHeight();
    movementValues.push_back(0.0f);
    movementValues.push_back(maxMove * 0.9f);
    movementValues.push_back(-maxMove * 0.1f);
    movementValues.push_back(-maxMove);
    movementValues.push_back(maxMove * 0.5f);
    movementValues.push_back(maxMove);
    movementValues.push_back(-0.2f * maxMove);
    movementValues.push_back(0.0f);
    this->lockedAnim.SetLerp(timeValues, movementValues);
    this->lockedAnim.SetRepeat(false);
}

const float SelectLevelMenuState::LevelMenuItem::NUM_TO_HIGH_SCORE_Y_GAP = 5;
const float SelectLevelMenuState::LevelMenuItem::HIGH_SCORE_TO_STAR_Y_GAP = 4;

SelectLevelMenuState::LevelMenuItem::LevelMenuItem(SelectLevelMenuState* state,
                                                   int levelNum, const GameLevel* level, 
                                                   float width, const Point2D& topLeftCorner, 
                                                   bool isEnabled, const Texture* starTexture) : 
AbstractLevelMenuItem(state, levelNum, level, width, topLeftCorner, isEnabled), starTexture(starTexture) {

    const Colour DISABLED_COLOUR(
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT,
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT,
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT);

    float nameLabelWidth = width - NUM_TO_NAME_GAP - this->numLabel->GetLastRasterWidth();

    std::string highScoreStr = "High Score: " + stringhelper::AddNumberCommas(this->level->GetHighScore()) ;
    this->highScoreLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Small), highScoreStr);
    
    //this->highScoreLabel->SetTopLeftCorner(this->nameLabel->GetTopLeftCorner() - Vector2D(0, NAME_TO_HIGH_SCORE_Y_GAP + this->nameLabel->GetHeight()));
    this->highScoreLabel->SetTopLeftCorner(this->nameLabel->GetTopLeftCorner()[0],
        this->numLabel->GetTopLeftCorner()[1] - (NUM_TO_HIGH_SCORE_Y_GAP + this->numLabel->GetHeight()));

    Colour starMultiplyColour(1,1,1);
    if (!isEnabled) {
        this->highScoreLabel->SetColour(DISABLED_COLOUR);
        starMultiplyColour = DISABLED_COLOUR;
    }

    this->starDisplayList = glGenLists(1);
    glNewList(this->starDisplayList, GL_COMPILE);

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glPushMatrix();

    static const float STAR_GAP = 5;
    
    this->starSize = (nameLabelWidth - STAR_GAP * (GameLevel::MAX_STARS_PER_LEVEL-1)) / static_cast<float>(GameLevel::MAX_STARS_PER_LEVEL);
    float halfStarSize = starSize / 2.0f;

    glTranslatef(this->highScoreLabel->GetTopLeftCorner()[0], 
        this->highScoreLabel->GetTopLeftCorner()[1] - this->highScoreLabel->GetHeight() - HIGH_SCORE_TO_STAR_Y_GAP, 0.0f);
    glTranslatef(-STAR_GAP - halfStarSize, -halfStarSize, 0);
    this->starTexture->BindTexture();
    
    if (isEnabled) {
        int numStars = level->GetHighScoreNumStars();
        const Colour& ACTIVE_STAR_COLOUR   = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
        const Colour& INACTIVE_STAR_COLOUR = GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR;
        for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
            if (i < numStars) {
                glColor4f(ACTIVE_STAR_COLOUR.R(), ACTIVE_STAR_COLOUR.G(), ACTIVE_STAR_COLOUR.B(), 1.0f);
            }
            else {
                glColor4f(INACTIVE_STAR_COLOUR.R(), INACTIVE_STAR_COLOUR.G(), INACTIVE_STAR_COLOUR.B(), 1.0f);
            }

            glTranslatef(STAR_GAP + starSize, 0, 0);
            glBegin(GL_QUADS);
	            glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
	            glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
	            glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
	            glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
            glEnd();
        }
    }
    else {
        GLfloat envColour[4] = { 1.0, 1.0, 1.0, 1.0 };
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColour);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glColor4f(DISABLED_COLOUR.R(), DISABLED_COLOUR.G(), DISABLED_COLOUR.B(), 1.0f);
        for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
            glTranslatef(STAR_GAP + starSize, 0, 0);
            glBegin(GL_QUADS);
	            glTexCoord2i(0, 0); glVertex2f(-halfStarSize, -halfStarSize);
	            glTexCoord2i(1, 0); glVertex2f( halfStarSize, -halfStarSize);
	            glTexCoord2i(1, 1); glVertex2f( halfStarSize,  halfStarSize);
	            glTexCoord2i(0, 1); glVertex2f(-halfStarSize,  halfStarSize);
            glEnd();
        }
    }
    this->starTexture->UnbindTexture();

    glPopMatrix();
    glPopAttrib();

    glEndList();
}

SelectLevelMenuState::LevelMenuItem::~LevelMenuItem() {

    delete this->highScoreLabel;
    this->highScoreLabel = NULL;

    glDeleteLists(this->starDisplayList, 1);
    this->starDisplayList = 0;
}

float SelectLevelMenuState::LevelMenuItem::GetHeight() const {
    return this->numLabel->GetHeight() + NUM_TO_HIGH_SCORE_Y_GAP +
           this->highScoreLabel->GetHeight() + HIGH_SCORE_TO_STAR_Y_GAP + this->starSize;
}

void SelectLevelMenuState::LevelMenuItem::Draw(const Camera& camera, double dT, bool isSelected) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    this->DrawBG(isSelected);

    // Draw the level number and name
    this->numLabel->Draw();
    this->nameLabel->Draw();

    // Draw the high score for the level
    this->highScoreLabel->Draw();
    
    // Draw the number of stars earned for the level
    glCallList(this->starDisplayList);

    // When locked, draw the padlock
    if (!this->isEnabled) {
        this->DrawPadlock(dT);
    }
}

SelectLevelMenuState::LevelMenuPage::~LevelMenuPage() {
    for (size_t i = 0; i < this->levelItems.size(); i++) {
        AbstractLevelMenuItem* levelItem = this->levelItems.at(i);
        delete levelItem;
        levelItem = NULL;
    }
    this->levelItems.clear();
}

const float SelectLevelMenuState::BossLevelMenuItem::BOSS_ICON_GAP        = 5.0f;
const float SelectLevelMenuState::BossLevelMenuItem::NUM_TO_BOSS_NAME_GAP = 10.0f;
const float SelectLevelMenuState::BossLevelMenuItem::BOSS_NAME_ICON_GAP   = 4.0f;

SelectLevelMenuState::BossLevelMenuItem::BossLevelMenuItem(SelectLevelMenuState* state,
                                                           int levelNum, const GameLevel* level,
                                                           float width, float height, const Point2D& topLeftCorner,
                                                           bool isEnabled, const Texture* bossTexture) :
AbstractLevelMenuItem(state, levelNum, level, width, topLeftCorner, isEnabled), height(height), bossTexture(bossTexture),
bossDeadLabel(NULL) {

    assert(bossTexture != NULL);
    
    const Colour DISABLED_COLOUR(
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT,
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT,
        SelectLevelMenuState::AbstractLevelMenuItem::DISABLED_GREY_AMT);

    float bossXPos = this->topLeftCorner[0] + NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth();
    float bossYPos = this->topLeftCorner[1];

    this->bossLabel = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "BOSS");
    this->bossLabel->SetTopLeftCorner(bossXPos, bossYPos);
    this->bossLabel->SetScale(std::min<float>(1.0f, (width - (2*BORDER_GAP + 2*NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth() + this->bossLabel->GetHeight())) / 
        this->bossLabel->GetLastRasterWidth()));

    this->bossIconSize = this->bossLabel->GetHeight();
    float halfBossIconSize = bossIconSize / 2.0f;

    // Boss Icon Stuff
    this->bossIconDisplayList = glGenLists(1);
    glNewList(this->bossIconDisplayList, GL_COMPILE);

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glPushMatrix();

    float iconXPos = this->topLeftCorner[0] + this->width - (this->width - 
        (NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth() + this->bossLabel->GetLastRasterWidth())) / 2.0f;
    float iconYPos = this->topLeftCorner[1] - halfBossIconSize;

    glTranslatef(iconXPos, iconYPos, 0.0f);

    this->bossTexture->BindTexture();

    if (isEnabled) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        GLfloat envColour[4] = { 1.0, 1.0, 1.0, 1.0 };
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColour);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glColor4f(DISABLED_COLOUR.R(), DISABLED_COLOUR.G(), DISABLED_COLOUR.B(), 1.0f);
    }
    glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2f(-halfBossIconSize, -halfBossIconSize);
        glTexCoord2i(1, 0); glVertex2f( halfBossIconSize, -halfBossIconSize);
        glTexCoord2i(1, 1); glVertex2f( halfBossIconSize,  halfBossIconSize);
        glTexCoord2i(0, 1); glVertex2f(-halfBossIconSize,  halfBossIconSize);
    glEnd();

    this->bossTexture->UnbindTexture();

    glPopMatrix();
    glPopAttrib();

    glEndList();
    
    this->nameLabel->SetFont(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big));
    this->nameLabel->SetTopLeftCorner(bossXPos,
        this->topLeftCorner[1] - (NUM_TO_BOSS_NAME_GAP + std::max<float>(this->bossLabel->GetHeight(), this->bossIconSize)));
    this->nameLabel->SetScale(std::min<float>(1.0f, (this->width - (NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth())) / this->nameLabel->GetWidth()));
    this->nameLabel->SetFixedWidth((this->width - (NUM_TO_NAME_GAP + this->numLabel->GetLastRasterWidth())));

    if (isEnabled) {
        this->bossLabel->SetColour(Colour(1, 0, 0));
        this->bossLabel->SetDropShadow(Colour(0.0f, 0.0f, 0.0f), 0.04f);

        // If the boss has already been defeated then we draw a big 'X' over its icon
        if (level->GetIsLevelPassedWithScore()) {
            this->bossDeadLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "X");
            this->bossDeadLabel->SetScale(1.5f);
            this->bossDeadLabel->SetTopLeftCorner(
                (iconXPos - halfBossIconSize) + (this->bossIconSize - this->bossDeadLabel->GetLastRasterWidth()) / 2.0f,
                (iconYPos + halfBossIconSize) - (this->bossIconSize - this->bossDeadLabel->GetHeight()) / 2.0f);
            this->bossDeadLabel->SetColour(Colour(1, 0, 0));
            
        }
    }
    else {
        this->bossLabel->SetColour(DISABLED_COLOUR);
    }

}

SelectLevelMenuState::BossLevelMenuItem::~BossLevelMenuItem() {
    delete this->bossLabel;
    this->bossLabel = NULL;

    if (this->bossDeadLabel != NULL) {
        delete this->bossDeadLabel;
        this->bossDeadLabel = NULL;
    }
}

void SelectLevelMenuState::BossLevelMenuItem::Draw(const Camera& camera, double dT, bool isSelected) {
    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    this->DrawBG(isSelected);

    // Draw the level number and name
    this->numLabel->Draw();
    this->nameLabel->Draw();
    this->bossLabel->Draw();
    
    // Draw the boss icon
    glCallList(this->bossIconDisplayList);

    // When locked, draw the padlock
    if (!this->isEnabled) {
        this->DrawPadlock(dT);
    }
    else {
        if (this->bossDeadLabel != NULL) {
            this->bossDeadLabel->Draw();
        }
    }
}

float SelectLevelMenuState::BossLevelMenuItem::GetHeight() const {
    return static_cast<int>(std::max<float>(this->height, NUM_TO_BOSS_NAME_GAP + 
        std::max<float>(this->bossLabel->GetHeight(), this->bossIconSize) + this->nameLabel->GetHeight()));
}