/**
 * GameOverDisplayState.cpp
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

// GameDisplay Includes
#include "GameOverDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"

const char* GameOverDisplayState::GAME_OVER_TEXT         = "Game Over";
const int GameOverDisplayState::GAME_OVER_LABEL_X_BORDER = 50;
const int GameOverDisplayState::GAME_OVER_LABEL_Y_BORDER = GameOverDisplayState::GAME_OVER_LABEL_X_BORDER;

const Colour GameOverDisplayState::MENU_ITEM_IDLE_COLOUR    = Colour(0.75f, 0.75f, 0.75f);
const Colour GameOverDisplayState::MENU_ITEM_SEL_COLOUR		= Colour(1, 0.8f, 0);
const Colour GameOverDisplayState::MENU_ITEM_ACTIVE_COLOUR	= Colour(1.0f, 1.0f, 1.0f);
const Colour GameOverDisplayState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);

GameOverDisplayState::GameOverDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display), gameOverMenu(NULL), starryBG(NULL), topMenuEventHandler(NULL), quitSubMenuEventHandler(NULL),
deathFSEffect(display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()), arcadeCountdownSecs(10.9), continueButtonPushed(false),
maxMenuItemWidth(0.0f), menuHeight(0.0f), selectedAndActivatedItem(-1), gameOverSoundID(INVALID_SOUND_ID) {

	this->gameOverLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Huge), GAME_OVER_TEXT);
	this->gameOverLabel.SetColour(Colour(1, 0, 0));
	this->gameOverLabel.SetDropShadow(Colour(0.85f, 0.85f, 0.85f), 0.05f);
    this->gameOverLabel.SetScale(1.25f * this->display->GetTextScalingFactor());

    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD_BG, Texture::Trilinear));
    assert(this->starryBG != NULL);

    this->SetupMenu();
    this->SetState(FadeToDeathState);
}

GameOverDisplayState::~GameOverDisplayState() {
    this->renderPipeline.SetHUDAlpha(1.0f);

    if (this->gameOverMenu != NULL) {
        delete this->gameOverMenu;
        this->gameOverMenu = NULL;
    }
    if (this->topMenuEventHandler != NULL) {
        delete this->topMenuEventHandler;
        this->topMenuEventHandler = NULL;
    }
    if (this->quitSubMenuEventHandler != NULL) {
        delete this->quitSubMenuEventHandler;
        this->quitSubMenuEventHandler = NULL;
    }

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);
    UNUSED_VARIABLE(success);

    // Make sure the game over sound is stopped
    this->display->GetSound()->StopSound(this->gameOverSoundID, 0.5);
}

void GameOverDisplayState::RenderFrame(double dT) {
    this->UpdateAndDrawState(dT);
}

void GameOverDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    if (GameDisplay::IsArcadeModeEnabled() && !this->continueButtonPushed) {
        if (this->currState == IdleState || this->currState == FadeOutState) {
            this->display->GetSound()->PlaySound(GameSound::MenuItemVerifyAndSelectStartGameEvent, false);
            this->continueButtonPushed = true;
            this->SetState(FadeOutState);
        }
    }
    else {
        if (this->currState == IdleState) {
            this->gameOverMenu->ButtonPressed(pressedButton, magnitude);
        }
    }
}

void GameOverDisplayState::UpdateAndDrawState(double dT) {
    GameSound* sound = this->display->GetSound();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Draw the starry background...
    this->starryBG->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(Camera::GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()));
    this->starryBG->UnbindTexture();

    this->deathFSEffect.Draw(Camera::GetWindowWidth(), Camera::GetWindowHeight(), dT);

    this->gameOverLabel.SetTopLeftCorner(this->moveLabelAnim.GetInterpolantValue(),
        Camera::GetWindowHeight() - GAME_OVER_LABEL_Y_BORDER);
	this->gameOverLabel.Draw();

    // Player doesn't get a menu in arcade mode... it's game over for good.
    if (!GameDisplay::IsArcadeModeEnabled()) {
        this->gameOverMenu->SetTopLeftCorner(this->moveMenuAnim.GetInterpolantValue(),
            this->menuHeight + GAME_OVER_LABEL_Y_BORDER);
        this->gameOverMenu->Draw(dT, Camera::GetWindowWidth(), Camera::GetWindowHeight());
    }

    glPopAttrib();

    switch (this->currState) {

        case FadeToDeathState: {
            this->renderPipeline.RenderHUDWithAlpha(dT, this->fastFadeAnim.GetInterpolantValue());
            
            this->deathFSEffect.SetIntensity(this->fadeAnim.GetInterpolantValue());
            this->gameOverLabel.SetAlpha(this->fadeAnim.GetInterpolantValue());

            if (this->fastFadeAnim.Tick(dT) && this->fadeAnim.Tick(dT) && this->moveLabelAnim.Tick(dT)) {
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
            // In arcade mode we have a max time to spend on this screen before we go back to the main menu
            if (GameDisplay::IsArcadeModeEnabled()) {
                // Draw a countdown before we go back to the main menu...
                // "Continue? 10..9.. etc.
                this->continueLabelColourAnim.Tick(dT);
                
                std::stringstream strStream;
                strStream << "Continue? ..." << std::max<int>(0, static_cast<int>(floor(this->arcadeCountdownSecs)));

                this->arcadeContinueLabel.SetText(strStream.str());
                this->arcadeContinueLabel.SetColour(this->continueLabelColourAnim.GetInterpolantValue());
                this->arcadeContinueLabel.SetTopLeftCorner(this->moveMenuAnim.GetInterpolantValue(), 
                    this->arcadeContinueLabel.GetHeight() + GAME_OVER_LABEL_Y_BORDER);
                
                this->arcadeContinueLabel.Draw();

                this->arcadeCountdownSecs -= (dT*0.5);
                if (this->arcadeCountdownSecs < 0) {
                    this->SetState(FadeOutState);
                }
            }
            break;

        case FadeOutState:

            if (GameDisplay::IsArcadeModeEnabled()) {
                this->continueLabelColourAnim.Tick(dT);
                this->arcadeContinueLabel.SetColour(this->continueLabelColourAnim.GetInterpolantValue());
                this->arcadeContinueLabel.SetAlpha(this->fadeAnim.GetInterpolantValue());
                this->arcadeContinueLabel.Draw();
            }

            this->deathFSEffect.SetAlpha(this->fadeAnim.GetInterpolantValue());
            this->gameOverLabel.SetAlpha(this->fadeAnim.GetInterpolantValue());

            if (this->moveMenuAnim.Tick(dT) && this->fadeAnim.Tick(dT)) {
                sound->StopAllEffects();

                if (GameDisplay::IsArcadeModeEnabled()) {
                    if (this->continueButtonPushed) {

                        // Clean up any misc. visual effects
                        this->display->GetAssets()->DeactivateMiscEffects();
                        // Reset the level
                        this->display->GetModel()->ResetCurrentLevel();
                        this->display->SetCurrentStateAsNextQueuedState();
                    }
                    else {
                        this->display->SetCurrentState(new MainMenuDisplayState(this->display));
                    }
                }
                else {
                    assert(this->selectedAndActivatedItem >= 0);
                    if (this->selectedAndActivatedItem == this->retryMenuItem) {

			            // Clean up any misc. visual effects
			            this->display->GetAssets()->DeactivateMiscEffects();
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

            // Fade out all sounds and play the game over event sound
            GameSound* sound = this->display->GetSound();
            sound->StopAllSounds(0.5);
            this->gameOverSoundID = sound->PlaySound(GameSound::GameOverEvent, false, false);
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
    
    const float textScaleFactor = this->display->GetTextScalingFactor();

    if (GameDisplay::IsArcadeModeEnabled()) {
        this->continueLabelColourAnim = GameViewConstants::GetInstance()->BuildFlashingColourAnimation();

        this->arcadeContinueLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "Continue?");
        this->arcadeContinueLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
        this->arcadeContinueLabel.SetScale(textScaleFactor);
    }
    else {
        this->menuHeight = 0.0f;
        this->maxMenuItemWidth = 0.0f;

	    // Set up the handlers
	    this->topMenuEventHandler     = new TopMenuEventHandler(this);
        this->quitSubMenuEventHandler = new QuitVerifyMenuEventHandler(this);

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
	    exitMenuItem->SetEventHandler(this->quitSubMenuEventHandler);

        this->exitGameItem = this->gameOverMenu->AddMenuItem(exitMenuItem);
        this->maxMenuItemWidth = std::max<float>(this->maxMenuItemWidth, tempLabelLg.GetLastRasterWidth());
        this->menuHeight += tempLabelLg.GetHeight();

	    this->gameOverMenu->SetSelectedMenuItem(this->retryMenuItem, false);
    }
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);

    // Play the sound effect associated with menu item changing/being highlighted by the user
    GameSound* sound = this->state->display->GetSound();
    sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false);
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {

	GameSound* sound = this->state->display->GetSound();

    if (itemIndex == this->state->retryMenuItem) {
        sound->PlaySound(GameSound::MenuItemVerifyAndSelectStartGameEvent, false);
    }
    else if (itemIndex == this->state->backToMainMenuItem) {
        sound->PlaySound(GameSound::MenuItemVerifyAndSelectStartGameEvent, false);
    }

    if (itemIndex != this->state->exitGameItem) {
        this->state->selectedAndActivatedItem = itemIndex;
        this->state->SetState(FadeOutState);
    }
}

void GameOverDisplayState::TopMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);
}

void GameOverDisplayState::TopMenuEventHandler::EscMenu() {
}


GameOverDisplayState::QuitVerifyMenuEventHandler::QuitVerifyMenuEventHandler(GameOverDisplayState* state) : 
VerifyMenuEventHandlerWithSound(state->display->GetSound()), state(state) {
}

void GameOverDisplayState::QuitVerifyMenuEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(500);
    // We exit the game if the exit game item has both been activated and verified...
    this->state->display->QuitGame();
}
