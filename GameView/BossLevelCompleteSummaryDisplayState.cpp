/**
 * BossLevelCompleteSummaryDisplayState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "BossLevelCompleteSummaryDisplayState.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "MenuBackgroundRenderer.h"

#include "../GameModel/Onomatoplex.h"
#include "../GameModel/GameProgressIO.h"

#include "../GameSound/GameSound.h"

#include "../ResourceManager.h"

const double BossLevelCompleteSummaryDisplayState::FADE_TIME              = 1.5;
const float BossLevelCompleteSummaryDisplayState::FOOTER_VERTICAL_PADDING = 20.0f;

const float BossLevelCompleteSummaryDisplayState::VICTORY_TO_WORLD_VERTICAL_PADDING  = 200.0f;
const float BossLevelCompleteSummaryDisplayState::WORLD_TO_COMPLETE_VERTICAL_PADDING = 10.0f;

const float BossLevelCompleteSummaryDisplayState::VICTORY_LABEL_VERTICAL_SPACING = 20.0f;

const float BossLevelCompleteSummaryDisplayState::LEFT_RIGHT_WORLD_LABEL_SPACING = 20.0f;

const float BossLevelCompleteSummaryDisplayState::COMPLETE_TO_UNLOCKED_VERTICAL_PADDING = 20.0f;

const float BossLevelCompleteSummaryDisplayState::MAX_COMPLETE_TEXT_SCALE = 1.3f;

BossLevelCompleteSummaryDisplayState::BossLevelCompleteSummaryDisplayState(GameDisplay* display) :
DisplayState(display), allAnimationIsDone(false), waitingForKeyPress(false), leavingState(false),
unlockedLabel(NULL), worldCompleteLabel(NULL),
spinGlowTex(NULL), bgMusicSoundID(INVALID_SOUND_ID), victoryMessageSoundID(INVALID_SOUND_ID),
pressAnyKeyLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
                 GameFontAssetsManager::Medium), "- Press Any Key to Continue -"),
victoryLabel1(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big),
             "Sweet " + Onomatoplex::Generator::GetInstance()->GenerateVictoryDescriptor()),
victoryLabel2(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "VICTORY!!!")
{ 

    GameModel* gameModel = this->display->GetModel();
    assert(gameModel != NULL);
    const GameWorld* completedWorld = gameModel->GetCurrentWorld();
    assert(completedWorld != NULL);
    int completedWorldIdx = completedWorld->GetWorldIndex();

    this->display->GetCamera().ClearCameraShake();
    // Pause all game play elements in the game model
	gameModel->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

    GameSound* sound = this->display->GetSound();
    this->bgMusicSoundID = sound->PlaySound(GameSound::WorldCompleteBackgroundLoop, true, false);

    this->spinGlowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->spinGlowTex != NULL);

    this->flareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->flareTex != NULL);

    this->bangStarTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture::Trilinear));
    assert(this->bangStarTex != NULL);

    // Setup all the labels...
    this->victoryLabel1.SetColour(Colour(1,1,1));
    this->victoryLabel1.SetDropShadow(Colour(0,0,0), 0.075f);
    this->victoryLabel1.SetScale(2.0f);
    this->victoryLabel2.SetColour(Colour(1,1,1));
    this->victoryLabel2.SetDropShadow(Colour(0,0,0), 0.06f);
    this->victoryLabel2.SetScale(2.0f);

    // Check to see if the next movement has already been unlocked, if not then we tell the player about it...
    // Special case: the player just completed the very last world... in this case there's nothing left to unlock
    // so we don't display that text
    if (gameModel->GetLastWorldIndex() != completedWorldIdx) {
        
        // Get the next world/movement...
        const GameWorld* nextWorld = gameModel->GetWorldByIndex(completedWorldIdx + 1);
        assert(nextWorld != NULL);

        if (!nextWorld->GetHasBeenUnlocked()) {
            this->unlockedLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), nextWorld->GetName() + " movement UNLOCKED.");
            this->unlockedLabel->SetColour(Colour(1,1,1));
            this->unlockedLabel->SetDropShadow(Colour(0,0,0), 0.075f);

            this->worldCompleteLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), completedWorld->GetName() + " movement COMPLETE.");
            this->worldCompleteLabel->SetColour(Colour(1,1,1));
            this->worldCompleteLabel->SetDropShadow(Colour(0,0,0), 0.075f);
        }
    }

	// Setup the label for the press any key text...
	this->pressAnyKeyLabel.SetDropShadow(Colour(0, 0, 0), this->display->GetTextScalingFactor() * 0.1f);
	this->pressAnyKeyLabel.SetScale(this->display->GetTextScalingFactor());

    // Setup all the animations...

	// Setup the fade animations
    this->fadeInAnimation.SetInterpolantValue(1.0f);
    this->fadeInAnimation.SetLerp(0.0, FADE_TIME, 1.0f, 0.0f);
    this->fadeInAnimation.SetRepeat(false);

    this->fadeOutAnimation.SetInterpolantValue(0.0f);
    this->fadeOutAnimation.SetLerp(FADE_TIME, 1.0f);
    this->fadeOutAnimation.SetRepeat(false);

    // The victory label "Sweet... victory" comes down from the top of the screen and bounces a bit
    float centerBlockTopYCoord = this->GetCenterTextBlockTopYCoord(Camera::GetWindowHeight());

    static const double VICTORY_LABEL_ANIM_TIME = 0.70;
    static const double FINAL_VICTORY_LABEL_ANIM_TIME = FADE_TIME + VICTORY_LABEL_ANIM_TIME;

    std::vector<double> timeVals;
    timeVals.reserve(4);
    timeVals.push_back(FADE_TIME);
    timeVals.push_back(FADE_TIME + 0.5);
    timeVals.push_back(FADE_TIME + 0.55);
    timeVals.push_back(FINAL_VICTORY_LABEL_ANIM_TIME);
    
    std::vector<float> moveVals;
    moveVals.reserve(timeVals.size());
    moveVals.push_back(Camera::GetWindowHeight() + 1.25f * this->GetVictoryLabelBlockHeight());
    moveVals.push_back(centerBlockTopYCoord);
    moveVals.push_back(centerBlockTopYCoord + 0.25f * this->GetVictoryLabelBlockHeight());
    moveVals.push_back(centerBlockTopYCoord);

    this->victoryLabelMoveAnim.SetLerp(timeVals, moveVals);
    this->victoryLabelMoveAnim.SetRepeat(false);
    this->victoryLabelMoveAnim.SetInterpolantValue(moveVals.front());

    timeVals.clear();
    moveVals.clear();

    static const double LABELS_ALPHA_ANIM_TIME = 1.0;
    static const double FINAL_WORLD_LABELS_ANIM_TIME = FINAL_VICTORY_LABEL_ANIM_TIME + LABELS_ALPHA_ANIM_TIME;
    static const double FINAL_UNLOCKED_LABELS_ANIM_TIME = FINAL_WORLD_LABELS_ANIM_TIME + LABELS_ALPHA_ANIM_TIME;
   
    this->worldCompleteAlphaAnim.SetLerp(FINAL_VICTORY_LABEL_ANIM_TIME, FINAL_WORLD_LABELS_ANIM_TIME, 0.0f, 1.0f);
    this->unlockedAlphaAnim.SetLerp(FINAL_WORLD_LABELS_ANIM_TIME, FINAL_UNLOCKED_LABELS_ANIM_TIME, 0.0f, 1.0f);

    this->glowRotationAnim.SetLerp(0.0, 4.0, 0.0f, 360.0f);
    this->glowRotationAnim.SetRepeat(true);
    this->glowScaleAnim.SetLerp(FINAL_VICTORY_LABEL_ANIM_TIME, FINAL_VICTORY_LABEL_ANIM_TIME + 0.5, 
        0.0f, 1.25f * std::max<float>(this->victoryLabel1.GetLastRasterWidth(),
        std::max<float>(this->victoryLabel2.GetLastRasterWidth(), this->GetVictoryLabelBlockHeight())));

	// Set the footer colour flash animation
    this->footerColourAnimation = GameViewConstants::GetInstance()->BuildFlashingColourAnimation();
}

BossLevelCompleteSummaryDisplayState::~BossLevelCompleteSummaryDisplayState() {
    this->display->GetModel()->GetPlayerPaddle()->SetLevelBoundsChecking(true);

    if (this->unlockedLabel != NULL) {
        delete this->unlockedLabel;
        this->unlockedLabel = NULL;
    }
    if (this->worldCompleteLabel != NULL) {
        delete this->worldCompleteLabel;
        this->worldCompleteLabel = NULL;
    }

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->spinGlowTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bangStarTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTex);
    assert(success);
    UNUSED_VARIABLE(success);
}

void BossLevelCompleteSummaryDisplayState::RenderFrame(double dT) {
 
    const Camera& camera = this->display->GetCamera();
    GameSound* sound = this->display->GetSound();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Draw the background...
    bgRenderer->DrawBG(camera);

    this->allAnimationIsDone = true;
    this->allAnimationIsDone &= this->fadeInAnimation.Tick(dT);
    this->allAnimationIsDone &= this->glowScaleAnim.Tick(dT);
    this->allAnimationIsDone &= this->victoryLabelMoveAnim.Tick(dT);
    this->allAnimationIsDone &= this->worldCompleteAlphaAnim.Tick(dT);

    this->glowRotationAnim.Tick(dT);

    // Draw the fade-in overlay if visible...
    float fadeInAlpha = this->fadeInAnimation.GetInterpolantValue();
    if (fadeInAlpha > 0.0) {
		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GeometryMaker::GetInstance()->DrawFullScreenQuad(Camera::GetWindowWidth(),
            Camera::GetWindowHeight(), 1.0f, ColourRGBA(1, 1, 1, fadeInAlpha));

		glPopAttrib();
    }

    float currYPos = this->DrawVictoryLabel(Camera::GetWindowWidth());
    currYPos -= VICTORY_TO_WORLD_VERTICAL_PADDING;
    currYPos = this->DrawWorldCompleteLabel(Camera::GetWindowWidth(), currYPos);
    currYPos -= COMPLETE_TO_UNLOCKED_VERTICAL_PADDING;
    this->DrawUnlockedLabel(dT, Camera::GetWindowWidth(), currYPos);

    if (this->allAnimationIsDone) {
        this->waitingForKeyPress = true;
        this->footerColourAnimation.Tick(dT);
        this->DrawPressAnyKeyTextFooter(Camera::GetWindowWidth());
    }

    if (this->leavingState) {
        // We're no longer waiting for a key press - fade out to white and then switch to the next state

        // Begin fading out the summary screen
        bool fadeIsDone = this->fadeOutAnimation.Tick(dT);
        float fadeValue = this->fadeOutAnimation.GetInterpolantValue();
        
        bgRenderer->DrawBG(camera, fadeValue);

        if (fadeIsDone) {

            // Stop the background music by fading it out...
            // We only stop it in this state if the next state is not the game complete state...
            const GameModel* gameModel = this->display->GetModel();
            if (!gameModel->IsOnLastLevelOfGame()) {
                sound->StopSound(this->bgMusicSoundID, 1.0);
                this->bgMusicSoundID = INVALID_SOUND_ID;
            }

            // If we're done fading then we can go to the next state
            // Update the game model until there's a new queued state
            while (!this->display->SetCurrentStateAsNextQueuedState()) {
                this->display->UpdateModel(dT);
            }
		    return;
        }
    }
}

float BossLevelCompleteSummaryDisplayState::GetCenterTextBlockTopYCoord(float screenHeight) const {
    return screenHeight - (screenHeight - this->GetCenterTextBlockHeight()) / 2.0f;
}

float BossLevelCompleteSummaryDisplayState::GetCenterTextBlockHeight() const {
    return  this->GetVictoryLabelBlockHeight() + VICTORY_TO_WORLD_VERTICAL_PADDING +
        this->GetWorldCompleteLabelHeight() + COMPLETE_TO_UNLOCKED_VERTICAL_PADDING + 
        this->GetUnlockedLabelHeight();
}

float BossLevelCompleteSummaryDisplayState::GetVictoryLabelBlockHeight() const {
    return this->victoryLabel1.GetHeight() + VICTORY_LABEL_VERTICAL_SPACING +
        this->victoryLabel2.GetHeight();
}

float BossLevelCompleteSummaryDisplayState::GetWorldCompleteLabelHeight() const {
    if (this->worldCompleteLabel == NULL) {
        return 0.0f;
    }
    return this->worldCompleteLabel->GetHeight();
}

float BossLevelCompleteSummaryDisplayState::GetUnlockedLabelHeight() const {
    if (this->unlockedLabel == NULL) {
        return 0.0f;
    }
    return this->unlockedLabel->GetHeight();
}

float BossLevelCompleteSummaryDisplayState::DrawVictoryLabel(float screenWidth) {
   
    const float horizontalLabelSize1 = this->victoryLabel1.GetLastRasterWidth();
    const float horizontalLabelSize2 = this->victoryLabel2.GetLastRasterWidth();

    const float currXPos1 = (screenWidth - horizontalLabelSize1) / 2.0f;
    const float currXPos2 = (screenWidth - horizontalLabelSize2) / 2.0f;
    const float currYPos1 = this->victoryLabelMoveAnim.GetInterpolantValue();
    const float currYPos2 = currYPos1 - (this->victoryLabel1.GetHeight() + VICTORY_LABEL_VERTICAL_SPACING);

    // Draw the flashy glowy background
    float glowScale = this->glowScaleAnim.GetInterpolantValue();
    if (glowScale > 0.0) {
        
        if (this->victoryMessageSoundID == INVALID_SOUND_ID) {
            this->victoryMessageSoundID = this->display->GetSound()->PlaySound(GameSound::WorldCompleteVictoryMessageEvent, false, false);
        }

        float rotAmt = this->glowRotationAnim.GetInterpolantValue();

        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Camera::PushWindowCoords();
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
        
        glTranslatef((screenWidth - glowScale) / 2.0f + glowScale / 2.0f, currYPos1 - this->GetVictoryLabelBlockHeight() / 2.0f, 0.0f);
        
        glPushMatrix();
        glRotatef(rotAmt, 0.0f, 0.0f, 1.0f);
        glScalef(0.8f * glowScale, 0.8f * glowScale, 1.0f);
        
        const Colour& glowColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
        glColor4f(glowColour.R(), glowColour.G(), glowColour.B(), 1.0f);
        this->spinGlowTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        
        glPopMatrix();

        glPushMatrix();
        glRotatef(-rotAmt, 0.0f, 0.0f, 1.0f);
        glScalef(1.25f * glowScale, 1.25f * glowScale, 1.0f);

        glColor4f(1.0f, 0.9f, 0.9f, 1.0f);
        this->flareTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();        
        
        glPopMatrix();

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPushMatrix();
        glScalef(glowScale, 0.5f*glowScale, 1.0f);
        this->bangStarTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        this->bangStarTex->UnbindTexture();
        glPopMatrix();

        Camera::PopWindowCoords();
        glPopAttrib();
    }

    // Draw the labels
    this->victoryLabel1.SetTopLeftCorner(currXPos1, currYPos1);
    this->victoryLabel2.SetTopLeftCorner(currXPos2, currYPos2);

    this->victoryLabel1.Draw();
    this->victoryLabel2.Draw();

    return currYPos2 - this->victoryLabel2.GetHeight();
}

float BossLevelCompleteSummaryDisplayState::DrawWorldCompleteLabel(float screenWidth, float currYPos) {
    if (this->worldCompleteLabel == NULL) {
        return currYPos;
    }

    const float currXPos = (screenWidth - this->worldCompleteLabel->GetLastRasterWidth()) / 2.0f;
    
    this->worldCompleteLabel->SetTopLeftCorner(currXPos, currYPos);
    this->worldCompleteLabel->SetAlpha(this->worldCompleteAlphaAnim.GetInterpolantValue());

    this->worldCompleteLabel->Draw();

    return currYPos - this->worldCompleteLabel->GetHeight();
}

void BossLevelCompleteSummaryDisplayState::DrawUnlockedLabel(double dT, float screenWidth, float currYPos) {
    // Don't draw it if there isn't one
    if (this->unlockedLabel == NULL) {
        return;
    }
    this->unlockedAlphaAnim.Tick(dT);

    const float currXPos = (screenWidth - this->unlockedLabel->GetLastRasterWidth()) / 2.0f;
    this->unlockedLabel->SetAlpha(this->unlockedAlphaAnim.GetInterpolantValue());
    this->unlockedLabel->SetTopLeftCorner(currXPos, currYPos);
    this->unlockedLabel->Draw();
}

void BossLevelCompleteSummaryDisplayState::DrawPressAnyKeyTextFooter(float screenWidth) {
	const float horizontalLabelSize = this->pressAnyKeyLabel.GetLastRasterWidth();

	this->pressAnyKeyLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
	this->pressAnyKeyLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, 
        FOOTER_VERTICAL_PADDING + this->pressAnyKeyLabel.GetHeight());

	this->pressAnyKeyLabel.Draw();
}


void BossLevelCompleteSummaryDisplayState::AnyKeyWasPressed() {
	if (!this->waitingForKeyPress || this->leavingState) {
        return;
    }

    this->leavingState = true;

    GameSound* sound = this->display->GetSound();
    
    // Play the confirm sound
    sound->PlaySound(GameSound::WorldSummaryConfirmEvent, false, false);
}