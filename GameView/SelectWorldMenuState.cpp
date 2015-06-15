/**
 * SelectWorldMenuState.cpp
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

#include "SelectWorldMenuState.h"
#include "SelectLevelMenuState.h"
#include "KeyboardHelperLabel.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "MenuBackgroundRenderer.h"
#include "GameViewEventManager.h"

#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameProgressIO.h"

const char* SelectWorldMenuState::WORLD_SELECT_TITLE = "Select a Movement...";

const float SelectWorldMenuState::SELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP   = 50;
const float SelectWorldMenuState::UNSELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP = 25;

const float SelectWorldMenuState::SELECTED_MENU_ITEM_SIZE   = 256;
const float SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE = 128;

SelectWorldMenuState::SelectWorldMenuState(GameDisplay* display, const DisplayStateInfo& info, SoundID bgSoundLoopID) :
DisplayState(display), bgSoundLoopID(bgSoundLoopID), pressEscAlphaAnim(0.0f), worldSelectTitleLbl(NULL), keyEscLabel(NULL), 
goBackToMainMenu(false), menuFBO(NULL), postMenuFBObj(NULL), itemActivated(false), padlockTex(NULL),
goToLevelSelectMoveAnim(0.0f), goToLevelSelectAlphaAnim(1.0f), starTexture(NULL), worldUnlockAnim(NULL),
totalNumGameStarsLabel(NULL), totalLabel(NULL), starGlowTexture(NULL) {
    this->Init(info);
}

SelectWorldMenuState::~SelectWorldMenuState() {

    delete this->worldSelectTitleLbl;
    this->worldSelectTitleLbl = NULL;

    delete this->keyEscLabel;
    this->keyEscLabel = NULL;

    delete this->totalNumGameStarsLabel;
    this->totalNumGameStarsLabel = NULL;
    delete this->totalLabel;
    this->totalLabel = NULL;

    delete this->menuFBO;
    this->menuFBO = NULL;
    delete this->postMenuFBObj;
    this->postMenuFBObj = NULL;

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
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->padlockTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starGlowTexture);
    assert(success);
    UNUSED_VARIABLE(success);

    if (this->worldUnlockAnim != NULL) {
        delete this->worldUnlockAnim;
        this->worldUnlockAnim = NULL;
    }
}

void SelectWorldMenuState::RenderFrame(double dT) {
    const int selectedWorldNum = this->selectedItemIdx + 1;

    this->pressEscAlphaAnim.Tick(dT);
    const Camera& camera = this->display->GetCamera();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

    // Bind the menu's FrameBufferObject - we use this to do Bloom on the menu
	this->menuFBO->BindFBObj();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the background...
    bgRenderer->DrawBG(camera);

	// Draw in window coordinates
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();

    // Draw the title...
    this->worldSelectTitleLbl->SetTopLeftCorner(HORIZONTAL_TITLE_GAP, Camera::GetWindowHeight() - VERTICAL_TITLE_GAP);
    this->worldSelectTitleLbl->Draw();

    // Draw the "Press esc to go back" stuff
    this->keyEscLabel->SetAlpha(this->pressEscAlphaAnim.GetInterpolantValue());
    this->keyEscLabel->SetTopLeftCorner(20, this->keyEscLabel->GetHeight()); 
    this->keyEscLabel->Draw();

    // Draw the star total for the game
    this->DrawStarTotal(dT);

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
        worldItem->Draw(camera, dT, selectedWorldNum);
    }

    // Perform any necessary world unlocking animation
    if (this->worldUnlockAnim != NULL) {
        this->worldUnlockAnim->Draw(camera, dT, *this->postMenuFBObj->GetFBOTexture());
    }
    
    
    // Draw a fade overlay if necessary
    bool fadeDone = this->fadeAnimation.Tick(dT);
    if (!fadeDone || this->goBackToMainMenu) {
        bgRenderer->DrawBG(camera, this->fadeAnimation.GetInterpolantValue());
    }

    glPopMatrix();
    Camera::PopWindowCoords();

	this->menuFBO->UnbindFBObj();

    if (this->itemActivated) {
        if (this->display->IsArcadeModeEnabled()) {
            // Different type of animation for going straight into a level: Flash the screen and fade out...
            bool animDone = this->arcadeFlashAnim.Tick(dT);
            animDone &= this->goToLevelSelectAlphaAnim.Tick(dT);
            
            float flashAmt = this->arcadeFlashAnim.GetInterpolantValue();
            float alphaAmt = this->goToLevelSelectAlphaAnim.GetInterpolantValue();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Camera::PushWindowCoords();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);

            const Texture* bgTexture = bgRenderer->GetMenuBGTexture();

            float screenWidth = Camera::GetWindowWidth();
            float screenHeight = Camera::GetWindowHeight();
            float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(bgTexture->GetWidth()));
            float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(bgTexture->GetHeight()));

            bgTexture->BindTexture();
            glColor4f(1,1,1,1);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(0, 0);
            glTexCoord2f(2*totalTexU, 0);
            glVertex2f(2*Camera::GetWindowWidth(), 0);
            glTexCoord2f(2*totalTexU, totalTexV);
            glVertex2f(2*Camera::GetWindowWidth(), Camera::GetWindowHeight());
            glTexCoord2f(0, totalTexV);
            glVertex2f(0, Camera::GetWindowHeight());
            glEnd();
            bgTexture->UnbindTexture();

            this->menuFBO->GetFBOTexture()->BindTexture();
            glColor4f(1,1,1,alphaAmt);
            glBegin(GL_QUADS);
            glTexCoord2f(1, 0);
            glVertex2f(Camera::GetWindowWidth(), 0);
            glTexCoord2f(1, 1);
            glVertex2f(Camera::GetWindowWidth(), Camera::GetWindowHeight());
            glTexCoord2f(0, 1);
            glVertex2f(0, Camera::GetWindowHeight());
            glTexCoord2f(0, 0);
            glVertex2f(0, 0);
            glEnd();
            this->menuFBO->GetFBOTexture()->UnbindTexture();

            glColor4f(1, 1, 1, flashAmt);
            glBegin(GL_QUADS);
            glVertex2f(Camera::GetWindowWidth(), 0);
            glVertex2f(Camera::GetWindowWidth(), Camera::GetWindowHeight());
            glVertex2f(0, Camera::GetWindowHeight());
            glVertex2f(0, 0);
            glEnd();

            glPopAttrib();
            glPopMatrix();
            Camera::PopWindowCoords();

            if (animDone) {
                // In arcade mode we go right to the first level of the world that was selected
                WorldSelectItem* selectedWorldItem = this->worldItems[this->selectedItemIdx];
                assert(selectedWorldItem != NULL);
                const GameWorld* selectedWorld = selectedWorldItem->GetWorld();
                assert(selectedWorld != NULL);

                // Load all the initial stuffs for the game - this will queue up the next states that we need to go to
                this->display->GetModel()->StartGameAtWorldAndLevel(selectedWorld->GetWorldIndex(), 0);

                // Place the view into the proper state to play the game	
                this->display->SetCurrentStateAsNextQueuedState();
                return;
            }
        }
        else {

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

            glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            const Texture* bgTexture = bgRenderer->GetMenuBGTexture();

            float screenWidth = Camera::GetWindowWidth();
            float screenHeight = Camera::GetWindowHeight();
            float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(bgTexture->GetWidth()));
            float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(bgTexture->GetHeight()));

            bgTexture->BindTexture();
            glColor4f(1,1,1,1);
            glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
	            glVertex2f(-moveAmt, 0);
                glTexCoord2f(2*totalTexU, 0);
	            glVertex2f(2*Camera::GetWindowWidth() - moveAmt, 0);
                glTexCoord2f(2*totalTexU, totalTexV);
                glVertex2f(2*Camera::GetWindowWidth() - moveAmt, Camera::GetWindowHeight());
	            glTexCoord2f(0, totalTexV);
                glVertex2f(-moveAmt, Camera::GetWindowHeight());
            glEnd();
            bgTexture->UnbindTexture();

            this->menuFBO->GetFBOTexture()->BindTexture();
            glColor4f(1,1,1,alphaAmt);
            glBegin(GL_QUADS);
	            glTexCoord2f(1, 0);
                glVertex2f(Camera::GetWindowWidth() - moveAmt, 0);
	            glTexCoord2f(1, 1);
                glVertex2f(Camera::GetWindowWidth() - moveAmt, Camera::GetWindowHeight());
	            glTexCoord2f(0, 1);
	            glVertex2f(-moveAmt, Camera::GetWindowHeight());
                glTexCoord2f(0, 0);
	            glVertex2f(-moveAmt, 0);
            glEnd();
            this->menuFBO->GetFBOTexture()->UnbindTexture();
            
            glPopAttrib();
            glPopMatrix();
            Camera::PopWindowCoords();
            
            if (animDone) {
                // Go to the level select for the currently selected world
                this->display->SetCurrentState(new SelectLevelMenuState(this->display, 
                    DisplayStateInfo::BuildSelectLevelInfo(this->selectedItemIdx), this->bgSoundLoopID));
                return;
            }
        }
    }

    if (!this->itemActivated) {
        this->postMenuFBObj->BindFBObj();
        this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();
        this->postMenuFBObj->UnbindFBObj();
        this->postMenuFBObj->GetFBOTexture()->RenderTextureToFullscreenQuad();
    }

    if (fadeDone && this->goBackToMainMenu) {
        bgRenderer->DrawBG(camera, this->fadeAnimation.GetInterpolantValue());

        // Go back to the main menu now
        this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::MainMenu, 
            DisplayStateInfo::BuildMainMenuInfo(true), this->display));
        return;
    }

    debug_opengl_state();
}

void SelectWorldMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

    // If there's an animation going on that locks the controls then exit immediately
    if (this->worldUnlockAnim != NULL) {
        if (this->worldUnlockAnim->AreControlsLocked()) {
            return;
        }
    }

    GameSound* sound = this->display->GetSound();
    switch (pressedButton) {
        case GameControl::EscapeButtonAction:
            this->GoBackToMainMenu();
            break;

        case GameControl::LeftButtonAction:
            this->MoveToPrevWorld();
            break;

        case GameControl::RightButtonAction:
            this->MoveToNextWorld();
            break;

        case GameControl::EnterButtonAction: {
            if (!this->itemActivated) {
                WorldSelectItem* selectedItem = this->worldItems[this->selectedItemIdx];
                assert(selectedItem != NULL);
                if (selectedItem->GetIsLocked()) {

                    sound->PlaySound(GameSound::WorldMenuItemLockedEvent, false);

                    // The world is locked... do an animation to shake it around a bit
                    selectedItem->ExecuteLockedAnimation();
                }
                else {
                    static const float ANIMATION_TIME_S = 0.5;
                    
                    if (this->display->IsArcadeModeEnabled()) {
                        
                        GameViewEventManager::Instance()->ActionArcadeWaitingForPlayerState(false);
                        GameViewEventManager::Instance()->ActionArcadePlayerSelectedWorld();
                        sound->PlaySound(GameSound::MenuItemVerifyAndSelectStartGameEvent, false);
                        sound->StopSound(this->bgSoundLoopID, ANIMATION_TIME_S);

                        std::vector<double> timeVals(7);
                        timeVals[0] = 0.0; timeVals[1] = 0.1; timeVals[2] = 0.15; timeVals[3] = 0.2; timeVals[4] = 0.25; timeVals[5] = 0.3; timeVals[6] = 0.35;
                        std::vector<float> flashAlphaVals(timeVals.size());
                        flashAlphaVals[0] = 0.0; flashAlphaVals[1] = 1.0; flashAlphaVals[2] = 0.0;
                        flashAlphaVals[3] = 1.0; flashAlphaVals[4] = 0.0; flashAlphaVals[5] = 1.0; flashAlphaVals[6] = 0.0;

                        this->arcadeFlashAnim.SetLerp(timeVals, flashAlphaVals);
                        this->arcadeFlashAnim.SetRepeat(false);

                        this->goToLevelSelectAlphaAnim.SetLerp(0.1, ANIMATION_TIME_S, 1.0, 0.0);
                    }
                    else {
                        sound->PlaySound(GameSound::WorldMenuItemSelectEvent, false);
                        this->goToLevelSelectAlphaAnim.SetLerp(ANIMATION_TIME_S, 0.0);
                    }

                    this->itemActivated = true;
                    this->goToLevelSelectAlphaAnim.SetRepeat(false);
                    this->goToLevelSelectMoveAnim.SetLerp(0.5, Camera::GetWindowWidth());
                    this->goToLevelSelectMoveAnim.SetRepeat(false);
                }
            }
            break;
        }

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

void SelectWorldMenuState::DrawStarTotal(double dT) {

    float gameTotalLabelTopCornerX = Camera::GetWindowWidth() - this->totalNumGameStarsLabel->GetLastRasterWidth() - HORIZONTAL_TITLE_GAP;
    float gameTotalLabelTopCornerY = this->totalNumGameStarsLabel->GetHeight() + VERTICAL_TITLE_GAP/2;
    this->totalNumGameStarsLabel->SetTopLeftCorner(gameTotalLabelTopCornerX, gameTotalLabelTopCornerY);
    this->totalNumGameStarsLabel->Draw();

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Colour& starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;

    // Star in the game star total
    float diff     = 0.25f * this->totalNumGameStarsLabel->GetHeight();
    float starSize = this->totalNumGameStarsLabel->GetHeight() + 2*diff;
    float centerX  = gameTotalLabelTopCornerX - diff - starSize / 2.0f;
    float centerY  = gameTotalLabelTopCornerY + 1.5f * diff - starSize / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glScalef(starSize, starSize, 1.0f);

    glPushMatrix();
    this->totalGameStarGlowPulseAnim.Tick(dT);
    float pulseScale = this->totalGameStarGlowPulseAnim.GetInterpolantValue();
    glScalef(pulseScale, pulseScale, 1.0f);
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 0.5f);
    this->starGlowTexture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    this->starTexture->BindTexture();
    glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
    GeometryMaker::GetInstance()->DrawQuad();    
    glPopMatrix();

    glPopAttrib();

    this->totalLabel->SetTopLeftCorner(centerX - starSize / 2.0f - diff - this->totalLabel->GetLastRasterWidth(), gameTotalLabelTopCornerY);
    this->totalLabel->Draw();
}

void SelectWorldMenuState::GoBackToMainMenu() {
    GameSound* sound = this->display->GetSound();
    sound->PlaySound(GameSound::MenuItemCancelEvent, false);

    // Fade out background music
    sound->StopSound(this->bgSoundLoopID, 0.5);

    this->fadeAnimation.SetLerp(0.0, 0.5f, 0.0f, 1.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(0.0f);
    this->goBackToMainMenu = true;
}

void SelectWorldMenuState::MoveToNextWorld() {
    this->worldItems[this->selectedItemIdx]->SetIsSelected(false);
    this->selectedItemIdx += 1;
    this->selectedItemIdx %= this->worldItems.size();
    this->worldItems[this->selectedItemIdx]->SetIsSelected(true);

    this->display->GetSound()->PlaySound(GameSound::WorldMenuItemChangedSelectionEvent, false);
}

void SelectWorldMenuState::MoveToPrevWorld() {
    this->worldItems[this->selectedItemIdx]->SetIsSelected(false);
    this->selectedItemIdx -= 1;
    if (this->selectedItemIdx < 0) {
        this->selectedItemIdx = this->worldItems.size()-1;
    }
    this->worldItems[this->selectedItemIdx]->SetIsSelected(true);

    this->display->GetSound()->PlaySound(GameSound::WorldMenuItemChangedSelectionEvent, false);
}

void SelectWorldMenuState::Init(const DisplayStateInfo& info) {
    
    float scalingFactor = this->display->GetTextScalingFactor();

    // Background music...
    if (this->bgSoundLoopID == INVALID_SOUND_ID) {
        this->bgSoundLoopID = this->display->GetSound()->PlaySound(GameSound::WorldMenuBackgroundLoop, true);
    }

    this->padlockTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_PADLOCK, Texture::Trilinear));
    assert(this->padlockTex != NULL);

    this->starTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STAR, Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->starTexture != NULL);

    this->starGlowTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->starGlowTexture != NULL);
    this->starGlowTexture->SetWrapParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->worldSelectTitleLbl = new TextLabel2D(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, 
        GameFontAssetsManager::Huge), SelectWorldMenuState::WORLD_SELECT_TITLE);
    this->worldSelectTitleLbl->SetColour(Colour(0.4f, 0.6f, 0.8f)); // Steel blue
    this->worldSelectTitleLbl->SetDropShadow(Colour(1,1,1), 0.05f);
    
    this->keyEscLabel = new KeyboardHelperLabel(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium, "Press", "Esc", "to Return");
    this->keyEscLabel->SetBeforeAndAfterTextColour(Colour(1,1,1));
    this->keyEscLabel->SetScale(this->display->GetTextScalingFactor());

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

    this->totalLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "Total");
    this->totalLabel->SetColour(Colour(1,1,1));
    this->totalLabel->SetScale(scalingFactor);

    std::stringstream totalNumGameStarsTxt;
    totalNumGameStarsTxt << this->display->GetModel()->GetTotalStarsCollectedInGame();
    this->totalNumGameStarsLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Big), totalNumGameStarsTxt.str());
    this->totalNumGameStarsLabel->SetColour(Colour(1,1,1));
    this->totalNumGameStarsLabel->SetScale(scalingFactor);

    timeVals.clear();
    timeVals.reserve(3);
    timeVals.push_back(0.0); timeVals.push_back(1.0); timeVals.push_back(2.0);
    std::vector<float> pulseVals;
    pulseVals.push_back(1.0f); pulseVals.push_back(1.25f); pulseVals.push_back(1.0f);
    this->totalGameStarGlowPulseAnim.SetLerp(timeVals, pulseVals);
    this->totalGameStarGlowPulseAnim.SetRepeat(true);

    // Build a framebuffer object for the menu
	this->menuFBO = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
    this->postMenuFBObj = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);

    // Setup the world items for the GUI menu
    GameModel* gameModel = this->display->GetModel();
    const std::vector<GameWorld*>& gameWorlds = gameModel->GetGameWorlds();
    assert(!gameWorlds.empty());

    int furthestWorldIdx = -1;
    int furthestLevelIdx = -1;
    gameModel->GetFurthestProgressWorldAndLevel(furthestWorldIdx, furthestLevelIdx);
    
    // Probably because I'm still developing the game and I added a world...
    // This should never happen in production!!
    if (furthestWorldIdx == -1) {
        gameModel->OverrideGetFurthestProgress(furthestWorldIdx, furthestLevelIdx);
    }

    // Special case: The furthest world hasn't been unlocked yet so we make sure that it's
    // treated as a locked world
    if (furthestWorldIdx == info.GetWorldUnlockIndex()) {
        furthestWorldIdx--;
    }

    this->worldItems.reserve(gameWorlds.size());
    for (int i = 0; i < static_cast<int>(gameWorlds.size()); i++) {

        const GameWorld* currGameWorld = gameWorlds[i];
        bool isLocked = (furthestWorldIdx < i);

        WorldSelectItem* menuItem = new WorldSelectItem(this, currGameWorld, isLocked);
        this->worldItems.push_back(menuItem);
    }

    timeVals.clear();
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    std::vector<float> scaleVals;
    scaleVals.push_back(0.0f);
    scaleVals.push_back(2 * SELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP);
    this->selectionBorderAddYellowAnim.SetLerp(timeVals, scaleVals);
    this->selectionBorderAddYellowAnim.SetRepeat(true);

    alphaVals.clear();
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.0f);
    this->selectionAlphaYellowAnim.SetLerp(timeVals, alphaVals);
    this->selectionAlphaYellowAnim.SetRepeat(true);

    // Setup any animations and selections based on the info that this object was initialized with
    if (info.GetWorldUnlockIndex() < 0) {

        // No special animation is necessary
        if (info.GetWorldSelectionIndex() < 0) {
            this->selectedItemIdx = furthestWorldIdx;
        }
        else {
            this->selectedItemIdx = info.GetWorldSelectionIndex();
        }
    }
    else {
        // A world is being unlocked -- we'll need to select the previous world and unlock
        // the next world...
        this->selectedItemIdx = info.GetWorldUnlockIndex() - 1;
        
        // Setup the animation object for unlocking a world
        this->worldUnlockAnim = new WorldUnlockAnimationTracker(this, this->worldItems[this->selectedItemIdx+1]);
    }
    this->worldItems[this->selectedItemIdx]->SetIsSelected(true);

    if (this->display->IsArcadeModeEnabled()) {
        GameViewEventManager::Instance()->ActionArcadeWaitingForPlayerState(true);
    }
}

// WorldUnlockAnimationTracker Functions **********************************************************

SelectWorldMenuState::WorldUnlockAnimationTracker::WorldUnlockAnimationTracker(SelectWorldMenuState* state,
                                                                               WorldSelectItem* worldItem) : 
worldItem(worldItem), state(state), timedEnergySuckEmitter(NULL), energySuckEmitter(NULL), bigExplosionEmitter(NULL), bigExplosionOnoEmitter(NULL),
debrisEmitter(NULL), fireSmokeEmitter1(NULL), fireSmokeEmitter2(NULL), countdownMoveToLockedWorld(1.0),
countdownWaitToShake(0.75), countdownWaitToEnergySuck(0.75), sparkleTex(NULL), debrisTex(NULL), worldUnlockSoundID(INVALID_SOUND_ID),
lensFlareTex(NULL), hugeExplosionTex(NULL), sphereNormalsTex(NULL), particleFader(1, 0), particleHalfFader(1, 0.5f), 
particleMediumGrowth(1.0f, 2.0f), particleMediumShrink(1.0f, 0.25f), particleSmallGrowth(1.0f, 1.5f), unlockAnimExecuted(false),
particleSuperGrowth(1.0f, 10.0f), particleFireFastColourFader(ColourRGBA(1.0f, 1.0f, 0.1f, 0.8f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE) {

    assert(worldItem != NULL);

    this->debrisTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->debrisTex != NULL);
    this->sparkleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->sparkleTex != NULL);
    this->lensFlareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LENSFLARE, Texture::Trilinear));
    assert(this->lensFlareTex != NULL);
    this->hugeExplosionTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HUGE_EXPLOSION, Texture::Trilinear));
    assert(this->hugeExplosionTex != NULL);
    this->sphereNormalsTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
    assert(this->sphereNormalsTex != NULL);

    // Initialize the smoke textures...
    if (this->smokeTextures.empty()) {
        this->smokeTextures.reserve(6);
        Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);	
    }

    this->normalTexRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
    this->normalTexRefractEffect.SetWarpAmountParam(27.0f);
    this->normalTexRefractEffect.SetIndexOfRefraction(1.2f);
    this->normalTexRefractEffect.SetNormalTexture(this->sphereNormalsTex);

    std::vector<Texture2D*> energySuckTextures;
    energySuckTextures.reserve(2);
    energySuckTextures.push_back(this->sparkleTex);
    energySuckTextures.push_back(this->lensFlareTex);

    const float MAX_DEVIATION_RADIUS = 0.3f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE;

    this->energySuckEmitter = new ESPPointEmitter();
    this->energySuckEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->energySuckEmitter->SetNumParticleLives(1);
    this->energySuckEmitter->SetInitialSpd(ESPInterval(500.0f, 1500.0f));
    this->energySuckEmitter->SetParticleLife(ESPInterval(0.5f, 1.75f));
    this->energySuckEmitter->SetParticleSize(ESPInterval(25.0f, 250.0f));
    this->energySuckEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, MAX_DEVIATION_RADIUS),
        ESPInterval(0.0f, MAX_DEVIATION_RADIUS), ESPInterval(0.0f));
    this->energySuckEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->energySuckEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->energySuckEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->energySuckEmitter->SetToggleEmitOnPlane(true);
    this->energySuckEmitter->SetEmitAngleInDegrees(180);
    this->energySuckEmitter->SetIsReversed(true);
    this->energySuckEmitter->SetParticleColour(ESPInterval(0.75f, 1.0f), ESPInterval(0.85f, 1.0f), ESPInterval(1.0f), ESPInterval(0.9f, 1.0f));
    this->energySuckEmitter->AddEffector(&this->particleHalfFader);
    this->energySuckEmitter->AddEffector(&this->particleMediumShrink);
    this->energySuckEmitter->SetRandomTextureParticles(200, energySuckTextures);


    this->timedEnergySuckEmitter = new ESPPointEmitter();
    this->timedEnergySuckEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->timedEnergySuckEmitter->SetNumParticleLives(1);
    this->timedEnergySuckEmitter->SetInitialSpd(ESPInterval(500.0f, 1500.0f));
    this->timedEnergySuckEmitter->SetParticleLife(ESPInterval(1.75f));
    this->timedEnergySuckEmitter->SetParticleSize(ESPInterval(25.0f, 250.0f));
    this->timedEnergySuckEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, MAX_DEVIATION_RADIUS),
        ESPInterval(0.0f, MAX_DEVIATION_RADIUS), ESPInterval(0.0f));
    this->timedEnergySuckEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->timedEnergySuckEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->timedEnergySuckEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->timedEnergySuckEmitter->SetToggleEmitOnPlane(true);
    this->timedEnergySuckEmitter->SetEmitAngleInDegrees(180);
    this->timedEnergySuckEmitter->SetIsReversed(true);
    this->timedEnergySuckEmitter->SetParticleColour(ESPInterval(0.75f, 1.0f), ESPInterval(0.85f, 1.0f), ESPInterval(1.0f), ESPInterval(0.9f, 1.0f));
    this->timedEnergySuckEmitter->AddEffector(&this->particleHalfFader);
    this->timedEnergySuckEmitter->AddEffector(&this->particleMediumShrink);
    this->timedEnergySuckEmitter->SetRandomTextureParticles(30, energySuckTextures);

    static const float EXPLOSION_LIFE = 2.0;

    this->bigExplosionEmitter = new ESPPointEmitter();
    this->bigExplosionEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->bigExplosionEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
    this->bigExplosionEmitter->SetParticleLife(ESPInterval(EXPLOSION_LIFE));
    this->bigExplosionEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    this->bigExplosionEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->bigExplosionEmitter->SetEmitPosition(Point3D(0,0,0));
    this->bigExplosionEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->bigExplosionEmitter->SetToggleEmitOnPlane(true);
    this->bigExplosionEmitter->SetParticleRotation(ESPInterval(-10.0f, 10.0f));
    this->bigExplosionEmitter->SetParticleSize(ESPInterval(2.5f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE),
        ESPInterval(1.25f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE));
    this->bigExplosionEmitter->AddEffector(&this->particleFader);
    this->bigExplosionEmitter->AddEffector(&this->particleMediumGrowth);
    this->bigExplosionEmitter->SetParticles(1, this->hugeExplosionTex);

    this->bigExplosionOnoEmitter = new ESPPointEmitter();
    this->bigExplosionOnoEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->bigExplosionOnoEmitter->SetInitialSpd(ESPInterval(0.0f));
    this->bigExplosionOnoEmitter->SetParticleLife(EXPLOSION_LIFE);
    this->bigExplosionOnoEmitter->SetParticleSize(ESPInterval(1.0f));
    this->bigExplosionOnoEmitter->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
    this->bigExplosionOnoEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
    this->bigExplosionOnoEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->bigExplosionOnoEmitter->SetEmitPosition(Point3D(0,0,0));
    this->bigExplosionOnoEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->bigExplosionOnoEmitter->SetToggleEmitOnPlane(true);
    this->bigExplosionOnoEmitter->AddEffector(&this->particleFader);
    this->bigExplosionOnoEmitter->AddEffector(&this->particleSmallGrowth);
    TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "");
    bangTextLabel.SetColour(Colour(1, 1, 1));
    bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
    this->bigExplosionOnoEmitter->SetParticles(1, bangTextLabel, Onomatoplex::EXPLOSION, Onomatoplex::UBER, true);

    this->shockwaveEffect = new ESPPointEmitter();
    this->shockwaveEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->shockwaveEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
    this->shockwaveEffect->SetParticleLife(ESPInterval(EXPLOSION_LIFE));
    this->shockwaveEffect->SetParticleSize(ESPInterval(1.25f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE));
    this->shockwaveEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    this->shockwaveEffect->SetParticleAlignment(ESP::NoAlignment);
    this->shockwaveEffect->SetEmitPosition(Point3D(0,0,0));
    this->shockwaveEffect->SetEmitDirection(Vector3D(0,1,0));
    this->shockwaveEffect->SetToggleEmitOnPlane(true);
    this->shockwaveEffect->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->shockwaveEffect->AddEffector(&this->particleFader);
    this->shockwaveEffect->AddEffector(&this->particleSuperGrowth);
    this->shockwaveEffect->SetParticles(1, &this->normalTexRefractEffect);

    ESPInterval smokeSize(0.1f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE, 
        0.75f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE);

    this->fireSmokeEmitter1 = new ESPPointEmitter();
    this->fireSmokeEmitter1->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->fireSmokeEmitter1->SetNumParticleLives(1);
    this->fireSmokeEmitter1->SetInitialSpd(ESPInterval(100.0f, 1000.0f));
    this->fireSmokeEmitter1->SetParticleLife(ESPInterval(1.25f, 2.75f));
    this->fireSmokeEmitter1->SetParticleSize(smokeSize);
    this->fireSmokeEmitter1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->fireSmokeEmitter1->SetParticleAlignment(ESP::NoAlignment);
    this->fireSmokeEmitter1->SetEmitPosition(Point3D(0,0,0));
    this->fireSmokeEmitter1->SetEmitDirection(Vector3D(0,1,0));
    this->fireSmokeEmitter1->SetToggleEmitOnPlane(true);
    this->fireSmokeEmitter1->SetEmitAngleInDegrees(180);
    this->fireSmokeEmitter1->AddEffector(&this->particleFireFastColourFader);
    this->fireSmokeEmitter1->AddEffector(&this->particleMediumGrowth);
    this->fireSmokeEmitter1->AddEffector(&this->smokeRotatorCW);
    this->fireSmokeEmitter1->SetRandomTextureParticles(50, this->smokeTextures);

    this->fireSmokeEmitter2 = new ESPPointEmitter();
    this->fireSmokeEmitter2->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->fireSmokeEmitter2->SetNumParticleLives(1);
    this->fireSmokeEmitter2->SetInitialSpd(ESPInterval(100.0f, 1000.0f));
    this->fireSmokeEmitter2->SetParticleLife(ESPInterval(1.25f, 2.75f));
    this->fireSmokeEmitter2->SetParticleSize(smokeSize);
    this->fireSmokeEmitter2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->fireSmokeEmitter2->SetParticleAlignment(ESP::NoAlignment);
    this->fireSmokeEmitter2->SetEmitPosition(Point3D(0,0,0));
    this->fireSmokeEmitter2->SetEmitDirection(Vector3D(0,1,0));
    this->fireSmokeEmitter2->SetToggleEmitOnPlane(true);
    this->fireSmokeEmitter2->SetEmitAngleInDegrees(180);
    this->fireSmokeEmitter2->AddEffector(&this->particleFireFastColourFader);
    this->fireSmokeEmitter2->AddEffector(&this->particleMediumGrowth);
    this->fireSmokeEmitter2->AddEffector(&this->smokeRotatorCCW);
    this->fireSmokeEmitter2->SetRandomTextureParticles(50, this->smokeTextures);

    this->debrisEmitter = new ESPPointEmitter();
    this->debrisEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->debrisEmitter->SetNumParticleLives(1);
    this->debrisEmitter->SetInitialSpd(ESPInterval(200.0f, 1200.0f));
    this->debrisEmitter->SetParticleLife(ESPInterval(1.25f, 2.75f));
    this->debrisEmitter->SetParticleSize(0.33f * smokeSize);
    this->debrisEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->debrisEmitter->SetParticleAlignment(ESP::NoAlignment);
    this->debrisEmitter->SetEmitPosition(Point3D(0,0,0));
    this->debrisEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->debrisEmitter->SetToggleEmitOnPlane(true);
    this->debrisEmitter->SetEmitAngleInDegrees(180);
    this->debrisEmitter->AddEffector(&this->particleFireFastColourFader);
    this->debrisEmitter->AddEffector(&this->particleMediumGrowth);
    this->debrisEmitter->SetParticles(40, this->debrisTex);

    // Build the lock shake animation
    static const float SHAKE_AMT = 0.125f * WorldSelectItem::PADLOCK_SCALE * SELECTED_MENU_ITEM_SIZE;
    static const int NUM_SHAKES = 31;
    std::vector<double> timeVals;
    timeVals.reserve(NUM_SHAKES);
    std::vector<Vector2D> shakeVals;
    shakeVals.reserve(NUM_SHAKES);

    timeVals.push_back(0.0);
    shakeVals.push_back(Vector2D(0.0f, 0.0f));
    for (int i = 1; i < NUM_SHAKES; i++) {
        timeVals.push_back(timeVals.back() + 0.005 + 0.01f * Randomizer::GetInstance()->RandomNumZeroToOne());
        shakeVals.push_back(Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_AMT, Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_AMT));
    }

    this->lockShakeAnim.SetLerp(timeVals, shakeVals);
    this->lockShakeAnim.SetInterpolantValue(Vector2D(0.0f, 0.0f));
    this->lockShakeAnim.SetRepeat(true);
}

SelectWorldMenuState::WorldUnlockAnimationTracker::~WorldUnlockAnimationTracker() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->debrisTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lensFlareTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->hugeExplosionTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sphereNormalsTex);
    assert(success);

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
        iter != this->smokeTextures.end(); ++iter) {

        success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        assert(success);	
    }
    this->smokeTextures.clear();

    UNUSED_VARIABLE(success);

    delete this->timedEnergySuckEmitter;
    this->timedEnergySuckEmitter = NULL;
    delete this->energySuckEmitter;
    this->energySuckEmitter = NULL;
    delete this->bigExplosionEmitter;
    this->bigExplosionEmitter = NULL;
    delete this->bigExplosionOnoEmitter;
    this->bigExplosionOnoEmitter = NULL;
    delete this->shockwaveEffect;
    this->shockwaveEffect = NULL;
    delete this->fireSmokeEmitter1;
    this->fireSmokeEmitter1 = NULL;
    delete this->fireSmokeEmitter2;
    this->fireSmokeEmitter2 = NULL;
    delete this->debrisEmitter;
    this->debrisEmitter = NULL;

    this->state->display->GetSound()->StopSound(this->worldUnlockSoundID);
    this->worldUnlockSoundID = INVALID_SOUND_ID;
}

bool SelectWorldMenuState::WorldUnlockAnimationTracker::AreControlsLocked() const {
    return this->worldItem->GetIsLocked();
}

void SelectWorldMenuState::WorldUnlockAnimationTracker::Draw(const Camera& camera, double dT, const Texture2D& bgTexture) {

    // Check for when we automatically move the selection to the previously locked world
    if (this->countdownMoveToLockedWorld > 0.0) {
        this->countdownMoveToLockedWorld -= dT;
        if (this->countdownMoveToLockedWorld <= 0.0) {
            // Move to the next world
            this->state->MoveToNextWorld();
        }
        return;
    }

    if (this->countdownWaitToShake > 0.0) {
        this->countdownWaitToShake -= dT;
        if (this->countdownWaitToShake <= 0.0 && this->worldUnlockSoundID == INVALID_SOUND_ID) {
            // Play the sound for the world unlock
            this->worldUnlockSoundID = this->state->display->GetSound()->PlaySound(GameSound::WorldUnlockEvent, false, false);
        }
        return;
    }

    if (this->countdownWaitToEnergySuck > 0.0) {
        this->countdownWaitToEnergySuck -= dT;
        this->lockShakeAnim.Tick(dT);
        this->worldItem->SetLockOffset(this->lockShakeAnim.GetInterpolantValue());
        return;
    }

    const float currCenterX = Camera::GetWindowWidth()  / 2.0f;
    const float currCenterY = Camera::GetWindowHeight() / 2.0f;

    // We've selected the world that will be 'unlocked' / animated, perform the animations
    glPushMatrix();
    glTranslatef(currCenterX, currCenterY, 0);

    // Energy suck (flares getting sucked into the lock for the level)
    this->energySuckEmitter->Tick(dT);
    this->energySuckEmitter->Draw(camera);
    this->timedEnergySuckEmitter->Tick(dT);
    this->timedEnergySuckEmitter->Draw(camera);
    if (!this->timedEnergySuckEmitter->IsDead()) {

        this->lockShakeAnim.Tick(dT);
        this->worldItem->SetLockOffset(this->lockShakeAnim.GetInterpolantValue());
        glPopMatrix();
        return;
    }
    
    this->worldItem->SetLockOffset(Vector2D(0,0));
    this->normalTexRefractEffect.SetFBOTexture(&bgTexture);
    this->shockwaveEffect->Tick(dT);
    this->shockwaveEffect->Draw(camera);
    this->fireSmokeEmitter1->Tick(dT);
    this->fireSmokeEmitter1->Draw(camera);
    this->fireSmokeEmitter2->Tick(dT);
    this->fireSmokeEmitter2->Draw(camera);
    this->debrisEmitter->Tick(dT);
    this->debrisEmitter->Draw(camera);
    this->bigExplosionEmitter->Tick(dT);
    this->bigExplosionEmitter->Draw(camera);
    this->bigExplosionOnoEmitter->Tick(dT);
    this->bigExplosionOnoEmitter->Draw(camera);

    if (!this->unlockAnimExecuted) {
        this->worldItem->ExecuteUnlockAnimation();
        this->unlockAnimExecuted = true;
    }

    glPopMatrix();
}

// WorldSelectItem Functions **********************************************************************

#define UNSELECTED_COLOUR Colour(0.8f, 0.8f, 0.8f)
#define SELECTED_COLOUR Colour(1,1,0)

const float SelectWorldMenuState::WorldSelectItem::PADLOCK_SCALE = 0.6f;

SelectWorldMenuState::WorldSelectItem::WorldSelectItem(SelectWorldMenuState* state,
                                                       const GameWorld* world, bool isLocked) :

state(state), gameWorld(world), image(NULL), unselectedLabel(NULL), 
unselectedStarTotalLabel(NULL), selectedStarTotalLabel(NULL), unlockAnimActive(false),
selectedLabel(NULL), isSelected(false), isLocked(isLocked), lockOffset(0,0) {

    assert(world != NULL);
    
    std::string labelTextStr;
    Colour labelUnselectedColour = UNSELECTED_COLOUR;
    Colour labelSelectedColour = SELECTED_COLOUR;

    if (this->isLocked) {
        labelTextStr = std::string("???");
        labelUnselectedColour = Colour(0.5f, 0.5f, 0.5f);
        labelSelectedColour   = Colour(0.5f, 0.5f, 0.5f);
        this->greyscaleEffect.SetGreyscaleFactor(1.0f);
    }
    else {
        labelTextStr = world->GetName();
        this->greyscaleEffect.SetGreyscaleFactor(0.0f);
    }

    float unselectedLabelWidth = SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE + 0.75f *
        SelectWorldMenuState::UNSELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP;
    float selectedLabelWidth   = SelectWorldMenuState::SELECTED_MENU_ITEM_SIZE + 0.75f *
        SelectWorldMenuState::SELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP;

    this->unselectedLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
        unselectedLabelWidth, labelTextStr);
    this->unselectedLabel->SetColour(labelUnselectedColour);
    this->unselectedLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    
    this->selectedLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        selectedLabelWidth, labelTextStr);
    this->selectedLabel->SetDropShadow(Colour(0,0,0), 0.08f);
    this->selectedLabel->SetColour(labelSelectedColour);
    this->selectedLabel->SetScale(1.1f);
    this->selectedLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);

    this->sizeAnim.ClearLerp();
    this->sizeAnim.SetInterpolantValue(SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE);
    this->sizeAnim.SetRepeat(false);

    // Create a label for the total number of stars collected in the world represented by this item
    int totalNumStarsCollected = world->GetNumStarsCollectedInWorld();
    int totalNumStars = world->GetTotalAchievableStarsInWorld();

    std::stringstream totalNumStarsTxt;
    totalNumStarsTxt << totalNumStarsCollected << "/" << totalNumStars;
    
    this->unselectedStarTotalLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Small), totalNumStarsTxt.str());
    this->unselectedStarTotalLabel->SetColour(Colour(0.75f, 0.75f, 0.75f));
    this->unselectedStarTotalLabel->SetScale(0.85f);

    this->selectedStarTotalLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, 
        GameFontAssetsManager::Medium), totalNumStarsTxt.str());
    this->selectedStarTotalLabel->SetColour(Colour(1,1,1));
    this->selectedStarTotalLabel->SetScale(0.85f);

    this->image = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        world->GetImageFilepath(), Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->image != NULL);

    this->lockedAnim.ClearLerp();
    this->lockedAnim.SetInterpolantValue(0);
}

SelectWorldMenuState::WorldSelectItem::~WorldSelectItem() {

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->image);
    UNUSED_VARIABLE(success);
    assert(success);
    
    delete this->unselectedLabel;
    this->unselectedLabel = NULL;
    delete this->selectedLabel;
    this->selectedLabel = NULL;

    delete this->unselectedStarTotalLabel;
    this->unselectedStarTotalLabel = NULL;
    delete this->selectedStarTotalLabel;
    this->selectedStarTotalLabel = NULL;
}

void SelectWorldMenuState::WorldSelectItem::SetIsSelected(bool isSelected) {
    this->isSelected = isSelected;
    
    if (this->isSelected) {
        this->sizeAnim.SetLerp(0.35, SelectWorldMenuState::SELECTED_MENU_ITEM_SIZE);
    }
    else {
        this->sizeAnim.SetLerp(0.35, SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE);
    }
    this->sizeAnim.SetRepeat(false);
}

void SelectWorldMenuState::WorldSelectItem::Draw(const Camera& camera, double dT, int selectedWorldNum) {
    
    float lockAlpha = 1.0f;
    if (this->unlockAnimActive) {
        bool isGreyscaleAnimFinished = this->greyscaleFactorAnim.Tick(dT);
        this->greyscaleEffect.SetGreyscaleFactor(this->greyscaleFactorAnim.GetInterpolantValue());

        bool isLockFadeAnimFinished = this->lockFadeAnim.Tick(dT);
        lockAlpha = this->lockFadeAnim.GetInterpolantValue();
        if (isLockFadeAnimFinished) {
            this->Unlock();

            if (isGreyscaleAnimFinished) {
                this->unlockAnimActive = false;
            }
        }
    }
    
    this->sizeAnim.Tick(dT);
    const float currSize = this->sizeAnim.GetInterpolantValue();
    const float halfCurrSize = currSize / 2.0f;

    TextLabel2DFixedWidth* currLabel = NULL;
    TextLabel2D* currStarLabel = NULL;
    if (this->isSelected) {
        currLabel = this->selectedLabel;
        currStarLabel = this->selectedStarTotalLabel;
    }
    else {
        currLabel = this->unselectedLabel;
        currStarLabel = this->unselectedStarTotalLabel;
    }

    const float currCenterX = this->GetXPosition(camera, selectedWorldNum);
    const float currCenterY = Camera::GetWindowHeight() / 2.0f;

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1,1,1,1);

    glPushMatrix();
    glTranslatef(currCenterX, currCenterY, 0);

    glPushMatrix();
    glScalef(currSize, currSize, 1);
    
    // Draw the world image
    Colour outlineColour(0,0,0);

    if (this->isLocked) {
        outlineColour = Colour(0.2f, 0.2f, 0.2f);
    }
    this->greyscaleEffect.SetColourTexture(this->image);
    this->greyscaleEffect.Draw(camera, GeometryMaker::GetInstance()->GetQuadDL());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        this->lockedAnim.Tick(dT);
        float currShakeX = this->lockedAnim.GetInterpolantValue();

        // When locked, draw the padlock over the world image
        glColor4f(1, 1, 1, lockAlpha);
        glPushMatrix();
        int fractSize = static_cast<int>(PADLOCK_SCALE * currSize);
        glTranslatef(currShakeX + this->lockOffset[0], this->lockOffset[1], 0.0f);
        glScalef(fractSize, fractSize, 1);
        this->state->padlockTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        this->state->padlockTex->UnbindTexture();
        glPopMatrix();
    }

    glPopMatrix();

    static const float TITLE_VERTICAL_GAP = 10;
    float labelX = currCenterX - (currLabel->GetWidth() / 2.0f);
    float labelY = currCenterY - halfCurrSize - TITLE_VERTICAL_GAP;
    
    currLabel->SetTopLeftCorner(labelX, labelY);
    currLabel->Draw();

    if (!this->isLocked) {
        const Colour& starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;

        static const float STAR_VERTICAL_GAP = 1.75f * TITLE_VERTICAL_GAP;
        static const float STAR_ICON_HORIZONTAL_GAP = 2;
        
        float starSize   = 1.5f * currStarLabel->GetHeight();
        float halfStarSize = starSize / 2.0f;
        float starLabelX = currCenterX - (currStarLabel->GetLastRasterWidth() + halfStarSize + STAR_ICON_HORIZONTAL_GAP) / 2.0f;
        float starLabelY = currCenterY - halfCurrSize - TITLE_VERTICAL_GAP - currLabel->GetHeight() - STAR_VERTICAL_GAP;
        
        this->state->starTexture->BindTexture();
        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
        glPushMatrix();
        glTranslatef(starLabelX, starLabelY - starSize/3.0f, 0);
        glScalef(starSize, starSize, 1.0f); 
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
        this->state->starTexture->UnbindTexture();

        currStarLabel->SetTopLeftCorner(starLabelX + halfStarSize + STAR_ICON_HORIZONTAL_GAP, starLabelY);
        currStarLabel->Draw();
    }

    glPopAttrib();
}

void SelectWorldMenuState::WorldSelectItem::DrawSelectionBorder(const Camera& camera, double dT, 
                                                                float orangeAlpha, float yellowAlpha, 
                                                                float sizeAmt) {
	UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(dT);

    const float currSize = this->sizeAnim.GetInterpolantValue();
    const float currCenterX = this->GetXPosition(camera, this->GetWorld()->GetWorldNumber());
    const float currCenterY = Camera::GetWindowHeight() / 2.0f;

    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(currCenterX, currCenterY, 0);

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

void SelectWorldMenuState::WorldSelectItem::ExecuteLockedAnimation() {
    
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
    float maxMove = 0.25f * PADLOCK_SCALE * this->sizeAnim.GetInterpolantValue();
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

void SelectWorldMenuState::WorldSelectItem::ExecuteUnlockAnimation() {
    this->unlockAnimActive = true;

    this->lockFadeAnim.SetLerp(0.0, 0.5, 1.0f, 0.0f);
    this->lockFadeAnim.SetInterpolantValue(1.0f);
    this->lockFadeAnim.SetRepeat(false);

    this->greyscaleFactorAnim.SetLerp(2.0, 3.5, 1.0f, 0.0f);
    this->greyscaleFactorAnim.SetInterpolantValue(1.0f);
    this->greyscaleFactorAnim.SetRepeat(false);
}

void SelectWorldMenuState::WorldSelectItem::SetLockOffset(const Vector2D& offset) {
    this->lockOffset = offset;
}

void SelectWorldMenuState::WorldSelectItem::Unlock() {
    this->isLocked = false;
    this->unselectedLabel->SetText(this->gameWorld->GetName());
    this->selectedLabel->SetText(this->gameWorld->GetName());
    this->unselectedLabel->SetColour(UNSELECTED_COLOUR);
    this->selectedLabel->SetColour(SELECTED_COLOUR);
}

float SelectWorldMenuState::WorldSelectItem::GetXPosition(const Camera& camera, int selectedWorldNum) const {
    UNUSED_PARAMETER(camera);

    int numWorldsToSelected = this->gameWorld->GetWorldNumber() - selectedWorldNum;
    const float windowCenterX = Camera::GetWindowWidth() / 2.0f;
    
    if (numWorldsToSelected == 0) {
        return windowCenterX;
    }

    float unsignedXDist = SelectWorldMenuState::SELECTED_MENU_ITEM_SIZE / 2.0f +
        SelectWorldMenuState::SELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP +
        SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE / 2.0f +
        (abs(numWorldsToSelected)-1) * (1.5f * SelectWorldMenuState::UNSELECTED_MENU_ITEM_SIZE + 
        SelectWorldMenuState::UNSELECTED_TO_UNSELECTED_MENU_ITEM_HORIZ_GAP);

    return windowCenterX + NumberFuncs::SignOf(numWorldsToSelected) * unsignedXDist;
}