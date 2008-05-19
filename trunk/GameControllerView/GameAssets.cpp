#include "GameAssets.h"
#include "Mesh.h"
#include "ObjReader.h"

#include "../Utils/Includes.h"


GameAssets::GameAssets(): playerPaddle(NULL), ball(NULL), block(NULL), currLoadedStyle(-1) {
}

GameAssets::~GameAssets() {
	this->DeleteAssets();
}

/*
 * Delete any previously loaded assets.
 * Precondition: true.
 */
void GameAssets::DeleteAssets() {
	if (this->playerPaddle != NULL) {
		delete this->playerPaddle;
	}
	if (this->ball != NULL) {
		delete this->ball;
	}
	if (this->block != NULL) {
		delete this->block;
	}
}

// Draw a piece of the level (block that you destory or that makes up part of the level
// outline), this is done by positioning it, drawing the correct material and then
// drawing the mesh itself.
void GameAssets::DrawLevelPieceMesh(const LevelPiece &p) {
	Point2D loc = p.GetCenter();
	
	// Draw the piece with its appropriate transform
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	
	// General material characteristics
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	GLfloat blockSpecular[]   = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, blockSpecular);

	switch(p.GetType()) {
		case LevelPiece::RedBreakable:
			{
				GLfloat redAmbAndDiff[] = {1.0f, 0.0f, 0.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, redAmbAndDiff);
				this->block->Draw();
			}
			break;			
		case LevelPiece::OrangeBreakable: 
			{
				GLfloat orangeAmbAndDiff[] = {1.0f, 0.5f, 0.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, orangeAmbAndDiff);
				this->block->Draw();
			}
			break;
		case LevelPiece::YellowBreakable: 
			{
				GLfloat yellowAmbAndDiff[] = {1.0f, 1.0f, 0.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellowAmbAndDiff);
				this->block->Draw();
			}
			break;
		case LevelPiece::GreenBreakable:
			{
				GLfloat greenAmbAndDiff[] = {0.0f, 1.0f, 0.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, greenAmbAndDiff);
				this->block->Draw();
			}
			break;			
		case LevelPiece::Solid:
			{
				GLfloat greyAmbAndDiff[] = {0.8f, 0.8f, 0.8f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, greyAmbAndDiff);
				this->block->Draw();
			}
			break;
		case LevelPiece::Bomb:
			// Nothing to draw here yet..
			break;
		default:
			break;
	}

	glPopMatrix();
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBall(const GameBall& b) {
	
	// Ball material characteristics
	GLfloat ballSpecular[]   = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat ballAmbAndDiff[] = {0.1f, 0.0f, 0.5f, 1.0f};
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ballSpecular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ballAmbAndDiff);

	Point2D loc = b.GetBounds().Center();
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	this->ball->Draw();
	glPopMatrix();
}

void GameAssets::DrawPaddle(const PlayerPaddle& p) {
	Point2D paddleCenter = p.GetCenterPosition();	

	GLfloat paddleSpecular[]   = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat paddleAmbAndDiff[] = {0.65f, 0.7f, 1.0f, 1.0f};
	glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);
	glMaterialfv(GL_FRONT, GL_SPECULAR, paddleSpecular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, paddleAmbAndDiff);

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1], 0);
	this->playerPaddle->Draw();
	p.DebugDraw();
	glPopMatrix();
}

/*
 * This will load a set of assets for use in a game based off a
 * given world-style, after loading all assets will be available for use
 * in-game.
 * Precondition: true.
 */
void GameAssets::LoadAssets(GameWorld::WorldStyle assetSet) {
	// Do nothing if we have already loaded the given style
	if (assetSet == this->currLoadedStyle) {
		return;
	}

	// Delete all previously loaded assets
	this->DeleteAssets();

	// Load in the new asset set
	switch(assetSet) {
		case GameWorld::Deco:
			this->LoadDecoStyleAssets();
			break;
		default:
			assert(false);
			return;
	}

	this->currLoadedStyle = assetSet;
}

/*
 * Load in the "Deco" assets for use in the game.
 * Precondition: true.
 */
void GameAssets::LoadDecoStyleAssets() {
	this->playerPaddle = ObjReader::ReadMesh("resources/models/deco_paddle.obj");
	this->ball = ObjReader::ReadMesh("resources/models/ball.obj");
	this->block = ObjReader::ReadMesh("resources/models/deco_block.obj");

}