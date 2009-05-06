#include "GameItemAssets.h"
#include "GameViewConstants.h"
#include "GameESPAssets.h"

// Blammo Engine Includes
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/ResourceManager.h"

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
#include "../GameModel/BlackoutItem.h"
#include "../GameModel/UpsideDownItem.h"
#include "../GameModel/BallSafetyNetItem.h"
#include "../GameModel/OneUpItem.h"

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
	std::map<std::string, Texture*>::iterator iter1 = this->itemTextures.begin();
	for (; iter1 != this->itemTextures.end(); iter1++) {
		bool loadResult = ResourceManager::GetInstance()->ReleaseTextureResource(iter1->second);
		assert(loadResult);
	}
	this->itemTextures.clear();

	// Unload timer textures
	std::map<std::string, Texture*>::iterator iter2 = this->itemTimerTextures.begin();
	for (; iter2 != this->itemTimerTextures.end(); iter2++) {
		bool loadResult = ResourceManager::GetInstance()->ReleaseTextureResource(iter2->second);
		assert(loadResult);
	}
	this->itemTimerTextures.clear();

	// Unload timer filler textures
	std::map<std::string, Texture*>::iterator iter3 = this->itemTimerFillerTextures.begin();
	for (; iter3 != this->itemTimerFillerTextures.end(); iter3++) {
		bool loadResult = ResourceManager::GetInstance()->ReleaseTextureResource(iter3->second);
		assert(loadResult);
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
	}
}

/**
 * Private helper function for loading item-related texture assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemTextures() {
	debug_output("Loading Item Textures...");

	// Load the item textures
	Texture* slowBallItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_SLOWBALL,			Texture::Trilinear);
	Texture* fastBallItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_FASTBALL,			Texture::Trilinear);
	Texture* uberBallItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_UBERBALL,			Texture::Trilinear);
	Texture* invisiBallItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_INVISIBALL,		Texture::Trilinear);
	Texture* ghostBallItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_GHOSTBALL,			Texture::Trilinear);
	Texture* laserPaddleItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLELASER,		Texture::Trilinear);
	Texture* multiBall3ItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_MULTIBALL3,		Texture::Trilinear);
	Texture* multiBall5ItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_MULTIBALL5,		Texture::Trilinear);
	Texture* paddleGrowItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLEGROW,		Texture::Trilinear);
	Texture* paddleShrinkItemTex	= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_PADDLESHRINK,	Texture::Trilinear);
	Texture* ballGrowItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_BALLGROW,			Texture::Trilinear);
	Texture* ballShrinkItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_BALLSHRINK,		Texture::Trilinear);
	Texture* blackoutItemTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_BLACKOUT,			Texture::Trilinear);
	Texture* upsideDownItemTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_UPSIDEDOWN,		Texture::Trilinear);
	Texture* ballSafetyNetItemTex	= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_BALLSAFETYNET,	Texture::Trilinear);
	Texture* oneUpItemTex					= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_1UP,						Texture::Trilinear);
	
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
	assert(blackoutItemTex			!= NULL);
	assert(upsideDownItemTex		!= NULL);
	assert(ballSafetyNetItemTex != NULL);
	assert(oneUpItemTex					!= NULL);

	this->itemTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,						slowBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::FAST_BALL_ITEM_NAME,						fastBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(UberBallItem::UBER_BALL_ITEM_NAME,							uberBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,					invisiBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(GhostBallItem::GHOST_BALL_ITEM_NAME,						ghostBallItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,				laserPaddleItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(MultiBallItem::MULTI3_BALL_ITEM_NAME,					multiBall3ItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(MultiBallItem::MULTI5_BALL_ITEM_NAME,					multiBall5ItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(PaddleSizeItem::PADDLE_GROW_ITEM_NAME,					paddleGrowItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME,				paddleShrinkItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(BallSizeItem::BALL_GROW_ITEM_NAME,							ballGrowItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(BallSizeItem::BALL_SHRINK_ITEM_NAME,						ballShrinkItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(BlackoutItem::BLACKOUT_ITEM_NAME,							blackoutItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,					upsideDownItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME,	ballSafetyNetItemTex));
	this->itemTextures.insert(std::pair<std::string, Texture*>(OneUpItem::ONE_UP_ITEM_NAME,										oneUpItemTex));

	// Load the timer textures
	Texture* slowBallTimerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_SLOWBALL,		Texture::Trilinear);
	Texture* fastBallTimerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FASTBALL,		Texture::Trilinear);
	Texture* uberBallTimerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_UBERBALL,		Texture::Trilinear);
	Texture* invisiBallTimerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_INVISIBALL,	Texture::Trilinear);
	Texture* ghostBallTimerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_GHOSTBALL,		Texture::Trilinear);
	Texture* laserPaddleTimerTex	= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_PADDLELASER,	Texture::Trilinear);
	Texture* blackoutTimerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_BLACKOUT,		Texture::Trilinear);
	Texture* upsideDownTimerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_UPSIDEDOWN,	Texture::Trilinear);

	assert(slowBallTimerTex			!= NULL);
	assert(fastBallTimerTex			!= NULL);
	assert(uberBallTimerTex			!= NULL);
	assert(invisiBallTimerTex		!= NULL);
	assert(ghostBallTimerTex		!= NULL);
	assert(laserPaddleTimerTex	!= NULL);
	assert(blackoutTimerTex			!= NULL);
	assert(upsideDownTimerTex		!= NULL);

	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,				slowBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::FAST_BALL_ITEM_NAME,				fastBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(UberBallItem::UBER_BALL_ITEM_NAME,				uberBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,		invisiBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(GhostBallItem::GHOST_BALL_ITEM_NAME,			ghostBallTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,	laserPaddleTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(BlackoutItem::BLACKOUT_ITEM_NAME,					blackoutTimerTex));
	this->itemTimerTextures.insert(std::pair<std::string, Texture*>(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,			upsideDownTimerTex));

	// Load the fillers for the timer textures (used to make the timer look like its ticking down)
	Texture* ballSlowTimerFillerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_SPDBALL,		 Texture::Bilinear);
	Texture* ballFastTimerFillerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_SPDBALL,		 Texture::Bilinear);
	Texture* uberBallTimerFillerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_UBERBALL,		 Texture::Bilinear);
	Texture* invisiBallTimerFillerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_INVISIBALL,  Texture::Bilinear);
	Texture* ghostBallTimerFillerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL,	 Texture::Bilinear);
	Texture* laserPaddleTimerFillerTex	= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_PADDLELASER, Texture::Bilinear);
	Texture* blackoutTimerFillerTex			= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_BLACKOUT,		 Texture::Bilinear);
	Texture* upsideDownTimerFillerTex		= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN,		Texture::Bilinear);

	assert(ballSlowTimerFillerTex			!= NULL);
	assert(ballFastTimerFillerTex			!= NULL);
	assert(uberBallTimerFillerTex			!= NULL);
	assert(invisiBallTimerFillerTex		!= NULL);
	assert(ghostBallTimerFillerTex		!= NULL);
	assert(laserPaddleTimerFillerTex	!= NULL);
	assert(blackoutTimerFillerTex			!= NULL);
	assert(upsideDownTimerFillerTex		!= NULL);

	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::SLOW_BALL_ITEM_NAME,				ballSlowTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(BallSpeedItem::FAST_BALL_ITEM_NAME,				ballFastTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(UberBallItem::UBER_BALL_ITEM_NAME,				uberBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(InvisiBallItem::INVISI_BALL_ITEM_NAME,		invisiBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(GhostBallItem::GHOST_BALL_ITEM_NAME,			ghostBallTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,	laserPaddleTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(BlackoutItem::BLACKOUT_ITEM_NAME,					blackoutTimerFillerTex));
	this->itemTimerFillerTextures.insert(std::pair<std::string, Texture*>(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,			upsideDownTimerFillerTex));

	return true;
}

/**
 * Private helper function for loading item-related mesh assets.
 * Returns: true on successful load, false otherwise.
 */
bool GameItemAssets::LoadItemMeshes() {
		if (this->item == NULL) {
			// Load the mesh for items
			this->item = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_MESH);
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
	std::map<std::string, Texture*>::const_iterator lookupIter = this->itemTextures.find(itemName);
	assert(lookupIter != this->itemTextures.end());

	Texture2D* itemTexture = dynamic_cast<Texture2D*>(lookupIter->second);
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

	// If there are no timers to draw then we don't draw any
	if (timers.size() == 0) {
		return;
	}

	// TODO : NICE SMOOTH ANIMATIONS FOR TIMER CREATION AND DESTRUCTION!!!

	// Spacing along the vertical between timer graphics (in pixels)
	const int TIMER_VERTICAL_SPACING = 5;
	// Spacing along the horizontal of timer graphics distanced from the right edge of the game window (in pixels)
 	const int TIMER_HORIZONTAL_SPACING = displayWidth / 50;

	// There is only a max restriction on height, width follows based on the timer texture's width-to-height ratio
	const int MAX_TIMER_HEIGHT_SPACED = 60;
	// The timer height including spacing (includes spaces of TIMER_VERTICAL_SPACING at the bottom of timer)
	const int TIMER_HEIGHT_SPACED			= std::min<int>(MAX_TIMER_HEIGHT_SPACED, displayHeight / static_cast<int>(timers.size()));
	const int TIMER_HEIGHT_NON_SPACED	= TIMER_HEIGHT_SPACED - TIMER_VERTICAL_SPACING;

	// Tracked height at which to draw the next timer graphic
	int currHeight	= displayHeight - ((displayHeight - (static_cast<int>(timers.size()) * TIMER_HEIGHT_SPACED)) / 2);

	// Go through each timer and draw its appropriate graphic and current elapsed 'filler'
	for(std::list<GameItemTimer*>::const_iterator allTimerIter = timers.begin(); allTimerIter != timers.end(); allTimerIter++) {
		const GameItemTimer* timer = *allTimerIter;
		assert(timer != NULL);

		// If the timer has already expired then we don't draw it
		//if (timer->HasExpired()) {
		//	continue;
		//}

		// Check to see if a timer exists, if so then draw it
		std::map<std::string, Texture*>::iterator tempIterTimer = this->itemTimerTextures.find(timer->GetTimerItemName());
		std::map<std::string, Texture*>::iterator tempIterFiller = this->itemTimerFillerTextures.find(timer->GetTimerItemName());
		
		if (tempIterTimer != this->itemTimerTextures.end() && tempIterFiller != this->itemTimerFillerTextures.end()) {
			
			// Get the textures for the timer and its filler
			Texture* timerTex		= tempIterTimer->second;
			Texture* fillerTex	= tempIterFiller->second;
			
			assert(timerTex != NULL);
			assert(fillerTex != NULL);
			
			int height = TIMER_HEIGHT_NON_SPACED;
			int width	= height * timerTex->GetWidth() / timerTex->GetHeight();
			int halfHeight = height / 2;
			currHeight -= halfHeight;
			
			// Prepare OGL for drawing the timer
			glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
			
			// Make world coordinates equal window coordinates
			Camera::PushWindowCoords();
			
			glMatrixMode(GL_MODELVIEW);
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

			// Draw the timer...
			glPushMatrix();
			glLoadIdentity();

			// Do the transformation to setup the start location for where to draw the
			// timer graphics (right-hand side of the screen)
			glTranslatef(displayWidth - width - TIMER_HORIZONTAL_SPACING, currHeight, 0.0f);

			// Draw the filler for the timer (how much has elapsed)
			double percentElapsed = timer->GetPercentTimeElapsed();
			double fillerHeight    = height - (height * percentElapsed);
			double fillerTexHeight = 1.0 - percentElapsed;
			
			// Back fill - so that the filler that has expired so far stands out
			// from the background
			fillerTex->BindTexture();
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2f(0, 0);
			glTexCoord2i(1, 0); glVertex2f(width, 0);
			glTexCoord2i(1, 1); glVertex2f(width, height);
			glTexCoord2i(0, 1); glVertex2f(0, height);
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
										GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B(), 1.0f);
					break;
				default:
					assert(false);
					break;
			}
			
			// The actual filler, which is constantly decreasing to zero
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2d(0, 0);
			glTexCoord2d(1, 0); glVertex2d(width, 0);
			glTexCoord2d(1, fillerTexHeight); glVertex2d(width, fillerHeight);
			glTexCoord2d(0, fillerTexHeight); glVertex2d(0, fillerHeight);
			glEnd();
			fillerTex->UnbindTexture();
			
			// TODO: make draw lists to speed this up...
			timerTex->BindTexture();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2f(0, 0);
			glTexCoord2i(1, 0); glVertex2f(width, 0);
			glTexCoord2i(1, 1); glVertex2f(width, height);
			glTexCoord2i(0, 1); glVertex2f(0, height);
			glEnd();
			timerTex->UnbindTexture();

			glPopMatrix();
			glPopAttrib();  

			// Pop the projection matrix
			Camera::PopWindowCoords();

			// Add spacing under the timer 
			currHeight -= (halfHeight + TIMER_VERTICAL_SPACING);
		}
		else {
			assert(false);
		}
	}
}