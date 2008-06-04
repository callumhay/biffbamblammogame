#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "TextureFontSet.h"

// model stuffs
#include "../GameModel/GameModel.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

const float InGameDisplayState::FOV_ANGLE_IN_DEGS	= 45.0f;
const float InGameDisplayState::NEAR_PLANE_DIST		= 0.01f;
const float InGameDisplayState::FAR_PLANE_DIST		= 100.0f;

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
	
	// Set the perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_ANGLE_IN_DEGS, 
							   ((double)this->display->GetDisplayWidth()) / ((double)this->display->GetDisplayHeight()), 
								 NEAR_PLANE_DIST, FAR_PLANE_DIST);
	
	// Background
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// TODO: Camera...
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -43.0f); // Eye transform

	// Draw the game scene ----------------------------------------------------------
	glPushMatrix();

	Vector2D levelDim = this->display->GetModel()->GetLevelUnitDimensions();
	glTranslatef(-levelDim[0]/2.0f, -levelDim[1]/2.0f, 0.0f);
	
	this->DrawLevelPieces();
	this->DrawPlayerPaddle();
	this->display->GetAssets()->DrawGameBall(this->display->GetModel()->GetGameBall());

	glPopMatrix();
	// -------------------------------------------------------------------------------

	// Draw the HUD ------------------------------------------------------------------
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
	// -------------------------------------------------------------------------------
}

// Private helper functions ************************************************************

/*
 * Helper function for drawing all the game pieces / blocks that make up the currently
 * loaded level. This does not include drawing the player, ball, effects or backgrounds.
 */
void InGameDisplayState::DrawLevelPieces() {
	std::vector<std::vector<LevelPiece*>> &pieces = this->display->GetModel()->GetCurrentLevelPieces();

	// Go through each piece and draw
	for (size_t h = 0; h < pieces.size(); h++) {
		for (size_t w = 0; w < pieces[h].size(); w++) {
			LevelPiece* currPiece = pieces[h][w];
			this->display->GetAssets()->DrawLevelPieceMesh(*currPiece);
			currPiece->DebugDraw();
		}
	}
}

/*
 * Helper function for drawing the player paddle.
 */
void InGameDisplayState::DrawPlayerPaddle() {
	PlayerPaddle paddle = this->display->GetModel()->GetPlayerPaddle();
	this->display->GetAssets()->DrawPaddle(paddle);
}