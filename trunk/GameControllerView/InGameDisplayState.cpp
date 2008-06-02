#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"

// model stuffs
#include "../GameModel/GameModel.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"


const float InGameDisplayState::FOV_ANGLE_IN_DEGS	= 45.0f;
const float InGameDisplayState::NEAR_PLANE_DIST		= 0.01f;
const float InGameDisplayState::FAR_PLANE_DIST		= 100.0f;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display) {
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

	// Draw the game scene
	glPushMatrix();

	Vector2D levelDim = this->display->GetModel()->GetLevelUnitDimensions();
	glTranslatef(-levelDim[0]/2.0f, -levelDim[1]/2.0f, 0.0f);
	
	this->DrawLevelPieces();
	this->DrawPlayerPaddle();
	this->display->GetAssets()->DrawGameBall(this->display->GetModel()->GetGameBall());

	glPopMatrix();
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