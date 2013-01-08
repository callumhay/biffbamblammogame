/**
 * GameOverDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameOverDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const char* GameOverDisplayState::GAME_OVER_TEXT         = "Game Over";
const int GameOverDisplayState::GAME_OVER_LABEL_X_BORDER = 50;
const int GameOverDisplayState::GAME_OVER_LABEL_Y_BORDER = GameOverDisplayState::GAME_OVER_LABEL_X_BORDER;

const Colour GameOverDisplayState::MENU_ITEM_IDLE_COLOUR    = Colour(0.75f, 0.75f, 0.75f);
const Colour GameOverDisplayState::MENU_ITEM_SEL_COLOUR		= Colour(1, 0.8f, 0);
const Colour GameOverDisplayState::MENU_ITEM_ACTIVE_COLOUR	= Colour(1.0f, 1.0f, 1.0f);
const Colour GameOverDisplayState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);

GameOverDisplayState::GameOverDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display), gameOverMenu(NULL),
deathFSEffect(display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()),
maxMenuItemWidth(0.0f), menuHeight(0.0f), selectedAndActivatedItem(-1) {

	this->gameOverLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Huge), GAME_OVER_TEXT);
	this->gameOverLabel.SetColour(Colour(1, 0, 0));
	this->gameOverLabel.SetDropShadow(Colour(0.85f, 0.85f, 0.85f), 0.05f);
    this->gameOverLabel.SetScale(1.25f * this->display->GetTextScalingFactor());

    this->SetupMenu();
    this->SetState(FadeToDeathState);
}

GameOverDisplayState::~GameOverDisplayState() {
    this->renderPipeline.SetHUDAlpha(1.0f);

    delete this->gameOverMenu;
    this->gameOverMenu = NULL;
    delete this->topMenuEventHandler;
    this->topMenuEventHandler = NULL;
}

void GameOverDisplayState::RenderFrame(double dT) {
    this->UpdateAndDrawState(dT);
}

void GameOverDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {
	UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);

    if (this->currState == IdleState) {
        this->gameOverMenu->ButtonPressed(pressedButton);
    }
}

void GameOverDisplayState::UpdateAndDrawState(double dT) {
	const Camera& camera = this->display->GetCamera();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    this->deathFSEffect.Draw(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);

    this->gameOverLabel.SetTopLeftCorner(this->moveLabelAnim.GetInterpolantValue(),
        camera.GetWindowHeight() - GAME_OVER_LABEL_Y_BORDER);
	this->gameOverLabel.Draw();

    this->gameOverMenu->SetTopLeftCorner(this->moveMenuAnim.GetInterpolantValue(),
        this->menuHeight + GAME_OVER_LABEL_Y_BORDER);
    this->gameOverMenu->Draw(dT, camera.GetWindowWidth(), camera.GetWindowHeight());

    glPopAttrib();

    switch (this->currState) {

        case FadeToDeathState: {
            this->renderPipeline.RenderHUDWithAlpha(dT, this->fastFadeAnim.GetInterpolantValue());
            
            this->deathFSEffect.SetIntensity(this->fadeAnim.GetInterpolantValue());
            this->gameOverLabel.SetAlpha(this->fadeAnim.GetInterpolantValue());

            if (this->fastFadeAnim.Tick(dT) && this->fadeAnim.Tick(dT) &&
                this->moveLabelAnim.Tick(dT)) {
                this->SetState(ShowingTextState);
            }
            break;
        }

        case ShowingTextState:
            if (this->moveMenuAnim.Tick(dT)) {
                this->SetState(IdleState);
            }
            break;

        case IdleState:
            break;

        case FadeOutState:

            this->deathFSEffect.SetAlpha(this->fadeAnim.GetInterpolantValue());
            this->gameOverLabel.SetAlpha(this->fadeAnim.GetInterpolantValue());

            if (this->moveMenuAnim.Tick(dT) && this->fadeAnim.Tick(dT)) {

                assert(this->selectedAndActivatedItem >= 0);
                if (this->selectedAndActivatedItem == this->retryMenuItem) {
			        // Clean up any misc. visual effects
			        this->display->GetAssets()->DeactivateMiscEffects();
			        // Kill all sounds
			        this->display->GetAssets()->GetSoundAssets()->StopAllSounds();

                    // Reset the level
                    this->display->GetModel()->ResetCurrentLevel();

                    this->display->SetCurrentStateAsNextQueuedState();
                }
                else if (this->selectedAndActivatedItem == this->backToMainMenuItem) {
                    this->display->SetCurrentState(new MainMenuDisplayState(this->display));
                }
                else {
                    assert(this->selectedAndActivatedItem == this->exitGameItem);
                    this->display->QuitGame();
                }
            }

            break;

        default:
            assert(false);
            return;
    }
}

void GameOverDisplayState::SetState(const AnimationState& state) {
    switch (state) {

        case FadeToDeathState: {
            double menuStartTime = 0.33;
            this->fadeAnim.SetLerp(menuStartTime, 0.75, 0.0f, 1.0f);
            this->fadeAnim.SetInterpolantValue(0.0f);
            this->fadeAnim.SetRepeat(false);

            this->fastFadeAnim.SetLerp(0.0, 0.33, 1.0f, 0.0f);
            this->fastFadeAnim.SetInterpolantValue(1.0f);
            this->fastFadeAnim.SetRepeat(false);

            float startXPos = -(10 + this->maxMenuItemWidth);

            std::vector<float> moveValues;
            moveValues.reserve(3);
            moveValues.push_back(startXPos);
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER + 40);
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER);

            std::vector<double> timeValues;
            timeValues.reserve(3);
            timeValues.push_back(0.0);
            timeValues.push_back(0.22);
            timeValues.push_back(0.33);

            this->moveMenuAnim.SetLerp(timeValues, moveValues);
            this->moveMenuAnim.SetInterpolantValue(startXPos);
            this->moveMenuAnim.SetRepeat(false);

            timeValues.clear();
            moveValues.clear();

            startXPos = -(this->gameOverLabel.GetLastRasterWidth()+10);
            moveValues.reserve(3);
            moveValues.push_back(startXPos);
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER + 40);
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER);
            
            timeValues.reserve(3);
            timeValues.push_back(menuStartTime);
            timeValues.push_back(menuStartTime + 0.22);
            timeValues.push_back(menuStartTime + 0.33);

            this->moveLabelAnim.SetLerp(timeValues, moveValues);
            this->moveLabelAnim.SetInterpolantValue(startXPos);
            this->moveLabelAnim.SetRepeat(false);

            break;
        }

        case ShowingTextState:
            break;

        case IdleState:
            break;

        case FadeOutState: {
            this->fadeAnim.SetLerp(0.45, 0.0f);

            std::vector<float> moveValues;
            moveValues.reserve(3);
            
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER);
            moveValues.push_back(GAME_OVER_LABEL_X_BORDER + 50);
            moveValues.push_back(-(10 + this->maxMenuItemWidth));

            std::vector<double> timeValues;
            timeValues.reserve(3);
            timeValues.push_back(0.0);
            timeValues.push_back(0.1);
            timeValues.push_back(0.25);

            this->moveMenuAnim.SetLerp(timeValues, moveValues);
            break;
        }

        default:
            assert(false);
            return;
    }

    this->currState = state;
}

void GameOverDisplayState::SetupMenu() {
    assert(this->gameOverMenu == NULL);
    
    this->menuHeight = 0.0f;
    this->maxMenuItemWidth = 0.0f;

	const float textScaleFactor = this->display->GetTextScalingFactor();

	// Set up the handlers
	this->topMenuEventHandler    = new TopMenuEventHandler(this);

    this->gameOverMenu = new GameMenu();
	this->gameOverMenu->AddEventHandler(this->topMenuEventHandler);
	this->gameOverMenu->SetColourScheme(MENU_ITEM_IDLE_COLOUR, MENU_ITEM_SEL_COLOUR, 
	    MENU_ITEM_ACTIVE_COLOUR, MENU_ITEM_GREYED_COLOUR);

	// Prepare some of the properties of the text labels in the top level menu...
	const float dropShadowAmtSm = 0.10f;
	const float dropShadowAmtLg = 0.10f;
	const Colour dropShadowColour = Colour(0.0f, 0.0f, 0.0f);

	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),  "Retry");
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),     "Retry");
	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelSm.SetScale(textScaleFactor);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);
	tempLabelLg.SetScale(textScaleFactor);

    // Retry Item...
    this->retryMenuItem = this->gameOverMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
    this->maxMenuItemWidth = std::max<float>(this->maxMenuItemWidth, tempLabelLg.GetLastRasterWidth());
    this->menuHeight += tempLabelLg.GetHeight() + this->gameOverMenu->GetMenuItemPadding();

    // Back to main menu item...
	tempLabelSm.SetText("Back To Main Menu");
	tempLabelLg.SetText("Back To Main Menu");
    this->backToMainMenuItem = this->gameOverMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
    this->maxMenuItemWidth = std::max<float>(this->maxMenuItemWidth, tempLabelLg.GetLastRasterWidth());
    this->menuHeight += tempLabelLg.GetHeight() + this->gameOverMenu->GetMenuItemPadding();

    // Exit game...
	tempLabelSm.SetText("Rage Quit");
	tempLabelLg.SetText("Rage Quit");

   
	VerifyMenuItem* exitMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));
	exitMenuItem->SetVerifyMenuColours(Colour(1,1,1), Colour(0.5f, 0.5f, 0.5f), Colour(1,1,1));
	exitMenuItem->SetVerifyMenuText("Are you sure you want to quit?", "Yes", "No");
	//exitMenuItem->SetEventHandler(???);

    this->exitGameItem = this->gameOverMenu->AddMenuItem(exitMenuItem);
    this->maxMenuItemWidth = std::max<float>(this->maxMenuItemWidth, tempLabelLg.GetLastRasterWidth());
    this->menuHeight += tempLabelLg.GetHeight();

	this->gameOverMenu->SetSelectedMenuItem(this->retryMenuItem);
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	// Play the sound effect assoicated with menu item selection/activation
	//GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
    
    if (itemIndex != this->state->exitGameItem) {
        this->state->selectedAndActivatedItem = itemIndex;
        this->state->SetState(FadeOutState);
    }
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemVerifiedEvent(int itemIndex) {
    if (itemIndex == this->state->exitGameItem) {

        //GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
		//soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);

        // We exit the game if the exit game item has both been activated and verified...
		this->state->display->QuitGame();
    }
}

void GameOverDisplayState::TopMenuEventHandler::EscMenu() {
}