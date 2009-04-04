#include "GameItemAssets.h"
#include "GameViewConstants.h"
#include "GameESPAssets.h"

// Blammo Engine Includes
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Texture3D.h"

// Game Model Includes
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/InvisiBallItem.h"
#include "../GameModel/GhostBallItem.h"
#include "../GameModel/LaserPaddleItem.h"
#include "../GameModel/MultiBallItem.h"
#include "../GameModel/PaddleSizeItem.h"
#include "../GameModel/BallSizeItem.h"

GameItemAssets::GameItemAssets(GameESPAssets* espAssets) : 
espAssets(espAssets), item(NULL) {
}

GameItemAssets::~GameItemAssets() {
	// Unload all the item assets...
	this->UnloadItemTextures();
	this->UnloadItemMeshes();
}

/**
 * Private helper function for unloading item-related textures.
 */
void GameItemAssets::UnloadItemTextures() {
	// Unload drop item textures
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
		Texture2D* currTex = iter3->second;
		if (currTex != NULL) {
			delete currTex;
			currTex = NULL;
		}
	}
	this->itemTimerFillerTextures.clear();
}

/**
 * Private helper function for unloading item-related mesh assets.
 */
void GameItemAssets::UnloadItemMeshes() {
	if (this->item != NULL) {
		// Make sure there is not texture assoc with item or we will delete it twice!!
		this->item->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, NULL);

		delete this->item;
		this->item = NULL;
	}
}

/**
 * Private helper function for loading item-related texture assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemTextures() {
	debug_output("Loading Item Textures...");

	// Load the item textures
	Texture2D* slowBallItemTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_SLOWBALL,			Texture::Trilinear);
	Texture2D* fastBallItemTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_FASTBALL,			Texture::Trilinear);
	Texture2D* uberBallItemTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_UBERBALL,			Texture::Trilinear);
	Texture2D* invisiBallItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_INVISIBALL,		Texture::Trilinear);
	Texture2D* ghostBallItemTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_GHOSTBALL,			Texture::Trilinear);
	Texture2D* laserPaddleItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLELASER,		Texture::Trilinear);
	Texture2D* multiBall3ItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_MULTIBALL3,		Texture::Trilinear);
	Texture2D* multiBall5ItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_MULTIBALL5,		Texture::Trilinear);
	Texture2D* paddleGrowItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLEGROW,		Texture::Trilinear);
	Texture2D* paddleShrinkItemTex	= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLESHRINK,	Texture::Trilinear);
	Texture2D* ballGrowItemTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_BALLGROW,			Texture::Trilinear);
	Texture2D* ballShrinkItemTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_BALLSHRINK,		Texture::Trilinear);

	assert(slowBallItemTex			!= NULL);
	assert(fastBallItemTex			!= NULL);
	assert(uberBallItemTex			!= NULL);
	assert(invisiBallItemTex		!= NULL);
	assert(ghostBallItemTex			!= NULL);
	assert(laserPaddleItemTex		!= NULL);
	assert(multiBall3ItemTex		!= NULL);
	assert(multiBall5ItemTex		!= NULL);
	assert(paddleGrowItemTex		!= NULL);
	assert(paddleShrinkItemTex	!= NULL);
	assert(ballGrowItemTex			!= NULL);
	assert(ballShrinkItemTex		!= NULL);

	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,			slowBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,			fastBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,				uberBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,		invisiBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(GhostBallItem::GHOST_BALL_ITEM_NAME,			ghostBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,	laserPaddleItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(MultiBallItem::MULTI3_BALL_ITEM_NAME,		multiBall3ItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(MultiBallItem::MULTI5_BALL_ITEM_NAME,		multiBall5ItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(PaddleSizeItem::PADDLE_GROW_ITEM_NAME,		paddleGrowItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME,	paddleShrinkItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSizeItem::BALL_GROW_ITEM_NAME,				ballGrowItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture2D*>(BallSizeItem::BALL_SHRINK_ITEM_NAME,			ballShrinkItemTex));

	// Load the timer textures
	Texture2D* slowBallTimerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_SLOWBALL,		Texture::Trilinear);
	Texture2D* fastBallTimerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FASTBALL,		Texture::Trilinear);
	Texture2D* uberBallTimerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_UBERBALL,		Texture::Trilinear);
	Texture2D* invisiBallTimerTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_INVISIBALL,	Texture::Trilinear);
	Texture2D* ghostBallTimerTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_GHOSTBALL,		Texture::Trilinear);
	Texture2D* laserPaddleTimerTex	= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_PADDLELASER,	Texture::Trilinear);

	assert(slowBallTimerTex			!= NULL);
	assert(fastBallTimerTex			!= NULL);
	assert(uberBallTimerTex			!= NULL);
	assert(invisiBallTimerTex		!= NULL);
	assert(ghostBallTimerTex		!= NULL);
	assert(laserPaddleTimerTex	!= NULL);

	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,				slowBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,				fastBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,				uberBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,		invisiBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(GhostBallItem::GHOST_BALL_ITEM_NAME,			ghostBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture2D*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,	laserPaddleTimerTex));

	// Load the fillers for the timer textures (used to make the timer look like its ticking down)
	Texture2D* ballSlowTimerFillerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_SPDBALL,		 Texture::Trilinear);
	Texture2D* ballFastTimerFillerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_SPDBALL,		 Texture::Trilinear);
	Texture2D* uberBallTimerFillerTex			= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_UBERBALL,		 Texture::Trilinear);
	Texture2D* invisiBallTimerFillerTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_INVISIBALL,  Texture::Trilinear);
	Texture2D* ghostBallTimerFillerTex		= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL,	 Texture::Trilinear);
	Texture2D* laserPaddleTimerFillerTex	= Texture2D::CreateTexture2DFromImgFile(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_PADDLELASER, Texture::Trilinear);

	assert(ballSlowTimerFillerTex			!= NULL);
	assert(ballFastTimerFillerTex			!= NULL);
	assert(uberBallTimerFillerTex			!= NULL);
	assert(invisiBallTimerFillerTex		!= NULL);
	assert(ghostBallTimerFillerTex		!= NULL);
	assert(laserPaddleTimerFillerTex	!= NULL);

	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,				ballSlowTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(BallSpeedItem::FAST_BALL_ITEM_NAME,				ballFastTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(UberBallItem::UBER_BALL_ITEM_NAME,				uberBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,		invisiBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(GhostBallItem::GHOST_BALL_ITEM_NAME,			ghostBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture2D*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,	laserPaddleTimerFillerTex));

	return true;
}

/**
 * Private helper function for loading item-related mesh assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemMeshes() {
		if (this->item == NULL) {
			// Load the mesh for items
			this->item = ObjReader::ReadMesh(GameViewConstants::GetInstance()->ITEM_MESH);
		}

		if (this->item == NULL) {
			return false;
		}
		return true;
}

/**
 * Public function for loading all the item assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemAssets() {
	bool DidLoadTextures = this->LoadItemTextures();
	bool DidLoadMeshes	 = this->LoadItemMeshes();

	return DidLoadTextures && DidLoadMeshes;
}

/**
 * Draw the given item as a item drop in-game.
 */
void GameItemAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem) const  {
	Point2D center = gameItem.GetCenter();
	glPushMatrix();
	glTranslatef(center[0], center[1], 0.0f);

	// Set material for the image based on the item name/type
	std::string itemName	= gameItem.GetName();
	std::map<std::string, Texture2D*>::const_iterator lookupIter = this->itemTextures.find(itemName);
	assert(lookupIter != this->itemTextures.end());
	Texture2D* itemTexture = lookupIter->second;
	assert(itemTexture != NULL);
	
	this->item->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_LABEL_MATGRP, itemTexture);
	
	Colour itemEndColour;
	switch (gameItem.GetItemType()) {
		case GameItem::Good :
			itemEndColour = GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR;
			break;
		case GameItem::Bad :
			itemEndColour = GameViewConstants::GetInstance()->ITEM_BAD_COLOUR;
			break;		
		case GameItem::Neutral:
			itemEndColour = GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR;
			break;
		default:
			assert(false);
			break;
	}

	this->item->SetColourForMaterial(GameViewConstants::GetInstance()->ITEM_END_MATGRP, itemEndColour);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Draw the item's effect and the item itself
	this->espAssets->DrawItemDropEffects(dT, camera, gameItem);
	this->item->Draw(camera);
	glPopMatrix();
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameItemAssets::DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight) {

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
		assert(timer != NULL);

		// If the timer has already expired then we don't draw it
		if (timer->HasExpired()) {
			continue;
		}

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
			glBlendEquation(GL_MULT);

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
					glColor4f(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R(), 
									  GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G(), 
										GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B(), 1.0f);
					break;
				case GameItem::Bad:
					glColor4f(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.R(), 
									  GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.G(), 
										GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.B(), 1.0f);
					break;
				case GameItem::Neutral:
					glColor4f(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.R(), 
										GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.G(), 
										GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B(), 1.0f);	// Light Blue
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