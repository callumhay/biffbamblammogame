#include "GameAssets.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "DecoSkybox.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"

// Other asset classes
#include "GameWorldAssets.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"

// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

// Includes for Item types
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/InvisiBallItem.h"

const std::string GameAssets::ITEM_MESH						= GameViewConstants::GetInstance()->MESH_DIR + "/item.obj";
const std::string GameAssets::ITEM_LABEL_MATGRP		= "ItemLabel";	// Material group name for changing the label on the item mesh
const std::string GameAssets::ITEM_END_MATGRP			= "ColourEnd";	// Material group name for changing the colour on the item mesh

const Colour GameAssets::ITEM_GOOD_COLOUR					= Colour(0.0f, 0.8f, 0.0f);
const Colour GameAssets::ITEM_BAD_COLOUR					= Colour(0.8f, 0.0f, 0.0f);
const Colour GameAssets::ITEM_NEUTRAL_COLOUR			= Colour(0.0f, 0.6f, 1.0f);

// Item texture assets
const std::string GameAssets::ITEM_SLOWBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/slowball_powerup256x128.jpg";
const std::string GameAssets::ITEM_FASTBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/fastball_powerdown256x128.jpg";
const std::string GameAssets::ITEM_UBERBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/uberball_powerup256x128.jpg";
const std::string GameAssets::ITEM_INVISIBALL_TEXTURE = GameViewConstants::GetInstance()->TEXTURE_DIR + "/invisiball_powerdown256x128.jpg";

const std::string GameAssets::ITEM_TIMER_SLOWBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/slowball_timer_hud128x64.png";
const std::string GameAssets::ITEM_TIMER_FASTBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/fastball_timer_hud128x64.png";
const std::string GameAssets::ITEM_TIMER_UBERBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/uberball_timer_hud256x128.png";
const std::string GameAssets::ITEM_TIMER_INVISIBALL_TEXTURE = GameViewConstants::GetInstance()->TEXTURE_DIR + "/invisiball_timer_hud256x128.png";

const std::string GameAssets::ITEM_TIMER_FILLER_SPDBALL_TEXTURE			= GameViewConstants::GetInstance()->TEXTURE_DIR + "/ballspeed_timer_fill_hud128x64.png";
const std::string GameAssets::ITEM_TIMER_FILLER_UBERBALL_TEXTURE		= GameViewConstants::GetInstance()->TEXTURE_DIR + "/uberball_timer_fill_hud256x128.png";
const std::string GameAssets::ITEM_TIMER_FILLER_INVISIBALL_TEXTURE	= GameViewConstants::GetInstance()->TEXTURE_DIR + "/invisiball_timer_fill_hud256x128.png";

// *****************************************************

GameAssets::GameAssets(): worldAssets(NULL),
ball(NULL), spikeyBall(NULL), item(NULL), levelMesh(NULL), invisiBallEffect(NULL) {

	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	
	// Load regular fonts
	this->LoadRegularFontAssets();

	// Load regular meshes
	this->LoadRegularMeshAssets();

	// Load regular effects
	this->LoadRegularEffectAssets();
}

GameAssets::~GameAssets() {
	// Delete regular mesh assets
	this->DeleteRegularMeshAssets();

	// Delete regular effect assets
	this->DeleteRegularEffectAssets();

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
	if (this->worldAssets != NULL) {
		delete this->worldAssets;
		this->worldAssets = NULL;
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
	if (this->spikeyBall != NULL) {
		delete this->spikeyBall;
		this->spikeyBall = NULL;
	}
	if (this->item != NULL) {
		this->item->SetTextureForMaterial(GameAssets::ITEM_LABEL_MATGRP, NULL);	// Make sure there is not texture assoc with item or we will delete it twice!!
		delete this->item;
		this->item = NULL;
		this->UnloadItemTextures();
	}
}



// Draw the foreground level pieces...
void GameAssets::DrawLevelPieces(const Camera& camera) const {
	this->levelMesh->Draw(camera);
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBall(const GameBall& b, const Camera& camera, Texture2D* sceneTex) const {
	
	Point2D loc = b.GetBounds().Center();
	glPushMatrix();
	glTranslatef(loc[0], loc[1], 0);
	Vector3D ballRot = b.GetRotation();
	glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);

	CgFxEffectBase* ballInvisiEffectTemp = NULL;
	// Deal with the Invisiball power-down if applicable....
	if ((b.GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall) {
		this->invisiBallEffect->SetFBOTexture(sceneTex);
		this->invisiBallEffect->SetWarpAmountParam(50.0f);
		this->invisiBallEffect->SetIndexOfRefraction(1.33f);
		ballInvisiEffectTemp = this->invisiBallEffect;
	}

	// Draw the ball based on its type...
	if ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		this->spikeyBall->Draw(camera, ballInvisiEffectTemp);
	}
	else {
		this->ball->Draw(camera, ballInvisiEffectTemp);
	}

	glPopMatrix();
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(const PlayerPaddle& p, const Camera& camera) const {
	this->worldAssets->DrawPaddle(p, camera);
}

/**
 * Draw the background / environment of the world type.
 */
void GameAssets::DrawBackground(double dT, const Camera& camera) {
	this->worldAssets->DrawBackground(dT, camera);
}

/**
 * Draw any particles present in the game currently.
 */
void GameAssets::DrawParticleEffects(double dT, const Camera& camera) {
	
	// Go through all the particles and do book keeping and drawing
	for (std::list<ESPEmitter*>::const_iterator iter = this->activeParticleEmitters.begin();
		iter != this->activeParticleEmitters.end(); iter++) {
	
		ESPEmitter* curr = *iter;

		// Check to see if dead, if so erase it...
		if (curr->IsDead()) {	
			iter = this->activeParticleEmitters.erase(iter);
			delete curr;

			if (this->activeParticleEmitters.size() == 0) {
				break;
			}
		}
		else {
			// Not dead...
			curr->Draw(camera);
			curr->Tick(dT);
		}
	}

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
void GameAssets::DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight) {

	// Spacing along the vertical between timer graphics
	unsigned int TIMER_Y_SPACING_PX = 5;
	// Spacing along the horizontal of timer graphics distanced from the right edge of the game window
 	unsigned int TIMER_X_SPACING_PX = displayWidth / 25;
	// Width of timer graphic
	unsigned int TIMER_WIDTH_PX			= (displayWidth / 5) - (2 * TIMER_X_SPACING_PX);
	// Tracked height at which to draw the next timer graphic
	unsigned int currHeight					= 2 * displayHeight / 3;

	// Go through each timer and draw its appropriate graphic and current elapsed 'filler'
	for(std::list<GameItemTimer*>::const_iterator allTimerIter = timers.begin(); allTimerIter != timers.end(); allTimerIter++) {
		const GameItemTimer* timer = *allTimerIter;

		// Check to see if a timer exists, if so then draw it
		std::map<std::string, Texture2D*>::iterator tempIterTimer = this->itemTimerTextures.find(timer->GetTimerItemName());
		std::map<std::string, Texture2D*>::iterator tempIterFiller = this->itemTimerFillerTextures.find(timer->GetTimerItemName());
		
		if (tempIterTimer != this->itemTimerTextures.end() && tempIterFiller != this->itemTimerFillerTextures.end()) {
			
			// Draw the timer
			Texture2D* timerTex		= tempIterTimer->second;
			Texture2D* fillerTex	= tempIterFiller->second;
			
			assert(timerTex != NULL);
			assert(fillerTex != NULL);
			
			unsigned int width	= TIMER_WIDTH_PX;
			unsigned int height = width * timerTex->GetHeight() / timerTex->GetWidth();

			// Prepare OGL for drawing the timer
			glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
			
			// Make world coordinates equal window coordinates
			Camera::PushWindowCoords();
			
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
			fillerTex->UnbindTexture();

			// TODO: make draw lists to speed this up...

			timerTex->BindTexture();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2f(0, 0);
			glTexCoord2d(1, 0); glVertex2f(width, 0);
			glTexCoord2d(1, 1); glVertex2f(width, height);
			glTexCoord2d(0, 1); glVertex2f(0, height);
			glEnd();
			timerTex->UnbindTexture();

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
	Texture2D* uberBallItemTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_UBERBALL_TEXTURE, Texture::Trilinear);
	Texture2D* invisiBallItemTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_INVISIBALL_TEXTURE, Texture::Trilinear);

	assert(slowBallItemTex		!= NULL);
	assert(fastBallItemTex		!= NULL);
	assert(uberBallItemTex		!= NULL);
	assert(invisiBallItemTex	!= NULL);

	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,		slowBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,		fastBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,			uberBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME, invisiBallItemTex));

	// Load the timer textures
	Texture2D* slowBallTimerTex		= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_SLOWBALL_TEXTURE,		Texture::Trilinear);
	Texture2D* fastBallTimerTex		= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FASTBALL_TEXTURE,		Texture::Trilinear);
	Texture2D* uberBallTimerTex		= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_UBERBALL_TEXTURE,		Texture::Trilinear);
	Texture2D* invisiBallTimerTex	= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_INVISIBALL_TEXTURE,	Texture::Trilinear);

	assert(slowBallTimerTex		!= NULL);
	assert(fastBallTimerTex		!= NULL);
	assert(uberBallTimerTex		!= NULL);
	assert(invisiBallTimerTex != NULL);

	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,			slowBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,			fastBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,			uberBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,	invisiBallTimerTex));

	// Load the fillers for the timer textures (used to make the timer look like its ticking down)
	Texture2D* ballSpdTimerFillerTex		= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FILLER_SPDBALL_TEXTURE,		Texture::Trilinear);
	Texture2D* uberBallTimerFillerTex		= Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FILLER_UBERBALL_TEXTURE,		Texture::Trilinear);
	Texture2D* invisiBallTimerFillerTex = Texture2D::CreateTexture2DFromImgFile(GameAssets::ITEM_TIMER_FILLER_INVISIBALL_TEXTURE, Texture::Trilinear);
	
	assert(ballSpdTimerFillerTex		!= NULL);
	assert(uberBallTimerFillerTex		!= NULL);
	assert(invisiBallTimerFillerTex != NULL);

	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,			ballSpdTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,			ballSpdTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,			uberBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,	invisiBallTimerFillerTex));
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ObjReader::ReadMesh(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ObjReader::ReadMesh(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->item == NULL) {
		// Load the mesh for items
		this->item = ObjReader::ReadMesh(ITEM_MESH);
		// Load all of the possible item labels
		this->LoadItemTextures();
	}
}

void GameAssets::LoadRegularEffectAssets() {
	this->invisiBallEffect = new CgFxPostRefract();
}

void GameAssets::DeleteRegularEffectAssets() {
	// Delete the invisiball effect
	if (this->invisiBallEffect != NULL) {
		delete this->invisiBallEffect;
		this->invisiBallEffect = NULL;
	}

	// Delete any left behind particles
	for(std::list<ESPEmitter*>::iterator iter = this->activeParticleEmitters.begin(); iter != this->activeParticleEmitters.end(); iter++) {
		ESPEmitter* currEmitter = *iter;
		delete currEmitter;
		currEmitter = NULL;
	}
	this->activeParticleEmitters.clear();
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
	std::map<std::string, Texture2D*>::iterator iter2 = this->itemTimerTextures.begin();
	for (; iter2 != this->itemTimerTextures.end(); iter2++) {
		delete iter2->second;
		iter2->second = NULL;
	}
	this->itemTimerTextures.clear();

	// Unload timer filler textures
	std::map<std::string, Texture2D*>::iterator iter3 = this->itemTimerFillerTextures.begin();
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
	// Load up the new set of assets
	this->worldAssets = GameWorldAssets::CreateWorldAssets(style);
}

/**
 * Load the given level as a mesh.
 */
void GameAssets::LoadLevelAssets(const GameLevel* level) {
	assert(level != NULL);
	assert(this->worldAssets != NULL);

	// Delete all previously loaded level-related assets
	this->DeleteLevelAssets();
	// Load the given level
	this->levelMesh = LevelMesh::CreateLevelMesh(this->worldAssets, level);
}

/**
 * Load the regular game fonts - these are always in memory since they are used
 * throughout the game in various places.
 */
void GameAssets::LoadRegularFontAssets() {
	debug_output("Loading regular font sets");

	TextureFontSet* temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_GUNBLAM, Small);
	assert(temp != NULL);
	this->fonts[GunBlam][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_GUNBLAM, Medium);
	assert(temp != NULL);
	this->fonts[GunBlam][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_GUNBLAM, Big);
	assert(temp != NULL);
	this->fonts[GunBlam][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_GUNBLAM, Huge);
	assert(temp != NULL);
	this->fonts[GunBlam][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Small);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Medium);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Big);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Huge);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Small);
	assert(temp != NULL);
	this->fonts[ElectricZap][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Medium);
	assert(temp != NULL);
	this->fonts[ElectricZap][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Big);
	assert(temp != NULL);
	this->fonts[ElectricZap][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Huge);
	assert(temp != NULL);
	this->fonts[ElectricZap][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Small);
	assert(temp != NULL);
	this->fonts[AllPurpose][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Medium);
	assert(temp != NULL);
	this->fonts[AllPurpose][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Big);
	assert(temp != NULL);
	this->fonts[AllPurpose][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Huge);
	assert(temp != NULL);
	this->fonts[AllPurpose][Huge]	= temp; 
}