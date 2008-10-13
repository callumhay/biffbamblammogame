#include "GameAssets.h"
#include "Mesh.h"
#include "ObjReader.h"
#include "TextureFontSet.h"
#include "CgShaderManager.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "Texture2D.h"

#include "Skybox.h"
#include "DecoSkybox.h"

// Includes for Item types
#include "../GameModel/SlowBallItem.h"
#include "../GameModel/FastBallItem.h"

#include "../Utils/Includes.h"

// Asset file path constants ***************************
const std::string GameAssets::RESOURCE_DIR	= "resources";
const std::string GameAssets::FONT_DIR			= "fonts";
const std::string GameAssets::MESH_DIR			= "models";
const std::string GameAssets::SHADER_DIR		= "shaders";
const std::string GameAssets::TEXTURE_DIR		= "textures";

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
const std::string GameAssets::SKYBOX_MESH	= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/skybox.obj";

const std::string GameAssets::ITEM_MESH						= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/item.obj";
const std::string GameAssets::ITEM_LABEL_MATGRP		= "ItemLabel";	// Material group name for changing the label on the item mesh
const std::string GameAssets::ITEM_END_MATGRP			= "ColourEnd";	// Material group name for changing the colour on the item mesh
const Colour GameAssets::ITEM_GOOD_COLOUR					= Colour(0.0f, 0.8f, 0.0f);
const Colour GameAssets::ITEM_BAD_COLOUR					= Colour(0.8f, 0.0f, 0.0f);
const Colour GameAssets::ITEM_NEUTRAL_COLOUR			= Colour(0.0f, 0.6f, 1.0f);

const std::string GameAssets::ITEM_SLOWBALL_TEXTURE	= GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/slowball_powerup512x128.jpg";
const std::string GameAssets::ITEM_FASTBALL_TEXTURE	= GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/fastball_powerdown512x128.jpg";;

const std::string GameAssets::ITEM_TIMER_SLOWBALL_TEXTURE	= GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/slowball_timer_hud128x64.png";
const std::string GameAssets::ITEM_TIMER_FASTBALL_TEXTURE	= GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/fastball_timer_hud128x64.png";

const std::string GameAssets::ITEM_TIMER_FILLER_SPDBALL_TEXTURE	= GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/ballspeed_timer_fill_hud128x64.png";

// Deco assets
const std::string GameAssets::DECO_PADDLE_MESH						= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/deco_paddle.obj";
const std::string GameAssets::DECO_BACKGROUND_MESH				= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/deco_background.obj";

// Cyberpunk assets
const std::string GameAssets::CYBERPUNK_PADDLE_MESH						= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/cyberpunk_paddle.obj";
const std::string GameAssets::CYBERPUNK_BACKGROUND_MESH				= GameAssets::RESOURCE_DIR + "/" + MESH_DIR + "/cyberpunk_background.obj";
const std::string GameAssets::CYBERPUNK_SKYBOX_TEXTURES[6]		= {
	GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg", GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg",
	GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg", GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg",
	GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg", GameAssets::RESOURCE_DIR + "/" + TEXTURE_DIR + "/deco_spirals1024x1024.jpg"
};

// *****************************************************

GameAssets::GameAssets(): ball(NULL), item(NULL), playerPaddle(NULL), skybox(NULL), background(NULL), levelMesh(NULL), currLoadedStyle(GameWorld::None) {
	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	
	// Load regular fonts
	this->LoadRegularFontAssets();

	// Load regular meshes
	this->LoadRegularMeshAssets();
}

GameAssets::~GameAssets() {
	// Delete regular mesh assets
	this->DeleteRegularMeshAssets();

	// Delete the currently loaded world and level assets if there are any
	this->DeleteWorldAssets();
	this->DeleteLevelAssets();

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
 * Delete any previously loaded assets related to the world.
 */
void GameAssets::DeleteWorldAssets() {
	if (this->playerPaddle != NULL) {
		delete this->playerPaddle;
		this->playerPaddle = NULL;
	}
	if (this->skybox != NULL) {
		delete this->skybox;
		this->skybox = NULL;
	}
	if (this->background != NULL) {
		delete this->background;
		this->background = NULL;
	}
}

/*
 * Delete any previously loaded assets related to the level.
 */
void GameAssets::DeleteLevelAssets() {
	if (this->levelMesh != NULL) {
		delete this->levelMesh;
		this->levelMesh = NULL;
	}
}

/**
 * Delete any previously loaded regular assets.
 */
void GameAssets::DeleteRegularMeshAssets() {
	if (this->ball != NULL) {
		delete this->ball;
		this->ball = NULL;
	}
	if (this->item != NULL) {
		this->item->SetTextureForMaterial(GameAssets::ITEM_LABEL_MATGRP, NULL);	// Make sure there is not texture assoc with item or we will delete it twice!!
		delete this->item;
		this->item = NULL;
		this->UnloadItemTextures();
	}
}

// Draw a piece of the level (block that you destory or that makes up part of the level
// outline), this is done by positioning it, drawing the correct material and then
// drawing the mesh itself.
void GameAssets::DrawLevelPieces(const Camera& camera) const {
	this->levelMesh->Draw(camera);
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBall(const GameBall& b, const Camera& camera) const {
	
	Point2D loc = b.GetBounds().Center();
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	
	// Draw the ball
	this->ball->Draw(camera);

	glPopMatrix();
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(const PlayerPaddle& p, const Camera& camera) const {
	Point2D paddleCenter = p.GetCenterPosition();	

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1], 0);
	
	// Draw the paddle
	this->playerPaddle->Draw(camera);
	
	glPopMatrix();
}

/**
 * Draw the background / environment of the world type.
 */
void GameAssets::DrawBackground(double dT, const Camera& camera) const {
	// Draw the skybox
	this->skybox->Draw(dT, camera);
	
	// Draw the background
	this->background->Draw(camera);
}

/**
 * Draw a given item in the world.
 */
void GameAssets::DrawItem(const GameItem& gameItem, const Camera& camera) const {
	Point2D center = gameItem.GetCenter();
	glPushMatrix();
	glTranslatef(center[0], center[1], 0.0f);

	// Set material for the image based on the item name/type
	std::string itemName	= gameItem.GetName();
	std::map<std::string, Texture2D*>::const_iterator lookupIter = this->itemTextures.find(itemName);
	assert(lookupIter != this->itemTextures.end());
	Texture2D* itemTexture = lookupIter->second;
	assert(itemTexture != NULL);
	this->item->SetTextureForMaterial(GameAssets::ITEM_LABEL_MATGRP, itemTexture);
	
	Colour itemEndColour;
	switch (gameItem.GetItemType()) {
		case GameItem::Good :
			itemEndColour = GameAssets::ITEM_GOOD_COLOUR;
			break;
		case GameItem::Bad :
			itemEndColour = GameAssets::ITEM_BAD_COLOUR;
			break;		
		case GameItem::Neutral:
			itemEndColour = GameAssets::ITEM_NEUTRAL_COLOUR;
			break;
		default:
			assert(false);
			break;
	}
	this->item->SetColourForMaterial(GameAssets::ITEM_END_MATGRP, itemEndColour);
	
	// Draw the item
	this->item->Draw(camera);
	glPopMatrix();
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameAssets::DrawTimers(const std::vector<GameItemTimer*>& timers, int displayWidth, int displayHeight) {

	// Spacing along the vertical between timer graphics
	unsigned int TIMER_Y_SPACING_PX = 5;
	// Spacing along the horizontal of timer graphics distanced from the right edge of the game window
 	unsigned int TIMER_X_SPACING_PX = displayWidth / 25;
	// Width of timer graphic
	unsigned int TIMER_WIDTH_PX			= (displayWidth / 5) - (2 * TIMER_X_SPACING_PX);
	// Tracked height at which to draw the next timer graphic
	unsigned int currHeight					= 2 * displayHeight / 3;

	// Go through each timer and draw its appropriate graphic and current elapsed 'filler'
	for(std::vector<GameItemTimer*>::const_iterator allTimerIter = timers.begin(); allTimerIter != timers.end(); allTimerIter++) {
		const GameItemTimer* timer = *allTimerIter;

		// Check to see if a timer exists, if so then draw it
		std::map<GameItemTimer::TimerType, Texture2D*>::iterator tempIterTimer = this->itemTimerTextures.find(timer->GetTimerType());
		std::map<GameItemTimer::TimerType, Texture2D*>::iterator tempIterFiller = this->itemTimerFillerTextures.find(timer->GetTimerType());
		
		if (tempIterTimer != this->itemTimerTextures.end() && tempIterFiller != this->itemTimerFillerTextures.end()) {
			
			// Draw the timer
			Texture2D* timerTex		= tempIterTimer->second;
			Texture2D* fillerTex	= tempIterFiller->second;
			
			assert(timerTex != NULL);
			assert(fillerTex != NULL);
			
			unsigned int width	= TIMER_WIDTH_PX;
			unsigned int height = width * timerTex->GetHeight() / timerTex->GetWidth();

			// Make world coordinates equal window coordinates
			Camera::PushWindowCoords();

			// Prepare OGL for drawing the timer
			glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
			glMatrixMode(GL_MODELVIEW);
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

			// Draw the timer...
			glPushMatrix();
			glLoadIdentity();

			// Do the transformation to setup the start location for where to draw the
			// timer graphics (right-hand side of the screen)

			glTranslatef(displayWidth - width - TIMER_X_SPACING_PX, currHeight, 0.0f);

			// Draw the filler for the timer (how much has elapsed)
			float percentElapsed = timer->GetPercentTimeElapsed();
			float fillerHeight    = height - (height * percentElapsed);
			float fillerTexHeight = 1 - percentElapsed;
			
			// Back fill - so that the filler that has expired so far stands out
			// from the background
			fillerTex->BindTexture();
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2f(0, 0);
			glTexCoord2d(1, 0); glVertex2f(width, 0);
			glTexCoord2d(1, 1); glVertex2f(width, height);
			glTexCoord2d(0, 1); glVertex2f(0, height);
			glEnd();

			// Figure out what colour to make the fill based on how it
			// affects the player (red is bad, green is good, etc.)
			GameItem::ItemType itemDisposition = timer->GetTimerDisposition();
			switch (itemDisposition) {
				case GameItem::Good:
					glColor4f(ITEM_GOOD_COLOUR.R(), ITEM_GOOD_COLOUR.G(), ITEM_GOOD_COLOUR.B(), 1.0f);	// Deep Green
					break;
				case GameItem::Bad:
					glColor4f(ITEM_BAD_COLOUR.R(), ITEM_BAD_COLOUR.G(), ITEM_BAD_COLOUR.B(), 1.0f);	// Deep Red
					break;
				case GameItem::Neutral:
					glColor4f(ITEM_NEUTRAL_COLOUR.R(), ITEM_NEUTRAL_COLOUR.G(), ITEM_NEUTRAL_COLOUR.B(), 1.0f);	// Light Blue
					break;
				default:
					assert(false);
					break;
			}
			
			// The actual filler, which is constantly decreasing to zero
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2f(0, 0);
			glTexCoord2d(1, 0); glVertex2f(width, 0);
			glTexCoord2d(1, fillerTexHeight); glVertex2f(width, fillerHeight);
			glTexCoord2d(0, fillerTexHeight); glVertex2f(0, fillerHeight);
			glEnd();

			// TODO: make draw lists to speed this up...

			timerTex->BindTexture();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2f(0, 0);
			glTexCoord2d(1, 0); glVertex2f(width, 0);
			glTexCoord2d(1, 1); glVertex2f(width, height);
			glTexCoord2d(0, 1); glVertex2f(0, height);
			glEnd();

			glPopMatrix();
			glPopAttrib();  

			// Pop the projection matrix
			Camera::PopWindowCoords();

			currHeight -= (height + TIMER_Y_SPACING_PX);
		}
		else {
			assert(false);
		}
	}
}

/**
 * Function for initializing the map for looking up item textures
 * as well as timer textures.
 */
void GameAssets::LoadItemTextures() {
	// Load the item textures
	Texture2D* slowBallItemTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_SLOWBALL_TEXTURE, Texture::Trilinear);
	Texture2D* fastBallItemTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_FASTBALL_TEXTURE, Texture::Trilinear);

	assert(slowBallItemTex != NULL);
	assert(fastBallItemTex != NULL);

	this->itemTextures.insert(std::pair<std::string, Texture2D*>(SlowBallItem::SLOW_BALL_ITEM_NAME, slowBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(FastBallItem::FAST_BALL_ITEM_NAME, fastBallItemTex));

	// Load the timer textures
	Texture2D* slowBallTimerTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_SLOWBALL_TEXTURE, Texture::Trilinear);
	Texture2D* fastBallTimerTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FASTBALL_TEXTURE, Texture::Trilinear);

	assert(slowBallTimerTex != NULL);
	assert(fastBallTimerTex != NULL);

	this->itemTimerTextures.insert(std::pair<GameItemTimer::TimerType, Texture2D*>(GameItemTimer::SlowBallTimer, slowBallTimerTex));
	this->itemTimerTextures.insert(std::pair<GameItemTimer::TimerType, Texture2D*>(GameItemTimer::FastBallTimer, fastBallTimerTex));

	// Load the fillers for the timer textures (used to make the timer look like its ticking down)
	Texture2D* ballSpdTimerFillerTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FILLER_SPDBALL_TEXTURE, Texture::Trilinear);
	
	assert(ballSpdTimerFillerTex != NULL);

	this->itemTimerFillerTextures.insert(std::pair<GameItemTimer::TimerType, Texture2D*>(GameItemTimer::SlowBallTimer, ballSpdTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<GameItemTimer::TimerType, Texture2D*>(GameItemTimer::FastBallTimer, ballSpdTimerFillerTex));
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ObjReader::ReadMesh(BALL_MESH);
	}
	if (this->item == NULL) {
		// Load the mesh for items
		this->item = ObjReader::ReadMesh(ITEM_MESH);
		// Load all of the possible item labels
		this->LoadItemTextures();
	}
}

/**
 * Function for unloading all the item textures.
 */
void GameAssets::UnloadItemTextures() {
	// Unload item textures
	std::map<std::string, Texture2D*>::iterator iter1 = this->itemTextures.begin();
	for (; iter1 != this->itemTextures.end(); iter1++) {
		delete iter1->second;
		iter1->second = NULL;
	}
	this->itemTextures.clear();

	// Unload timer textures
	std::map<GameItemTimer::TimerType, Texture2D*>::iterator iter2 = this->itemTimerTextures.begin();
	for (; iter2 != this->itemTimerTextures.end(); iter2++) {
		delete iter2->second;
		iter2->second = NULL;
	}
	this->itemTimerTextures.clear();

	// Unload timer filler textures
	std::map<GameItemTimer::TimerType, Texture2D*>::iterator iter3 = this->itemTimerFillerTextures.begin();
	for (; iter3 != this->itemTimerFillerTextures.end(); iter3++) {
		if (iter3->second != NULL) {
			delete iter3->second;
			iter3->second = NULL;
		}
	}
	this->itemTimerFillerTextures.clear();
}

/*
 * This will load a set of assets for use in a game based off a
 * given world-style, after loading all assets will be available for use
 * in-game.
 * Precondition: true.
 */
void GameAssets::LoadWorldAssets(GameWorld::WorldStyle style) {
	
	// Delete all previously loaded style-related assets
	this->DeleteWorldAssets();

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
 * Load the given level as a mesh.
 */
void GameAssets::LoadLevelAssets(GameWorld::WorldStyle worldStyle, const GameLevel* level) {
	assert(level != NULL);

	// Delete all previously loaded level-related assets
	this->DeleteLevelAssets();
	
	// Load the given level
	this->levelMesh = LevelMesh::CreateLevelMesh(worldStyle, level);
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
	this->background			= ObjReader::ReadMesh(DECO_BACKGROUND_MESH);
	this->skybox					= DecoSkybox::CreateDecoSkybox(SKYBOX_MESH);
}

/**
 * Load in the "Cyberpunk" assets for use in the game.
 */
void GameAssets::LoadCyberpunkStyleAssets() {
	debug_output("Loading cyberpunk style assets");
	
	// Cyberpunk mesh assets
	this->playerPaddle		= ObjReader::ReadMesh(CYBERPUNK_PADDLE_MESH);
	this->background			= ObjReader::ReadMesh(CYBERPUNK_BACKGROUND_MESH);
	this->skybox					= Skybox::CreateSkybox(SKYBOX_MESH, CYBERPUNK_SKYBOX_TEXTURES);
}