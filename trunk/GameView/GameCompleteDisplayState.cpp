/**
 * GameCompleteDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameCompleteDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const char* GameCompleteDisplayState::DEMO_COMPLETE_TITLE_TEXT = "Demo Complete!"; 
const char* GameCompleteDisplayState::CONGRATS_TEXT = "Thanks for playing! Hope you enjoyed Biff! Bam!! Blammo!?!";
const char* GameCompleteDisplayState::NOTE_TEXT     = "";

const float GameCompleteDisplayState::TITLE_Y_GAP          = 30;
const float GameCompleteDisplayState::TITLE_CONGRATS_Y_GAP = 60;
const float GameCompleteDisplayState::CONGRATS_NOTE_Y_GAP  = 50;
const float GameCompleteDisplayState::TEXT_LABEL_X_PADDING = 70;

const float GameCompleteDisplayState::CAM_DIST_FROM_ORIGIN = 20;

const double GameCompleteDisplayState::FADE_IN_TIME     = 1.0;
const double GameCompleteDisplayState::FADE_OUT_TIME    = 1.0;

GameCompleteDisplayState::GameCompleteDisplayState(GameDisplay* display) : DisplayState(display), goBackToMainMenu(false),
titleLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Huge), DEMO_COMPLETE_TITLE_TEXT),
creditLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
                GameViewConstants::GetInstance()->GAME_CREDITS_TEXT),
licenseLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
             GameViewConstants::GetInstance()->LICENSE_TEXT),
congratsLabel(NULL), noteLabel(NULL), bbbTitleDisplay(0.80f) {
    
    // Setup any labels/text
	this->titleLabel.SetColour(Colour(1.0f, 0.6f, 0));
	this->titleLabel.SetDropShadow(Colour(0, 0, 0), 0.08f);
    this->titleLabel.SetScale(this->display->GetTextScalingFactor());

    this->congratsLabel = 
        new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        Camera::GetWindowWidth() - 2 * TEXT_LABEL_X_PADDING, CONGRATS_TEXT);
    this->congratsLabel->SetColour(Colour(0,0,0));
    this->congratsLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->congratsLabel->SetScale(this->display->GetTextScalingFactor());

    this->noteLabel = 
        new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
        Camera::GetWindowWidth() - 2 * TEXT_LABEL_X_PADDING, NOTE_TEXT);
    this->noteLabel->SetColour(Colour(0,0,0));
    this->noteLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);
    this->noteLabel->SetScale(this->display->GetTextScalingFactor());

    this->creditLabel.SetColour(Colour(0,0,0));
    this->creditLabel.SetScale(this->display->GetTextScalingFactor());
    this->licenseLabel.SetColour(Colour(0.33f, 0.33f, 0.33f));
    this->licenseLabel.SetScale(0.75f * this->display->GetTextScalingFactor());

    // Fade in animation
    fadeAnim.SetLerp(0.0, FADE_IN_TIME, 1.0f, 0.0f);
    fadeAnim.SetInterpolantValue(1.0f);
    fadeAnim.SetRepeat(false);

}

GameCompleteDisplayState::~GameCompleteDisplayState() {
    delete this->congratsLabel;
    this->congratsLabel = NULL;
    delete this->noteLabel;
    this->noteLabel = NULL;
}

void GameCompleteDisplayState::RenderFrame(double dT) {
	UNUSED_PARAMETER(dT);
	
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all of the text labels
    float yPos = Camera::GetWindowHeight() - TITLE_Y_GAP;
    this->titleLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->titleLabel.GetLastRasterWidth())/2.0f, yPos);
	this->titleLabel.Draw();
    yPos -= (this->titleLabel.GetHeight() + TITLE_CONGRATS_Y_GAP);

    this->congratsLabel->SetTopLeftCorner((Camera::GetWindowWidth() - this->congratsLabel->GetWidth())/2.0f, yPos);
    this->congratsLabel->Draw();
    yPos -= (this->congratsLabel->GetHeight() + CONGRATS_NOTE_Y_GAP);

    this->noteLabel->SetTopLeftCorner((Camera::GetWindowWidth() - this->noteLabel->GetWidth())/2.0f, yPos);
    this->noteLabel->Draw();

    // Render created by text
    static const int NAME_LICENCE_Y_PADDING = 20;
    this->creditLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->creditLabel.GetLastRasterWidth()) / 2.0f,
        this->creditLabel.GetHeight() + this->licenseLabel.GetHeight() + NAME_LICENCE_Y_PADDING + 5);
    this->creditLabel.Draw();
    this->licenseLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->licenseLabel.GetLastRasterWidth()) / 2.0f,
        this->licenseLabel.GetHeight() + NAME_LICENCE_Y_PADDING);
    this->licenseLabel.Draw();


	Camera menuCamera;
	menuCamera.SetPerspective();
	menuCamera.Move(Vector3D(0, 0, GameCompleteDisplayState::CAM_DIST_FROM_ORIGIN));
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	menuCamera.ApplyCameraTransform();
    this->bbbTitleDisplay.Draw(-5, 2, menuCamera);
    glPopMatrix();

	// Draw the fade quad overlay
    this->fadeAnim.Tick(dT);
    float fadeValue = this->fadeAnim.GetInterpolantValue();
    
    if (fadeValue != 0.0f) {
	    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	    glDisable(GL_TEXTURE_2D);
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    GeometryMaker::GetInstance()->DrawFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 1.0f, 
                                                         ColourRGBA(1, 1, 1, fadeValue));
	    glPopAttrib();

        if (fadeValue >= 1.0f && this->goBackToMainMenu) {
            this->display->SetCurrentState(new MainMenuDisplayState(this->display));
            return;
        }
    }


}

void GameCompleteDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                             const GameControl::ActionMagnitude& magnitude) {
	UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);
    
    // Only accept button presses after the fade-in
    if (this->fadeAnim.GetInterpolantValue() == 0.0f) {
        this->goBackToMainMenu = true;
        this->fadeAnim.SetLerp(0.0, FADE_OUT_TIME, 0.0f, 1.0f);
        this->fadeAnim.SetInterpolantValue(0.0f);
    }
}