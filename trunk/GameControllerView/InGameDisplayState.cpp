#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "TextureFontSet.h"

// model stuffs
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

#include <sstream>

const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display) {
	
	// Set HUD display elements
	float dropShadowAmt = 0.05f;
	Colour shadowColourHUD(0, 0, 0);
	Colour textColourHUD(1, 1, 1);
	// Score display
	this->scoreLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Small), "0");
	this->scoreLabel.SetColour(textColourHUD);
	this->scoreLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
	// Lives left display
	this->livesLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Small), LIVES_LABEL_TEXT);
	this->livesLabel.SetColour(textColourHUD);
	this->livesLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
}

InGameDisplayState::~InGameDisplayState() {
}

/**
 * This will render a frame of the actual game while it is in-play.
 */
void InGameDisplayState::RenderFrame(double dT) {
	
	// Camera Stuff -----------------------------------------------------------------
	this->display->GetCamera().SetPerspective(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->display->GetCamera().ApplyCameraTransform();

	// -------------------------------------------------------------------------------

	// Draw the game scene
	this->DrawGameScene(dT);

	// Draw the HUD
	this->DrawGameHUD();
}

// Private helper functions ************************************************************

/**
 * Helper function for drawing the game scene - this includes background and foreground objects
 * related to the game itself.
 */
void InGameDisplayState::DrawGameScene(double dT) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	Vector2D levelDim = this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	
	// Draw the background scenery
	glTranslatef(0.0f, -levelDim[1]/2.0f, 0.0f);
	this->display->GetAssets()->DrawBackground(dT, this->display->GetCamera());
	
	// Draw the foreground stuff (paddle, ball, items)
	glTranslatef(-levelDim[0]/2.0f, 0, 0.0f);	
	this->display->GetAssets()->DrawPaddle(*this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());
	this->display->GetAssets()->DrawGameBall(*this->display->GetModel()->GetGameBall(), this->display->GetCamera());

	std::vector<GameItem*> &gameItems = this->display->GetModel()->GetLiveItems();
	for (std::vector<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
		this->display->GetAssets()->DrawItem((**iter), this->display->GetCamera());
	}

	glPopMatrix();

	this->display->GetAssets()->DrawLevelPieces(this->display->GetCamera());
}

/**
 * Helper function for drawing the Heads-Up-Display (HUD) for the game, 
 * including points, lives left, etc.
 */
void InGameDisplayState::DrawGameHUD() {
	// Draw the points in the top-right corner of the display
	std::stringstream ptStrStream;
	ptStrStream << this->display->GetModel()->GetScore();
	this->scoreLabel.SetText(ptStrStream.str());
	this->scoreLabel.SetTopLeftCorner(Point2D(this->display->GetDisplayWidth() - HUD_X_INDENT - this->scoreLabel.GetLastRasterWidth(), 
																			      this->display->GetDisplayHeight() - HUD_Y_INDENT));
	this->scoreLabel.Draw();

	// Draw the number of lives left in the top-left corner of the display
	// TODO: figure out number of lives... also, perhaps sprites or models instead?
	std::stringstream livesStrStream;
	livesStrStream << LIVES_LABEL_TEXT << this->display->GetModel()->GetLivesLeft();
	this->livesLabel.SetText(livesStrStream.str());
	this->livesLabel.SetTopLeftCorner(Point2D(HUD_X_INDENT, this->display->GetDisplayHeight() - HUD_Y_INDENT));
	this->livesLabel.Draw();
}