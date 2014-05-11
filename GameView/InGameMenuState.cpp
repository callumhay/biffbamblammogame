/**
 * InGameMenuState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// BlammoEngine Includes
#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/StringHelper.h"

#include "InGameMenuState.h"
#include "InGameDisplayState.h"
#include "MainMenuDisplayState.h"
#include "LevelStartDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"

#include "../GameModel/GameModel.h"
#include "../GameSound/GameSound.h"
#include "../WindowManager.h"

const Colour InGameMenuState::MENU_ITEM_IDLE_COLOUR		= Colour(0.75f, 0.75f, 0.75f);
const Colour InGameMenuState::MENU_ITEM_SEL_COLOUR		= Colour(1, 0.8f, 0);
const Colour InGameMenuState::MENU_ITEM_ACTIVE_COLOUR	= Colour(1.0f, 1.0f, 1.0f);
const Colour InGameMenuState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);

const char* InGameMenuState::VERIFY_MENU_YES  = "Yes!";
const char* InGameMenuState::VERIFY_MENU_NO   = "NOoo!";

const int InGameMenuState::LEVEL_NAME_LABEL_X_BORDER = 50;
const int InGameMenuState::MENU_LABEL_GAP = 50;
const int InGameMenuState::NAME_HS_LABEL_GAP = 20;
const int InGameMenuState::LABEL_STAR_GAP = 5;
const int InGameMenuState::STAR_SIZE = 42;
const int InGameMenuState::STAR_HORIZONTAL_GAP = 3;
const int InGameMenuState::BOSS_ICON_SIZE = 40;

InGameMenuState::InGameMenuState(GameDisplay* display, DisplayState* returnToDisplayState) : 
DisplayState(display), renderPipeline(display), nextAction(InGameMenuState::Nothing),
topMenu(NULL), topMenuEventHandler(NULL), difficultyEventHandler(NULL), difficultyVerifyHandler(NULL),
restartVerifyHandler(NULL), exitGameVerifyHandler(NULL), returnToMainMenuVerifyHandler(NULL),
returnToDisplayState(returnToDisplayState), initialDifficultySelected(-1), starTexture(NULL), bossTexture(NULL),
highscoreLabel(GameFontAssetsManager::GetInstance()->GetFont(
               GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), ""),
levelNameLabel(GameFontAssetsManager::GetInstance()->GetFont(
               GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), 
               Camera::GetWindowWidth() - 2*LEVEL_NAME_LABEL_X_BORDER, "") {

    float scalingFactor = this->display->GetTextScalingFactor();

    GameModel* gameModel = this->display->GetModel();
    GameSound* sound = this->display->GetSound();
        
	// Pause all sounds and effects and play the paused sound
	sound->PauseAllSounds();
    sound->PauseAllEffects();
    sound->PlaySound(GameSound::InGameMenuOpened, false);

	// Pause the game itself
	gameModel->SetPause(GameModel::PauseGame);

	// Read the configuration file to figure out how to initialize each of the options
	this->cfgOptions = ResourceManager::ReadConfigurationOptions(true);

    const GameLevel* currentLevel = gameModel->GetCurrentLevel();

    this->levelNameLabel.SetText(currentLevel->GetName());
    this->levelNameLabel.SetScale(0.85f * scalingFactor);
    this->levelNameLabel.SetColour(Colour(0.9f, 0.9f, 0.9f));
    this->levelNameLabel.SetDropShadow(Colour(0,0,0), scalingFactor * 0.05f);

    std::stringstream highScoreStr;
    highScoreStr << "High Score: " << stringhelper::AddNumberCommas(currentLevel->GetHighScore());

    this->highscoreLabel.SetText(highScoreStr.str());
    this->highscoreLabel.SetScale(0.75f*scalingFactor);
    this->highscoreLabel.SetColour(Colour(0.8f, 0.8f, 0.8f));

    // Grab any required texture resources
    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);
    this->starTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->bossTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BOSS_ICON, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->bossTexture != NULL);
    this->bossTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    {
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = 1.0f; timeVals[2] = 2.0f;
        std::vector<Colour> colourVals(timeVals.size());
        colourVals[0] = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
        colourVals[1] = GameViewConstants::GetInstance()->BRIGHT_POINT_STAR_COLOUR;
        colourVals[2] = colourVals[0];
        this->activeStarColourAnim.SetLerp(timeVals, colourVals);
        this->activeStarColourAnim.SetRepeat(true);
    }

	this->InitTopMenu();

    this->display->GetMouse()->HideMouse();
    if (!WindowManager::GetInstance()->GetIsFullscreen()) {
        WindowManager::GetInstance()->ShowCursor(true);
    }
}

InGameMenuState::~InGameMenuState() {
	delete this->topMenu;
	this->topMenu = NULL;
	delete this->topMenuEventHandler;
	this->topMenuEventHandler = NULL;
    delete this->difficultyEventHandler;
    this->difficultyEventHandler = NULL;
    delete this->difficultyVerifyHandler;
    this->difficultyVerifyHandler = NULL;
    delete this->restartVerifyHandler;
    this->restartVerifyHandler = NULL;
    delete this->exitGameVerifyHandler;
    this->exitGameVerifyHandler = NULL;
    delete this->returnToMainMenuVerifyHandler;
    this->returnToMainMenuVerifyHandler = NULL;

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bossTexture);
    assert(success);
    UNUSED_VARIABLE(success);
}

/**
 * Render a frame - this will render the last frame from the game with an overlay of the
 * in-game menu in its current state according to interaction with the user.
 */
void InGameMenuState::RenderFrame(double dT) {
    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (this->nextAction) {

		case InGameMenuState::ResumeGame:
			this->ResumeTheGame();
			return;

        case InGameMenuState::RestartLevel: {
            GameSound* sound = this->display->GetSound();
            sound->SetIgnorePlaySound(true);

            // Clean up any misc. visual effects
			this->display->GetAssets()->DeactivateMiscEffects();
			
            // Kill all sounds
			sound->StopAllSounds();
            sound->StopAllEffects();

            // Reset the level
            this->display->GetModel()->ResetCurrentLevel();

            // Unpause the game and go to the start of level state which will have
            // been queued when we told the model to reset the level
            this->CleanUpReturnToDisplayState();
            sound->SetIgnorePlaySound(false);
            this->display->SetCurrentStateAsNextQueuedState();

            return;
        }

        case InGameMenuState::ReturnToMainMenu: {
            GameSound* sound = this->display->GetSound();
            sound->SetIgnorePlaySound(true);

            // Clean up any misc. visual effects
			this->display->GetAssets()->DeactivateMiscEffects();
			
            // Kill all sounds
			sound->StopAllSounds();
            sound->StopAllEffects();
            
			// Go back to the main menu state
            this->CleanUpReturnToDisplayState();
            sound->SetIgnorePlaySound(false);
			this->display->SetCurrentState(new MainMenuDisplayState(this->display));
			return;
        }

		case InGameMenuState::ExitToDesktop:
            this->CleanUpReturnToDisplayState();
			this->display->QuitGame();
			return;

		case InGameMenuState::Nothing:
			break;

		default:
			assert(false);
			break;
	}

	// Render the last frame from the in-game display state, stored in the FBO assets
	// this will be the 'background' for the menu...
	FBObj* lastFullscreenRendering = this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO();
	assert(lastFullscreenRendering != NULL);

	lastFullscreenRendering->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);
    renderPipeline.RenderHUD(0.0);

	// Render an overlay that dulls the background rendering...
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(),
        -1.0f, ColourRGBA(0.0f, 0.0f, 0.0f, 0.6f));
	
	// Draw the label(s) and menu
	glEnable(GL_DEPTH_TEST);
	this->topMenu->SetCenteredOnScreen(Camera::GetWindowWidth(), Camera::GetWindowHeight());
	
    float scalingFactor = this->display->GetTextScalingFactor();
    float prevYPos = this->topMenu->GetTopLeftCorner()[1] + scalingFactor * MENU_LABEL_GAP;

    if (this->display->GetModel()->GetCurrentLevel()->GetHasBoss()) {
        prevYPos += scalingFactor * BOSS_ICON_SIZE/2.0f;
        this->DrawBossIcon(prevYPos);
        prevYPos += scalingFactor * BOSS_ICON_SIZE + scalingFactor*LABEL_STAR_GAP;
    }
    else {
        // Only draw high score and stars on non-boss levels
        Point2D hsTopLeftCorner((Camera::GetWindowWidth() - this->highscoreLabel.GetLastRasterWidth()) / 2.0f,
            prevYPos + this->highscoreLabel.GetHeight());
        this->highscoreLabel.SetTopLeftCorner(hsTopLeftCorner);
        this->highscoreLabel.Draw();

        prevYPos = hsTopLeftCorner[1] + scalingFactor * LABEL_STAR_GAP + scalingFactor * STAR_SIZE;
        this->DrawStars(dT, prevYPos);
        prevYPos += scalingFactor * LABEL_STAR_GAP;
    }

    Point2D nameLblTopLeftCorner((Camera::GetWindowWidth() - this->levelNameLabel.GetWidth()) / 2.0f,
        prevYPos + this->levelNameLabel.GetHeight());
    this->levelNameLabel.SetTopLeftCorner(nameLblTopLeftCorner);
    this->levelNameLabel.Draw();
    
    this->topMenu->Draw(dT, Camera::GetWindowWidth(), Camera::GetWindowHeight());
	glPopAttrib();
}

// Make sure this is called if we aren't going back to the returnToDisplayState
// i.e., if we exit to main menu, desktop, etc.
void InGameMenuState::CleanUpReturnToDisplayState() {
    if (this->returnToDisplayState != NULL) {
        delete this->returnToDisplayState;
        this->returnToDisplayState = NULL;
    }
}

/**
 * Called whenever a key is pressed while in this state.
 */
void InGameMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                    const GameControl::ActionMagnitude& magnitude) {
	                                 
    assert(this->topMenu != NULL);

	// If the pause button is hit again then just exit this menu back to the game...
	if (pressedButton == GameControl::PauseButtonAction) {
		this->ResumeTheGame();
	}
	else {
		// Tell the top-most menu about the key pressed event
		this->topMenu->ButtonPressed(pressedButton, magnitude);
	}
}

/**
 * Called whenever a key is released while in this state.
 */
void InGameMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	assert(this->topMenu != NULL);

	// Tell the top-most menu about the key released event
	this->topMenu->ButtonReleased(releasedButton);
}

void InGameMenuState::DrawStars(double dT, float currYPos) {
    
    GameModel* gameModel = this->display->GetModel();
    GameLevel* currentLevel = gameModel->GetCurrentLevel();

    int maxStarsAwardedForCurrLevel = currentLevel->GetHighScoreNumStars();
    float scalingFactor = this->display->GetTextScalingFactor();

    const float STAR_SIZE_SCALED = scalingFactor * STAR_SIZE;
    const float STAR_HORIZONTAL_GAP_SCALED = scalingFactor * STAR_HORIZONTAL_GAP;

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->starTexture->BindTexture();

    float currX = Camera::GetWindowWidth() / 2.0f - ((STAR_SIZE_SCALED*GameLevel::MAX_STARS_PER_LEVEL + STAR_HORIZONTAL_GAP*(GameLevel::MAX_STARS_PER_LEVEL-1))/2.0f);
    currX += STAR_SIZE_SCALED / 2.0f;
    currYPos -= STAR_SIZE_SCALED / 2.0f;

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (maxStarsAwardedForCurrLevel > 0) {
        this->activeStarColourAnim.Tick(dT);
    }

    Colour starColour;
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {

        //AnimationLerp<float>* starAnimation = this->starAnimations[i];
        if (i < maxStarsAwardedForCurrLevel) {
            starColour = this->activeStarColourAnim.GetInterpolantValue();
        }
        else {
            starColour = GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR;
        }

        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);

        glPushMatrix();
        glTranslatef(currX, currYPos, 0);
        glScalef(STAR_SIZE_SCALED, STAR_SIZE_SCALED, 1.0f);
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
        
        currX += STAR_SIZE_SCALED + STAR_HORIZONTAL_GAP_SCALED;
    }
    
    glPopAttrib();

    glPopMatrix();
    Camera::PopWindowCoords();

    debug_opengl_state();
}

void InGameMenuState::DrawBossIcon(float currYPos) {

    float scalingFactor = this->display->GetTextScalingFactor();
    const float BOSS_ICON_SIZE_SCALED = scalingFactor * BOSS_ICON_SIZE;

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->bossTexture->BindTexture();

    float currXPos = Camera::GetWindowWidth() / 2.0f;

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(currXPos, currYPos, 0);
    glScalef(BOSS_ICON_SIZE_SCALED, BOSS_ICON_SIZE_SCALED, 1.0f);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();

    debug_opengl_state();
}

void InGameMenuState::InitTopMenu() {
	assert(this->topMenu == NULL);
	assert(this->topMenuEventHandler == NULL);

	float textScaleFactor = this->display->GetTextScalingFactor();

	// Set up the handlers
	this->topMenuEventHandler    = new TopMenuEventHandler(this);
    this->difficultyEventHandler = new DifficultyEventHandler(this);

    this->difficultyVerifyHandler       = new DifficultyVerifyEventHandler(this);
    this->restartVerifyHandler          = new RestartVerifyEventHandler(this);
    this->exitGameVerifyHandler         = new ExitGameVerifyEventHandler(this);
    this->returnToMainMenuVerifyHandler = new ReturnToMainMenuVerifyEventHandler(this);

	// Setup the top menu attributes
	this->topMenu = new GameMenu();
	this->topMenu->SetAlignment(GameMenu::CenterJustified);
	this->topMenu->AddEventHandler(this->topMenuEventHandler);
	this->topMenu->SetColourScheme(InGameMenuState::MENU_ITEM_IDLE_COLOUR, InGameMenuState::MENU_ITEM_SEL_COLOUR, 
	    InGameMenuState::MENU_ITEM_ACTIVE_COLOUR, InGameMenuState::MENU_ITEM_GREYED_COLOUR);

	// Prepare some of the properties of the text labels in the top level menu...
	const float dropShadowAmtSm = 0.10f;
	const float dropShadowAmtLg = 0.10f;
	const Colour dropShadowColour = Colour(0.0f, 0.0f, 0.0f);

    static const char* VERIFY_EXIT_TO_MAIN_MENU_TEXT = "All progress in this level will be lost, exit to the main menu?";
    static const char* VERIFY_RESTART_TEXT           = "Are you sure you want to restart this level?";
    static const char* VERIFY_EXIT_TO_DESKTOP_TEXT   = "All progress in this level will be lost, are you sure you want to exit the game?";

    // Resume Item...
	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
        GameFontAssetsManager::Medium), "Resume");
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
        GameFontAssetsManager::Big),    "Resume");
    tempLabelSm.SetScale(textScaleFactor);
    tempLabelLg.SetScale(textScaleFactor);
	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);

	// Add items to the menu in their order (first to last)
	this->resumeItem = this->topMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
	
    // Restart Item...
	tempLabelSm.SetText("Restart Level");
	tempLabelLg.SetText("Restart Level");

	VerifyMenuItem* restartLevelItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	restartLevelItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	restartLevelItem->SetVerifyMenuText(VERIFY_RESTART_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    restartLevelItem->SetEventHandler(this->restartVerifyHandler);

    this->restartItem = this->topMenu->AddMenuItem(restartLevelItem);

    // We don't allow a difficulty item to be part of the menu if the player is in the tutorial
    if (this->returnToDisplayState->GetType() == DisplayState::InTutorialGame) {
        this->difficultyItem = GameMenu::NO_MENU_ITEM_INDEX;
    }
    else {

        // Add the difficulty item to the menu...
        static const int POPUP_WIDTH = 750;
        this->initialDifficultySelected = static_cast<int>(this->cfgOptions.GetDifficulty());
        
        tempLabelSm.SetText("");
	    tempLabelLg.SetText("");
	    VerifyMenuItem* verifyDifficultyMenuItem  = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	    verifyDifficultyMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	    verifyDifficultyMenuItem->SetVerifyMenuText("Changing the difficulty will restart the current level. Are you sure?", VERIFY_MENU_YES, VERIFY_MENU_NO);
        verifyDifficultyMenuItem->SetEventHandler(this->difficultyVerifyHandler);

        tempLabelSm.SetText("Difficulty");
	    tempLabelLg.SetText("Difficulty");
        std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();

        this->difficultyMenuItem = new SelectionListMenuItemWithVerify(tempLabelSm, tempLabelLg, difficultyOptions, verifyDifficultyMenuItem);
        this->difficultyMenuItem->SetSelectedItem(this->initialDifficultySelected);
        this->difficultyMenuItem->SetEventHandler(this->difficultyEventHandler);

        this->difficultyItem = this->topMenu->AddMenuItem(this->difficultyMenuItem); 
    }

	// The exit/return to main menu item has a verify menu...
	tempLabelSm.SetText("Return to Main Menu");
	tempLabelLg.SetText("Return to Main Menu");

	VerifyMenuItem* returnToMainMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	returnToMainMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	returnToMainMenuItem->SetVerifyMenuText(VERIFY_EXIT_TO_MAIN_MENU_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    returnToMainMenuItem->SetEventHandler(this->returnToMainMenuVerifyHandler);

	this->returnToMainItem = this->topMenu->AddMenuItem(returnToMainMenuItem);
	
	// The exit to desktop menu item has a verify menu...
	tempLabelSm.SetText("Exit Game");
	tempLabelLg.SetText("Exit Game");

	VerifyMenuItem* exitToDesktopMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	exitToDesktopMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	exitToDesktopMenuItem->SetVerifyMenuText(VERIFY_EXIT_TO_DESKTOP_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    exitToDesktopMenuItem->SetEventHandler(this->exitGameVerifyHandler);

	this->exitToDesktopItem = this->topMenu->AddMenuItem(exitToDesktopMenuItem);

	this->topMenu->SetSelectedMenuItem(this->resumeItem, false);
}

void InGameMenuState::ResumeTheGame() {
    WindowManager::GetInstance()->ShowCursor(false);

    GameSound* sound = this->display->GetSound();

    // Play unpause all sounds and effects
    sound->PlaySound(GameSound::InGameMenuClosed, false);
    // Resume world sounds - these are initially paused when coming to this state (in the constructor)
    sound->UnpauseAllEffects();
    sound->UnpauseAllSounds();

    // Unpause the game
    this->display->GetModel()->UnsetPause(GameModel::PauseGame);

    // Go back to the in-game display state
    if (this->returnToDisplayState != NULL) {
        this->display->SetCurrentState(returnToDisplayState);
    }
    else {
        this->display->SetCurrentState(new InGameDisplayState(this->display));
    }
}

// Top Menu Handler Functions ***********************************************************

void InGameMenuState::TopMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);

    // Play the sound effect assoicated with menu item changing/being highlighted by the user
    GameSound* sound = this->inGameMenuState->display->GetSound();
    sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false);
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {

    //GameSound* sound = this->inGameMenuState->display->GetSound();

	if (itemIndex == this->inGameMenuState->resumeItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ResumeGame;
	}
    else if (itemIndex == this->inGameMenuState->restartItem) {

    }
    else if (itemIndex == this->inGameMenuState->difficultyItem) {

    }
    else if (itemIndex == this->inGameMenuState->returnToMainItem) {

    }
    else if (itemIndex == this->inGameMenuState->exitToDesktopItem) {

    }
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);
}

void InGameMenuState::TopMenuEventHandler::EscMenu() {
	GameMenu* topMenu = this->inGameMenuState->topMenu;
	assert(topMenu != NULL);

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (topMenu->GetSelectedMenuItem() != this->inGameMenuState->resumeItem) {
		topMenu->SetSelectedMenuItem(this->inGameMenuState->resumeItem, true);
	}
	else {
		topMenu->ActivateSelectedMenuItem();
	}
}

// Verify Menu Event Handler ******************************************************
InGameMenuState::RestartVerifyEventHandler::RestartVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::RestartVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::RestartLevel;
}

InGameMenuState::ExitGameVerifyEventHandler::ExitGameVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ExitGameVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::ExitToDesktop;
}

InGameMenuState::ReturnToMainMenuVerifyEventHandler::ReturnToMainMenuVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ReturnToMainMenuVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::ReturnToMainMenu;
}

InGameMenuState::DifficultyEventHandler::DifficultyEventHandler(InGameMenuState *inGameMenuState) : 
SelectionListEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

InGameMenuState::DifficultyVerifyEventHandler::DifficultyVerifyEventHandler(InGameMenuState* inGameMenuState) :
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::DifficultyVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();

    GameModel* gameModel = this->inGameMenuState->display->GetModel();
    assert(!gameModel->IsCurrentLevelTheTutorialLevel());

    // If the difficulty is actually being changed then will have to restart the current level...
    int currSelectionIdx = this->inGameMenuState->difficultyMenuItem->GetSelectedItemIndex();
    GameModel::Difficulty difficultyToSet = static_cast<GameModel::Difficulty>(currSelectionIdx);
    this->inGameMenuState->cfgOptions.SetDifficulty(difficultyToSet);
    gameModel->SetDifficulty(difficultyToSet);
    
    // Rewrite the configuration file to accomodate the change in difficulty
    ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->inGameMenuState->cfgOptions);

    // Flag to restart the level...
    this->inGameMenuState->nextAction = InGameMenuState::RestartLevel;
}

void InGameMenuState::DifficultyVerifyEventHandler::MenuItemCancelled() {
    VerifyMenuEventHandlerWithSound::MenuItemCancelled();

    GameModel* gameModel = this->inGameMenuState->display->GetModel();
    this->inGameMenuState->difficultyMenuItem->SetSelectedItem(static_cast<int>(gameModel->GetDifficulty()));
}