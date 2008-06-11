#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "TextureFontSet.h"

// model stuffs
#include "../GameModel/GameModel.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

#include <sstream>

const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

const float InGameDisplayState::FOV_ANGLE_IN_DEGS	= 45.0f;
const float InGameDisplayState::NEAR_PLANE_DIST		= 0.01f;
const float InGameDisplayState::FAR_PLANE_DIST		= 220.0f;

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

	this->gameCamera.Move(Vector3D(0, 0, 43.0f));
}

InGameDisplayState::~InGameDisplayState() {
}

/**
 * This will render a frame of the actual game while it is in-play.
 */
void InGameDisplayState::RenderFrame(double dT) {
	
	// TODO: Camera Stuff -----------------------------------------------------------
	// Set the perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_ANGLE_IN_DEGS, 
							   ((double)this->display->GetDisplayWidth()) / ((double)this->display->GetDisplayHeight()), 
								 NEAR_PLANE_DIST, FAR_PLANE_DIST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0, 0, -43.0f); // Eye transform

	this->gameCamera.ApplyCameraTransform();

	// -------------------------------------------------------------------------------

	// Draw the game scene
	this->DrawGameScene();

	// Draw the HUD
	this->DrawGameHUD();
}

// Private helper functions ************************************************************

/**
 * Helper function for drawing the game scene - this includes background and foreground objects
 * related to the game itself.
 */
void InGameDisplayState::DrawGameScene() {
	// Background
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Foreground game objects
	Vector2D levelDim = this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	
	glTranslatef(0.0f, -levelDim[1]/2.0f, 0.0f);
	this->display->GetAssets()->DrawBackground(this->gameCamera);

	glTranslatef(-levelDim[0]/2.0f, 0, 0.0f);	
	this->DrawLevelPieces();
	this->display->GetAssets()->DrawPaddle(this->display->GetModel()->GetPlayerPaddle(), this->gameCamera);
	this->display->GetAssets()->DrawGameBall(this->display->GetModel()->GetGameBall(), this->gameCamera);

	glPopMatrix();
}

/*
 * Helper function for drawing all the game pieces / blocks that make up the currently
 * loaded level. This does not include drawing the player, ball, effects or backgrounds.
 */
void InGameDisplayState::DrawLevelPieces() {
	std::vector<std::vector<LevelPiece*>> &pieces = this->display->GetModel()->GetCurrentLevelPieces();
	this->display->GetAssets()->DrawLevelPieces(pieces, this->gameCamera);
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