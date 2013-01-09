/**
 * WorldStartDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "WorldStartDisplayState.h"
#include "GameDisplay.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameModel.h"

const double WorldStartDisplayState::FADE_OUT_TIME							= 1.0;		// Time it takes to fade to white when this state is being exited
const double WorldStartDisplayState::HEADER_WIPE_TIME						= 1.5;		// Time it takes for the header to wipe fade-in
const double WorldStartDisplayState::FOOTER_FLASH_TIME					= 0.5;		// Time to switch the footer colour when it's flashing
const float  WorldStartDisplayState::HEADER_WIPE_FADE_QUAD_SIZE = 110.0f;	// Size of the gradient quad used to wipe in the header

WorldStartDisplayState::WorldStartDisplayState(GameDisplay* display) : DisplayState(display),
waitingForKeyPress(false), starryBG(NULL),
worldNameLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Huge), ""),
nowEnteringLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "Now Entering..."),
pressAnyKeyLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "- Press Any Key to Continue -"){

	// Pause all game play elements in the game model
    this->display->GetModel()->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall | GameModel::PauseState);

    // Load background texture
    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
    GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear));
    assert(this->starryBG != NULL);

	// Setup the fade animation to be inactive to start
	this->fadeAnimation.SetInterpolantValue(0.0f);
	this->fadeAnimation.SetLerp(0.0f, 0.0f, 0.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);

	// Set the header fade wipe
	this->showHeaderAnimation.SetInterpolantValue(0.0f);
	this->showHeaderAnimation.SetLerp(0.0f, WorldStartDisplayState::HEADER_WIPE_TIME, 0.0f, this->display->GetCamera().GetWindowWidth() + 2 * HEADER_WIPE_FADE_QUAD_SIZE);
	this->showHeaderAnimation.SetRepeat(false);

	this->titleFadeIn.SetLerp(0.1 * HEADER_WIPE_TIME, HEADER_WIPE_TIME, 0.0f, 1.0f);
	this->titleFadeIn.SetRepeat(false);
	this->titleMoveIn.SetLerp(0.0, HEADER_WIPE_TIME, 0.0, this->display->GetCamera().GetWindowWidth() - 50.0f);
	this->titleMoveIn.SetRepeat(false);

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

	// Create a label for the current world name...
	const std::string& worldName = this->display->GetModel()->GetCurrentWorldName();
	this->worldNameLabel.SetText(worldName);
	this->worldNameLabel.SetDropShadow(Colour(0, 0, 0), 0.05f);
	this->worldNameLabel.SetColour(Colour(1.0f, 0.5f, 0.0f));			// Orange
	this->worldNameLabel.SetScale(this->display->GetTextScalingFactor());

	// Setup the label for the Now entering text...
	this->nowEnteringLabel.SetDropShadow(Colour(1, 1, 1), 0.075f);
	this->nowEnteringLabel.SetColour(Colour(0.39f, 0.72f, 1.0f));	// Steel blue colour
	this->nowEnteringLabel.SetScale(1.25 * this->display->GetTextScalingFactor());

	// Setup the label for the press any key text...
	this->pressAnyKeyLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	this->pressAnyKeyLabel.SetScale(this->display->GetTextScalingFactor());

	// Make sure we dont' exceed the limits of OpenGL line widths
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, this->minMaxLineWidth);
}

WorldStartDisplayState::~WorldStartDisplayState() {
    bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);

    UNUSED_VARIABLE(success);
}

void WorldStartDisplayState::RenderFrame(double dT) {
	const Camera& camera   = this->display->GetCamera();
	
	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Draw the starry background...
    this->starryBG->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()));
    this->starryBG->UnbindTexture();

	// Draw the splash screen for the current world...
	Camera::PushWindowCoords();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Draw the header of the splash screen: " Now Entering... -------------------------------- "
	bool allAnimationDone = this->showHeaderAnimation.Tick(dT);
	allAnimationDone &= this->titleFadeIn.Tick(dT);
	allAnimationDone &= this->titleMoveIn.Tick(dT);
	this->waitingForKeyPress = allAnimationDone;
	this->DrawNowEnteringTextHeader(camera.GetWindowWidth(), camera.GetWindowHeight());

	// If we're waiting for the user's key press then flash on and off the text telling them to press any key
	if (this->waitingForKeyPress) {
		this->footerColourAnimation.Tick(dT);
		this->DrawPressAnyKeyTextFooter(camera.GetWindowWidth());
	}

	Camera::PopWindowCoords();

	// Are we fading out yet?
	bool isFadeAnimationDone = fadeAnimation.Tick(dT);
	float fadeValue = fadeAnimation.GetInterpolantValue();
	if (fadeValue != 0.0f) {

		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->starryBG->BindTexture();
        GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 
            GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowWidth()) / static_cast<float>(this->starryBG->GetWidth()),
            GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(camera.GetWindowHeight()) / static_cast<float>(this->starryBG->GetHeight()),
            ColourRGBA(1, 1, 1, fadeValue));
        this->starryBG->UnbindTexture();

		glPopAttrib();

		if (isFadeAnimationDone) {
			// All done our fade, time to go to the next queued state...
			this->display->SetCurrentStateAsNextQueuedState();
		}
	}
}

void WorldStartDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                           const GameControl::ActionMagnitude& magnitude) {

	UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);

    this->AnyKeyWasPressed();
}

void WorldStartDisplayState::MousePressed(const GameControl::MouseButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
    this->AnyKeyWasPressed();
}

void WorldStartDisplayState::AnyKeyWasPressed() {
	if (this->waitingForKeyPress) {
		// Start the fade out animation - the user wants to start playing!
		this->fadeAnimation.SetLerp(WorldStartDisplayState::FADE_OUT_TIME, 1.0f);
	}
}

void WorldStartDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

void WorldStartDisplayState::DrawNowEnteringTextHeader(float screenWidth, float screenHeight) {
	static const float NOW_ENTERING_TO_LINE_PADDING = 20.0f;
	static const float HORIZONTAL_PADDING = 20.0f;
	static const float VERTICAL_PADDING   = 25.0f;

	const float nowEnteringTextHeight = this->nowEnteringLabel.GetHeight(); 

	// Draw the "Now Entering..." label in the top left part of the screen
	this->nowEnteringLabel.SetTopLeftCorner(HORIZONTAL_PADDING, screenHeight - VERTICAL_PADDING);
	this->nowEnteringLabel.Draw();
	const float nowEnteringTextWidth = this->nowEnteringLabel.GetLastRasterWidth();

	glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);

	glPushMatrix();
	// Draw a line that goes from the end of the label to the other side of the screen
	glTranslatef(HORIZONTAL_PADDING + nowEnteringTextWidth + NOW_ENTERING_TO_LINE_PADDING, screenHeight - (nowEnteringTextHeight + VERTICAL_PADDING), 0.0f);

	float lineThicknessInPixels = std::min<float>(this->minMaxLineWidth[1], std::max<float>(this->minMaxLineWidth[0], 0.1f * nowEnteringTextHeight)); 
	glLineWidth(lineThicknessInPixels);

	float dropShadowPercent = 2.0f * this->nowEnteringLabel.GetDropShadow().amountPercentage;
	const ColourRGBA& labelColour = this->nowEnteringLabel.GetColour();
	
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex2f(dropShadowPercent, -dropShadowPercent);
	glVertex2f(screenWidth - (2 * HORIZONTAL_PADDING + nowEnteringTextWidth + NOW_ENTERING_TO_LINE_PADDING) + dropShadowPercent, -dropShadowPercent);

	glColor4f(labelColour.R(), labelColour.G(), labelColour.B(), labelColour.A());
	glVertex2f(0.0f, 0.0f);
	glVertex2f(screenWidth - (2 * HORIZONTAL_PADDING + nowEnteringTextWidth + NOW_ENTERING_TO_LINE_PADDING), 0.0f);
	glEnd();
	
	glPopMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslatef(0.0f, screenHeight, 0.0f);

	const float quadHeight			  = nowEnteringTextHeight + 2 * VERTICAL_PADDING;
	const float currOpaqueQuadWidth	  = screenWidth + HEADER_WIPE_FADE_QUAD_SIZE - this->showHeaderAnimation.GetInterpolantValue();

	// Draw the header fade in quads - 2 quads, one is totally opaque and starts covering the entire
	// header, the other quad is a gradiant from opaque to transparent and it moves along the header
	// over time, revealing it as it goes (wipe fade in)
    this->starryBG->BindTexture();
    glBegin(GL_QUADS);

    float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(this->starryBG->GetWidth()));
    float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(this->starryBG->GetHeight()));

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	if (currOpaqueQuadWidth > 0.0f) {
		// Opaque quad
        glTexCoord2f(0.0f, totalTexV);
		glVertex2f(0.0f, 0.0f);
        glTexCoord2f(0.0f, totalTexV - quadHeight * totalTexV / screenHeight);
		glVertex2f(0.0f, -quadHeight);
        glTexCoord2f(currOpaqueQuadWidth * totalTexU / screenWidth, totalTexV - quadHeight * totalTexV / screenHeight);
		glVertex2f(currOpaqueQuadWidth, -quadHeight);
        glTexCoord2f(currOpaqueQuadWidth * totalTexU / screenWidth, totalTexV);
		glVertex2f(currOpaqueQuadWidth, 0.0f);
	}

	// Gradient quad used to do the wipe
    glTexCoord2f(currOpaqueQuadWidth * totalTexU / screenWidth, totalTexV);
	glVertex2f(currOpaqueQuadWidth, 0.0f);
    glTexCoord2f(currOpaqueQuadWidth * totalTexU / screenWidth, totalTexV - quadHeight * totalTexV / screenHeight);
	glVertex2f(currOpaqueQuadWidth, -quadHeight);
	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    glTexCoord2f((currOpaqueQuadWidth + HEADER_WIPE_FADE_QUAD_SIZE) * totalTexU / screenWidth, totalTexV - quadHeight * totalTexV / screenHeight);
	glVertex2f(currOpaqueQuadWidth + HEADER_WIPE_FADE_QUAD_SIZE, -quadHeight);
    glTexCoord2f((currOpaqueQuadWidth + HEADER_WIPE_FADE_QUAD_SIZE) * totalTexU / screenWidth, totalTexV);
	glVertex2f(currOpaqueQuadWidth + HEADER_WIPE_FADE_QUAD_SIZE, 0.0f);

	glEnd();
    this->starryBG->UnbindTexture();

	glPopMatrix();

	// Draw the world name text label...
	const float nameLabelXPos = this->titleMoveIn.GetInterpolantValue();
	this->worldNameLabel.SetTopLeftCorner(nameLabelXPos - this->worldNameLabel.GetLastRasterWidth(), 
                                          screenHeight - (nowEnteringTextHeight + 3 * VERTICAL_PADDING));
	this->worldNameLabel.SetAlpha(this->titleFadeIn.GetInterpolantValue());
	this->worldNameLabel.Draw();

	glPopAttrib();
}

void WorldStartDisplayState::DrawPressAnyKeyTextFooter(float screenWidth) {
	static const float VERTICAL_PADDING = 20.0f;
	const float horizontalLabelSize = this->pressAnyKeyLabel.GetLastRasterWidth();

	this->pressAnyKeyLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
	this->pressAnyKeyLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, VERTICAL_PADDING + this->pressAnyKeyLabel.GetHeight());

	this->pressAnyKeyLabel.Draw();
}