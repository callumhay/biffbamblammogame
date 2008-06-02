#include "GameAssets.h"
#include "Mesh.h"
#include "ObjReader.h"
#include "TextureFontSet.h"

#include "../Utils/Includes.h"

// Asset file path constants ***************************
const std::string GameAssets::RESOURCE_DIR = "resources";
const std::string GameAssets::FONT_DIR	= "fonts";
const std::string GameAssets::MESH_DIR	= "models";


// Regular font assets
const std::string GameAssets::FONT_GUNBLAM					= GameAssets::RESOURCE_DIR  + "/" + FONT_DIR + "/gunblam.ttf";
const std::string GameAssets::FONT_EXPLOSIONBOOM		= GameAssets::RESOURCE_DIR  + "/" + FONT_DIR + "/explosionboom.ttf";
const std::string GameAssets::FONT_BLOODCRUNCH			= GameAssets::RESOURCE_DIR  + "/" + FONT_DIR + "/bloodcrunch.ttf";
const std::string GameAssets::FONT_ALLPURPOSE				= GameAssets::RESOURCE_DIR  + "/" + FONT_DIR + "/allpurpose.ttf";
//const std::string FONT_DECOISH;
//const std::string FONT_CYBERPUNKISH;

// Deco assets
const std::string GameAssets::DECO_PADDLE_MESH						= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/deco_paddle.obj";
const std::string GameAssets::DECO_BALL_MESH							= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/ball.obj";
const std::string GameAssets::DECO_BREAKABLE_BLOCK_MESH		= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/deco_block.obj";

// Cyberpunk assets
const std::string GameAssets::CYBERPUNK_PADDLE_MESH						= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/cyberpunk_paddle.obj";
const std::string GameAssets::CYBERPUNK_BALL_MESH							= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/ball.obj";
const std::string GameAssets::CYBERPUNK_BREAKABLE_BLOCK_MESH	= GameAssets::RESOURCE_DIR  + "/" + MESH_DIR + "/cyberpunk_block.obj";
// *****************************************************

GameAssets::GameAssets(): ball(NULL), playerPaddle(NULL), block(NULL), currLoadedStyle(GameWorld::None) {
	this->LoadRegularFontAssets();
}

GameAssets::~GameAssets() {
	// Delete the currently loaded style assets if there are any
	this->DeleteStyleAssets();
	
	// Delete the regular fonts
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>>::iterator fontSetIter;
	std::map<FontSize, TextureFontSet*>::iterator fontIter;
	for (fontSetIter = this->fonts.begin(); fontSetIter != this->fonts.end(); fontSetIter++) {
		std::map<FontSize, TextureFontSet*>& fontSet = fontSetIter->second;
		for (fontIter = fontSet.begin(); fontIter != fontSet.end(); fontIter++) {
			delete fontIter->second;
		}
		fontSet.clear();
	}
	this->fonts.clear();
}

/*
 * Delete any previously loaded assets.
 * Precondition: true.
 */
void GameAssets::DeleteStyleAssets() {
	if (this->ball != NULL) {
		delete this->ball;
	}
	if (this->playerPaddle != NULL) {
		delete this->playerPaddle;
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
void GameAssets::LoadAssets(GameWorld::WorldStyle style) {
	
	// Delete all previously loaded assets
	this->DeleteStyleAssets();

	// Load in the new asset set
	switch (style) {
		case GameWorld::Deco:
			this->LoadDecoStyleAssets();
			break;
		case GameWorld::Cyberpunk:
			this->LoadCyberpunkStyleAssets();
			break;
		default:
			assert(false);
	}

	this->currLoadedStyle = style;
}

/**
 * Load the regular game fonts - these are always in memory since they are used
 * throughout the game in various places.
 */
void GameAssets::LoadRegularFontAssets() {
	debug_output("Loading regular font sets");

	TextureFontSet* temp = TextureFontSet::CreateTextureFontFromTTF(FONT_GUNBLAM, Small);
	assert(temp != NULL);
	this->fonts[GunBlam][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_GUNBLAM, Medium);
	assert(temp != NULL);
	this->fonts[GunBlam][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_GUNBLAM, Big);
	assert(temp != NULL);
	this->fonts[GunBlam][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_GUNBLAM, Huge);
	assert(temp != NULL);
	this->fonts[GunBlam][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_EXPLOSIONBOOM, Small);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_EXPLOSIONBOOM, Medium);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_EXPLOSIONBOOM, Big);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_EXPLOSIONBOOM, Huge);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_BLOODCRUNCH, Small);
	assert(temp != NULL);
	this->fonts[BloodCrunch][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_BLOODCRUNCH, Medium);
	assert(temp != NULL);
	this->fonts[BloodCrunch][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_BLOODCRUNCH, Big);
	assert(temp != NULL);
	this->fonts[BloodCrunch][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_BLOODCRUNCH, Huge);
	assert(temp != NULL);
	this->fonts[BloodCrunch][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ALLPURPOSE, Small);
	assert(temp != NULL);
	this->fonts[AllPurpose][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ALLPURPOSE, Medium);
	assert(temp != NULL);
	this->fonts[AllPurpose][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ALLPURPOSE, Big);
	assert(temp != NULL);
	this->fonts[AllPurpose][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ALLPURPOSE, Huge);
	assert(temp != NULL);
	this->fonts[AllPurpose][Huge]	= temp; 
}

/**
 * Load in the "Deco" assets for use in the game.
 */
void GameAssets::LoadDecoStyleAssets() {
	debug_output("Loading deco style assets");

	// Deco mesh assets
	this->ball					= ObjReader::ReadMesh(DECO_BALL_MESH);
	this->playerPaddle	= ObjReader::ReadMesh(DECO_PADDLE_MESH);
	this->block					= ObjReader::ReadMesh(DECO_BREAKABLE_BLOCK_MESH);

}

/**
 * Load in the "Cyberpunk" assets for use in the game.
 */
void GameAssets::LoadCyberpunkStyleAssets() {
	debug_output("Loading cyberpunk style assets");
	
	// Cyberpunk mesh assets
	this->ball					= ObjReader::ReadMesh(CYBERPUNK_BALL_MESH);
	this->playerPaddle	= ObjReader::ReadMesh(CYBERPUNK_PADDLE_MESH);
	this->block					= ObjReader::ReadMesh(CYBERPUNK_BREAKABLE_BLOCK_MESH);

}