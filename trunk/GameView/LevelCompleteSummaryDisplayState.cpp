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
#include "GameViewConstants.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameModel.h"
#include "../ResourceManager.h"

const double LevelCompleteSummaryDisplayState::FADE_OUT_TIME                 = 0.75;
const double LevelCompleteSummaryDisplayState::FOOTER_FLASH_TIME             = 0.5;
const float LevelCompleteSummaryDisplayState::FOOTER_VERTICAL_PADDING        = 20.0f;
const float LevelCompleteSummaryDisplayState::LEVEL_NAME_HORIZONTAL_PADDING  = 20.0f;
const float LevelCompleteSummaryDisplayState::HEADER_LEVEL_NAME_VERTICAL_PADDING = 20.0f;
const float LevelCompleteSummaryDisplayState::HEADER_INBETWEEN_VERTICAL_PADDING  = 20.0f;
const float LevelCompleteSummaryDisplayState::TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING = 10.0f;
const float LevelCompleteSummaryDisplayState::HEADER_SCORE_INBETWEEN_VERTICAL_PADDING = 50.0f;

const double LevelCompleteSummaryDisplayState::POINTS_PER_SECOND = 10000;

LevelCompleteSummaryDisplayState::LevelCompleteSummaryDisplayState(GameDisplay* display) :
DisplayState(display), waitingForKeyPress(true),
levelNameLabel(NULL),
levelCompleteLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "Level Complete!"),
pressAnyKeyLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "- Press Any Key to Continue -"),
totalScoreLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), "Total Score:"),
scoreValueLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), "0"),
maxScoreValueWidth(0), starTexture(NULL) {

    GameModel* gameModel = this->display->GetModel();
    assert(gameModel != NULL);

	// Pause all game play elements in the game model
	gameModel->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

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

    this->levelCompleteLabel.SetColour(Colour(1.0f, 0.8f, 0.0f));
    this->levelCompleteLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);

    this->levelNameLabel = new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), 
        display->GetCamera().GetWindowWidth() - 2*LEVEL_NAME_HORIZONTAL_PADDING, 
        std::string("\"") + this->display->GetModel()->GetCurrentLevel()->GetName() + std::string("\""));
    this->levelNameLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
    this->levelNameLabel->SetDropShadow(Colour(0,0,0), 0.1f);

    // Setup the animation for the score
    
    double gameScore = static_cast<double>(gameModel->GetScore());
    double totalTime = gameScore / POINTS_PER_SECOND;
    this->scoreValueAnimation.SetInterpolantValue(0.0);
    this->scoreValueAnimation.SetLerp(0.0, totalTime, 0.0, gameScore);
    this->scoreValueAnimation.SetRepeat(false);

    // Figure out the maximum score value label width
    std::stringstream maxScoreValueStrStream;
    maxScoreValueStrStream << static_cast<int>(this->scoreValueAnimation.GetTargetValue());
    this->scoreValueLabel.SetText(maxScoreValueStrStream.str());
    this->maxScoreValueWidth = this->scoreValueLabel.GetLastRasterWidth();

    std::stringstream initScoreValueStrStream;
    maxScoreValueStrStream << static_cast<int>(this->scoreValueAnimation.GetInterpolantValue());
    this->scoreValueLabel.SetText(initScoreValueStrStream.str());

    // Grab any required texture resources
    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);
}

LevelCompleteSummaryDisplayState::~LevelCompleteSummaryDisplayState() {
    delete this->levelNameLabel;
    this->levelNameLabel = NULL;

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
}

void LevelCompleteSummaryDisplayState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (this->waitingForKeyPress) {
        // Set the score animation value
        this->scoreValueAnimation.Tick(dT);
        std::stringstream scoreValueStrStream;
        scoreValueStrStream << static_cast<int>(this->scoreValueAnimation.GetInterpolantValue());
        this->scoreValueLabel.SetText(scoreValueStrStream.str());


        // Animate and draw the "Press any key..." label
        this->footerColourAnimation.Tick(dT);
        this->DrawPressAnyKeyTextFooter(camera.GetWindowWidth());
    }

	//Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    this->DrawLevelNameLabel(camera.GetWindowWidth(), camera.GetWindowHeight());
    this->DrawLevelCompleteLabel(camera.GetWindowWidth(), camera.GetWindowHeight());
    this->DrawTotalScoreLabel(camera.GetWindowWidth(), camera.GetWindowHeight());

    //Camera::PopWindowCoords();

    if (!this->waitingForKeyPress) {
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

void LevelCompleteSummaryDisplayState::DrawLevelNameLabel(float screenWidth, float screenHeight) {
    const float horizontalLabelSize = this->levelNameLabel->GetWidth();
    this->levelNameLabel->SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, 
        screenHeight - HEADER_LEVEL_NAME_VERTICAL_PADDING);
    this->levelNameLabel->Draw();
}

void LevelCompleteSummaryDisplayState::DrawLevelCompleteLabel(float screenWidth, float screenHeight) {
    
    const float horizontalLabelSize = this->levelCompleteLabel.GetLastRasterWidth();
    this->levelCompleteLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, 
        screenHeight - (this->levelNameLabel->GetHeight() + HEADER_LEVEL_NAME_VERTICAL_PADDING + HEADER_INBETWEEN_VERTICAL_PADDING));
    this->levelCompleteLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawStars(float screenWidth, float screenHeight) {
    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    glPopAttrib();
}

void LevelCompleteSummaryDisplayState::DrawTotalScoreLabel(float screenWidth, float screenHeight) {
    const float totalScoreXSize = this->totalScoreLabel.GetLastRasterWidth();
    //const float scoreValueXSize = this->scoreValueLabel.GetLastRasterWidth();
    
    float totalScoreXPos = screenWidth/2 - TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING - totalScoreXSize;
    float scoreValueXPos = screenWidth/2 + TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;

    float yPos = screenHeight - (HEADER_LEVEL_NAME_VERTICAL_PADDING + this->levelNameLabel->GetHeight() +
        HEADER_INBETWEEN_VERTICAL_PADDING + this->levelCompleteLabel.GetHeight() + HEADER_SCORE_INBETWEEN_VERTICAL_PADDING);
    
    this->totalScoreLabel.SetTopLeftCorner(totalScoreXPos, yPos);
    this->scoreValueLabel.SetTopLeftCorner(scoreValueXPos, yPos);

    this->totalScoreLabel.Draw();
    this->scoreValueLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawPressAnyKeyTextFooter(float screenWidth) {
	const float horizontalLabelSize = this->pressAnyKeyLabel.GetLastRasterWidth();

	this->pressAnyKeyLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
	this->pressAnyKeyLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, 
        FOOTER_VERTICAL_PADDING + this->pressAnyKeyLabel.GetHeight());

	this->pressAnyKeyLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawStar(float leftX, float bottomY, const Colour& colour) {


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
}