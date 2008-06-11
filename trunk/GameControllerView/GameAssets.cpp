#include "GameAssets.h"
#include "Mesh.h"
#include "ObjReader.h"
#include "TextureFontSet.h"
#include "CgShaderManager.h"
#include "GameDisplay.h"

#include "../Utils/Includes.h"

// Asset file path constants ***************************
const std::string GameAssets::RESOURCE_DIR = "resources";
const std::string GameAssets::FONT_DIR	= "fonts";
const std::string GameAssets::MESH_DIR	= "models";
const std::string GameAssets::SHADER_DIR = "shaders";

// Shader assets
const std::string GameAssets::CELSHADER_FILEPATH	= GameAssets::RESOURCE_DIR + "/" + SHADER_DIR + "/CelShading.cgfx";

// Regular font assets
const std::string GameAssets::FONT_GUNBLAM				= GameAssets::RESOURCE_DIR + "/" + FONT_DIR + "/gunblam.ttf";
const std::string GameAssets::FONT_EXPLOSIONBOOM	= GameAssets::RESOURCE_DIR + "/" + FONT_DIR + "/explosionboom.ttf";
const std::string GameAssets::FONT_BLOODCRUNCH		= GameAssets::RESOURCE_DIR + "/" + FONT_DIR + "/bloodcrunch.ttf";
const std::string GameAssets::FONT_ALLPURPOSE			= GameAssets::RESOURCE_DIR + "/" + FONT_DIR + "/allpurpose.ttf";
const std::string GameAssets::FONT_ELECTRICZAP		= GameAssets::RESOURCE_DIR + "/" + FONT_DIR + "/electriczap.ttf";
//const std::string FONT_DECOISH;
//const std::string FONT_CYBERPUNKISH;

// Regular mesh asssets
const std::string GameAssets::BALL_MESH		= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/ball.obj";
const std::string GameAssets::BLOCK_MESH	= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/block.obj";

// Deco assets
const std::string GameAssets::DECO_PADDLE_MESH						= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/deco_paddle.obj";
const std::string GameAssets::DECO_SOLID_BLOCK_MESH				= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/deco_solid_block.obj";
const std::string GameAssets::DECO_BACKGROUND_MESH				= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/deco_background.obj";

// Cyberpunk assets
const std::string GameAssets::CYBERPUNK_PADDLE_MESH						= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/cyberpunk_paddle.obj";
const std::string GameAssets::CYBERPUNK_SOLID_BLOCK_MESH			= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/cyberpunk_block.obj";
const std::string GameAssets::CYBERPUNK_BACKGROUND_MESH				= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/cyberpunk_background.obj";
// *****************************************************

GameAssets::GameAssets(): ball(NULL), playerPaddle(NULL), breakableBlock(NULL), 
solidBlock(NULL), background(NULL), currLoadedStyle(GameWorld::None) {
	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	
	// Load Fonts
	this->LoadRegularFontAssets();
}

GameAssets::~GameAssets() {
	// Delete regular mesh assets
	if (this->ball != NULL) {
		delete this->ball;
	}
	if (this->breakableBlock != NULL) {
		delete this->breakableBlock;
	}

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
	if (this->playerPaddle != NULL) {
		delete this->playerPaddle;
	}

	if (this->solidBlock != NULL) {
		delete this->solidBlock;
	}
}

// Draw a piece of the level (block that you destory or that makes up part of the level
// outline), this is done by positioning it, drawing the correct material and then
// drawing the mesh itself.
void GameAssets::DrawLevelPieces(std::vector<std::vector<LevelPiece*>>& pieces, const Camera& camera) {
	// Go through each piece and draw the basic material
	for (size_t h = 0; h < pieces.size(); h++) {
		for (size_t w = 0; w < pieces[h].size(); w++) {

			LevelPiece* p = pieces[h][w];
			Point2D loc = p->GetCenter();
			
			// Draw the piece with its appropriate transform
			glPushMatrix();
			glTranslatef(loc[0], loc[1], 0);

			switch(p->GetType()) {
				case LevelPiece::RedBreakable:
					{
						this->breakableBlock->SetColour(Colour(1, 0, 0));
						this->breakableBlock->Draw(camera);
					}
					break;			
				case LevelPiece::OrangeBreakable: 
					{
						this->breakableBlock->SetColour(Colour(1, 0.5f, 0));
						this->breakableBlock->Draw(camera);
					}
					break;
				case LevelPiece::YellowBreakable: 
					{
						this->breakableBlock->SetColour(Colour(1, 1, 0));
						this->breakableBlock->Draw(camera);
					}
					break;
				case LevelPiece::GreenBreakable:
					{
						this->breakableBlock->SetColour(Colour(0, 1, 0));
						this->breakableBlock->Draw(camera);
					}
					break;			
				case LevelPiece::Solid:
						this->solidBlock->Draw(camera);
					break;
				case LevelPiece::Bomb:
					// Nothing to draw here yet..
					break;
				default:
					break;
			}

			glPopMatrix();
		}
	}

	// Go through each piece and draw the outlines
	glPushAttrib(GL_ENABLE_BIT || GL_POLYGON_BIT || GL_LINE_BIT || GL_TEXTURE_BIT);
	GameDisplay::SetOutlineRenderAttribs();
	
	for (size_t h = 0; h < pieces.size(); h++) {
		for (size_t w = 0; w < pieces[h].size(); w++) {

			LevelPiece* p = pieces[h][w];
			Point2D loc = p->GetCenter();
			
			// Draw the piece with its appropriate transform
			glPushMatrix();
			glTranslatef(loc[0], loc[1], 0);

			switch(p->GetType()) {
				case LevelPiece::RedBreakable:	
				case LevelPiece::OrangeBreakable: 
				case LevelPiece::YellowBreakable: 
				case LevelPiece::GreenBreakable:
					this->breakableBlock->FastDraw();
					break;
				case LevelPiece::Solid:
					this->solidBlock->FastDraw();
					break;
				case LevelPiece::Bomb:
					// Nothing to draw here yet..
					break;
				default:
					break;
			}
			glPopMatrix();
		}
	}

	glPopAttrib();
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBall(const GameBall& b, const Camera& camera) {
	
	Point2D loc = b.GetBounds().Center();
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	
	// Draw the ball
	this->ball->Draw(camera);
	
	// ... and its outlines
	glPushAttrib(GL_ENABLE_BIT || GL_POLYGON_BIT || GL_LINE_BIT || GL_TEXTURE_BIT);
	GameDisplay::SetOutlineRenderAttribs();
	this->ball->FastDraw();
	glPopAttrib();

	glPopMatrix();
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(const PlayerPaddle& p, const Camera& camera) {
	Point2D paddleCenter = p.GetCenterPosition();	

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1], 0);
	
	// Draw the paddle
	this->playerPaddle->Draw(camera);
	
	// ... and its outlines
	glPushAttrib(GL_ENABLE_BIT || GL_POLYGON_BIT || GL_LINE_BIT || GL_TEXTURE_BIT);
	GameDisplay::SetOutlineRenderAttribs();
	this->playerPaddle->FastDraw();
	glPopAttrib();

	glPopMatrix();
}

/**
 * Draw the background / environment of the world type.
 */
void GameAssets::DrawBackground(const Camera& camera) {
	// Draw the background
	this->background->Draw(camera);

	// ... and its outlines
	glPushAttrib(GL_ENABLE_BIT || GL_POLYGON_BIT || GL_LINE_BIT || GL_TEXTURE_BIT);
	GameDisplay::SetOutlineRenderAttribs(2.5f);
	this->background->FastDraw();
	glPopAttrib();
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

	// Load regular mesh assets
	if (this->ball == NULL) {
		this->ball = ObjReader::ReadMesh(BALL_MESH);
	}
	if (this->breakableBlock == NULL) {
		this->breakableBlock = ObjReader::ReadMesh(BLOCK_MESH);
	}

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

	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ELECTRICZAP, Small);
	assert(temp != NULL);
	this->fonts[ElectricZap][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ELECTRICZAP, Medium);
	assert(temp != NULL);
	this->fonts[ElectricZap][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ELECTRICZAP, Big);
	assert(temp != NULL);
	this->fonts[ElectricZap][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(FONT_ELECTRICZAP, Huge);
	assert(temp != NULL);
	this->fonts[ElectricZap][Huge]	= temp;

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
	this->playerPaddle		= ObjReader::ReadMesh(DECO_PADDLE_MESH);
	this->solidBlock			= ObjReader::ReadMesh(DECO_SOLID_BLOCK_MESH);
	this->background			= ObjReader::ReadMesh(DECO_BACKGROUND_MESH);
}

/**
 * Load in the "Cyberpunk" assets for use in the game.
 */
void GameAssets::LoadCyberpunkStyleAssets() {
	debug_output("Loading cyberpunk style assets");
	
	// Cyberpunk mesh assets
	this->playerPaddle		= ObjReader::ReadMesh(CYBERPUNK_PADDLE_MESH);
	this->solidBlock			=	ObjReader::ReadMesh(CYBERPUNK_SOLID_BLOCK_MESH);
	this->background			= ObjReader::ReadMesh(CYBERPUNK_BACKGROUND_MESH);
}