/**
 * LevelCompleteSummaryDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelCompleteSummaryDisplayState.h"
#include "GameDisplay.h"
#include "GameFontAssetsManager.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameModel.h"

const double LevelCompleteSummaryDisplayState::FADE_OUT_TIME     = 0.75;
const double LevelCompleteSummaryDisplayState::FOOTER_FLASH_TIME = 0.5;

LevelCompleteSummaryDisplayState::LevelCompleteSummaryDisplayState(GameDisplay* display) :
DisplayState(display), waitingForKeyPress(true),
pressAnyKeyLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "- Press Any Key to Continue -"){
    
	// Pause all game play elements in the game model
	this->display->GetModel()->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

	// Setup the fade animation to be inactive initially
	this->fadeAnimation.SetInterpolantValue(0.0f);
	this->fadeAnimation.SetLerp(0.0f, 0.0f, 0.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);

	// Setup the label for the press any key text...
	this->pressAnyKeyLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	this->pressAnyKeyLabel.SetScale(this->display->GetTextScalingFactor());

	// Set the footer colour flash animation
	std::vector<double> timeVals;
	timeVals.reserve(3);
	timeVals.push_back(0.0);
	timeVals.push_back(FOOTER_FLASH_TIME);
	timeVals.push_back(2 * FOOTER_FLASH_TIME);
	std::vector<Colour> colourVals;
	colourVals.reserve(3);
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	colourVals.push_back(Colour(1.0f, 0.8f, 0.0f));
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	this->footerColourAnimation.SetLerp(timeVals, colourVals);
	this->footerColourAnimation.SetRepeat(true);

}

LevelCompleteSummaryDisplayState::~LevelCompleteSummaryDisplayState() {
}

void LevelCompleteSummaryDisplayState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /*
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    // TODO...

    Camera::PopWindowCoords();
    */

    if (this->waitingForKeyPress) {

        // Animate and draw the "Press any key..." label
        this->footerColourAnimation.Tick(dT);
        this->DrawPressAnyKeyTextFooter(camera.GetWindowWidth());

    }
    else {
        // We're no longer waiting for a key press - fade out to white and then switch to the next state

        // Begin fading out the summary screen
        bool fadeIsDone = this->fadeAnimation.Tick(dT);
        float fadeValue = this->fadeAnimation.GetInterpolantValue();

		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
                                                         ColourRGBA(1, 1, 1, fadeValue));
		glPopAttrib();

        if (fadeIsDone) {
            // If we're done fading then we can go to the next state
            // Update the game model until there's a new queued state
            while (!this->display->SetCurrentStateAsNextQueuedState()) {
                this->display->UpdateModel(dT);
            }
		    return;
        }
    }
}

void LevelCompleteSummaryDisplayState::DrawPressAnyKeyTextFooter(float screenWidth) {
	static const float VERTICAL_PADDING = 20.0f;
	const float horizontalLabelSize = this->pressAnyKeyLabel.GetLastRasterWidth();

	this->pressAnyKeyLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
	this->pressAnyKeyLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, VERTICAL_PADDING + this->pressAnyKeyLabel.GetHeight());

	this->pressAnyKeyLabel.Draw();
}