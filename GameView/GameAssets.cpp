#include "GameAssets.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"
#include "CgFxVolumetricEffect.h"
#include "GameFontAssetsManager.h"
#include "LoadingScreen.h"
#include "LivesLeftHUD.h"
#include "CrosshairHUD.h"
#include "StickyPaddleGoo.h"
#include "LaserPaddleGun.h"

// Game Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"
// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

#include "../ResourceManager.h"

// *****************************************************

GameAssets::GameAssets(int screenWidth, int screenHeight): 

worldAssets(NULL),
espAssets(NULL),
itemAssets(NULL),
fboAssets(NULL),
lightAssets(NULL),
soundAssets(NULL),

lifeHUD(NULL),
crosshairHUD(NULL),

ball(NULL), 
spikeyBall(NULL), 
paddleLaserAttachment(NULL),
paddleStickyAttachment(NULL),

invisiBallEffect(NULL), 
ghostBallEffect(NULL)
{

	// Load ESP assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading purdy pictures...");
	this->espAssets = new GameESPAssets();

	// Load FBO assets
	this->fboAssets = new GameFBOAssets(screenWidth, screenHeight);

	// Load sound assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading melodic tunage...");
	this->soundAssets = new GameSoundAssets();

	// Load item assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading game items...");
	this->itemAssets = new GameItemAssets(this->espAssets);
	bool didItemAssetsLoad = this->itemAssets->LoadItemAssets();
	assert(didItemAssetsLoad);

	// Load all fonts
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading fonts...");
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();

	// Load regular meshes
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading regular geometry...");
	this->LoadRegularMeshAssets();

	// Load regular effects
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading groovy effects...");
	this->LoadRegularEffectAssets();

	// Initialize any HUD elements
	this->lifeHUD				= new LivesLeftHUD();
	this->crosshairHUD	= new CrosshairHUD();

	// Initialize the light assets
	this->lightAssets = new GameLightAssets();
}

GameAssets::~GameAssets() {
	// Delete regular effect assets
	this->DeleteRegularEffectAssets();

	// Delete the currently loaded world and level assets if there are any
	this->DeleteWorldAssets();

	// Delete item assets
	if (this->itemAssets != NULL) {
		delete this->itemAssets;
		this->itemAssets = NULL;
	}

	delete this->paddleLaserAttachment;
	this->paddleLaserAttachment = NULL;
	delete this->paddleStickyAttachment;
	this->paddleStickyAttachment = NULL;

	// Clear up the FrameBuffer Object assets
	delete this->fboAssets;
	this->fboAssets = NULL;

	// Clear up the light assets
	delete this->lightAssets;
	this->lightAssets = NULL;

	// Clear up sound assets
	delete this->soundAssets;
	this->soundAssets = NULL;

	// Delete any HUD objects
	delete this->lifeHUD;
	this->lifeHUD = NULL;
	delete this->crosshairHUD;
	this->crosshairHUD = NULL;
}

/*
 * Delete any previously loaded assets related to the world.
 */
void GameAssets::DeleteWorldAssets() {
	// Delete all the levels for the world that are currently loaded - THIS MUST BE CALLED BEFORE DELETING
	// THE WORLD ASSETS!!!
	for(std::map<const GameLevel*, LevelMesh*>::iterator iter = this->loadedLevelMeshes.begin(); iter != this->loadedLevelMeshes.end(); iter++) {
		LevelMesh* currMesh = iter->second;
		delete currMesh;
		currMesh = NULL;
	}
	this->loadedLevelMeshes.clear();	
	
	if (this->worldAssets != NULL) {
		delete this->worldAssets;
		this->worldAssets = NULL;
	}
}

// Draw the foreground level pieces...
void GameAssets::DrawLevelPieces(double dT, const GameLevel* currLevel, const Camera& camera) {
	LevelMesh* currLevelMesh = this->GetLevelMesh(currLevel);

	PointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	currLevelMesh->DrawPieces(dT, camera, fgKeyLight, fgFillLight, ballLight);
}
void GameAssets::DrawSafetyNetIfActive(double dT, const GameLevel* currLevel, const Camera& camera) {
	LevelMesh* currLevelMesh = this->GetLevelMesh(currLevel);

	PointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	currLevelMesh->DrawSafetyNet(dT, camera, fgKeyLight, fgFillLight, ballLight);
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, const Vector2D& worldT) {
	
	// Average values used to calculate the colour and position of the ball light
	Point3D avgBallPosition(0,0,0);
	Colour avgBallColour(0,0,0);
	unsigned int visibleBallCount = 0;

	// Go through each ball in the game, draw it accordingly
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();
	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		
		CgFxEffectBase* ballEffectTemp = NULL;
		Colour currBallColour(0,0,0);

		Point2D ballPos = currBall->GetBounds().Center();

		if (currBall->GetBallType() == GameBall::NormalBall) {
			// Normal ball with a regular light
			avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], 0.0f);
			avgBallColour = avgBallColour + GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR;
			visibleBallCount++;
		}
		else {
			// The ball has an item in effect on it... figure out what effect and render it appropriately

			unsigned int numColoursApplied = 0;

			// GHOST BALL CHECK
			if ((currBall->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall &&
					(currBall->GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall) {
				
				// Draw when the ghost ball is not an invisiball	
				ballEffectTemp = this->ghostBallEffect;
				this->espAssets->DrawGhostBallEffects(dT, camera, *currBall);
				currBallColour = currBallColour +
												 Colour(GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.R(),
																GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.G(),
																GameViewConstants::GetInstance()->GHOST_BALL_COLOUR.B());
				numColoursApplied++;
			}

			// UBER BALL CHECK
			if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall &&
					(currBall->GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall) {
				
				// Draw when uber ball and not invisiball
				this->espAssets->DrawUberBallEffects(dT, camera, *currBall);
				currBallColour = currBallColour + 
												 Colour(GameViewConstants::GetInstance()->UBER_BALL_COLOUR.R(),
																GameViewConstants::GetInstance()->UBER_BALL_COLOUR.G(),
																GameViewConstants::GetInstance()->UBER_BALL_COLOUR.B());
				numColoursApplied++;
			}

			// INVISIBALL CHECK
			if ((currBall->GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall) {
				// Obtain the POST full screen rendering from the previous frame - this is
				// a bit of a hack but it saves us from reading/writing to the same FBO simultaneously
				this->invisiBallEffect->SetFBOTexture(this->fboAssets->GetPostFullSceneFBO()->GetFBOTexture());
				ballEffectTemp = this->invisiBallEffect;
			}
			else {
				// We only take the average of visible balls.
				avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], 0.0f);
			
				assert(numColoursApplied > 0);
				avgBallColour = avgBallColour + (currBallColour / numColoursApplied);

				visibleBallCount++;
			}
		}

		// Draw the ball model...
		glPushMatrix();
		float ballScaleFactor = currBall->GetBallScaleFactor();
		glTranslatef(ballPos[0], ballPos[1], 0);
		
		// Draw background effects for the ball
		this->espAssets->DrawBackgroundBallEffects(dT, camera, *currBall);

		Vector3D ballRot = currBall->GetRotation();
		glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
		glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
		glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);
		glScalef(ballScaleFactor, ballScaleFactor, ballScaleFactor);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		PointLight ballKeyLight, ballFillLight;
		this->lightAssets->GetBallAffectingLights(ballKeyLight, ballFillLight);

		if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
			this->spikeyBall->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}
		else {
			this->ball->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}

		glPopMatrix();
	}

	// Calculate the average position and colour of all the visible balls in the game
	if (visibleBallCount > 0) {
		avgBallPosition = avgBallPosition / visibleBallCount;
		avgBallPosition = avgBallPosition + Vector3D(worldT[0], worldT[1], 0.0f);
		avgBallColour = avgBallColour / visibleBallCount;
		
		// Grab a trasform matrix from the game model to say where the ball light is
		// if the level is flipped or some such thing
		Point3D newAvgBallPos =  gameModel.GetTransformInfo()->GetGameTransform() * avgBallPosition;

		// Set the ball light to the correct position
		this->lightAssets->GetBallLight().SetPosition(newAvgBallPos);
	}
	else {
		avgBallColour = Colour(0,0,0);
	}

	// Set the ball light to the correct diffuse colour
	this->lightAssets->GetBallLight().SetDiffuseColour(avgBallColour);
}

/**
 * Draw the effects that take place after drawing everything (Except final fullscreen effects).
 */
void GameAssets::DrawGameBallsFinalEffects(double dT, GameModel& gameModel, const Camera& camera) {
	
	// Go through each ball and draw its post effects
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();

	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ballIter++) {
		GameBall* currBall = *ballIter;		
		
		// PADDLE CAMERA CHECK
		PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
		if (paddle->GetIsPaddleCameraOn() && (currBall->GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall) {
			// Draw a target around balls when in paddle camera mode AND ball is not invisible
			this->espAssets->DrawTargetBallEffects(dT, camera, *currBall, *paddle);
		}
	}
}

void GameAssets::Tick(double dT) {
	// Tick the world assets (e.g., for background animations)
	this->worldAssets->Tick(dT);

	// Tick the FBO assets (e.g., for post-processing / fullscreen effects animations)
	this->fboAssets->Tick(dT);

	// Tick the light assets (light animations for strobing, changing colours, etc.)
	this->lightAssets->Tick(dT);

	// Tick the sound assets
	this->soundAssets->Tick(dT);
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera) {
	Point2D paddleCenter = p.GetCenterPosition();	
	float paddleScaleFactor = p.GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);
	
	// Obtain the lights that affect the paddle
	PointLight paddleKeyLight, paddleFillLight, ballLight;
	this->lightAssets->GetPaddleAffectingLights(paddleKeyLight, paddleFillLight, ballLight);

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);

	// When the paddle camera is on we just draw the paddle with no special effects
	// the camera will fade based on its alpha (see world assets DrawPaddle function)
	if (p.GetIsPaddleCameraOn()) {
		// Just draw the paddle and finish
		this->worldAssets->DrawPaddle(p, camera, paddleKeyLight, paddleFillLight, ballLight);
		glPopMatrix();
		return;
	}

	// Draw any effects on the paddle (e.g., item acquiring effects)
	this->espAssets->DrawBackgroundPaddleEffects(dT, camera, p);
	// Draw the paddle
	this->worldAssets->DrawPaddle(p, camera, paddleKeyLight, paddleFillLight, ballLight);

	// In the case of a laser paddle, we draw the laser attachment and its related effects
	if ((p.GetPaddleType() & PlayerPaddle::LaserPaddle) == PlayerPaddle::LaserPaddle) {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		// Draw attachment (gun) mesh
		this->paddleLaserAttachment->Draw(dT, p, camera, paddleKeyLight, paddleFillLight);
	}

	glPopMatrix();
}

/**
 * Used to draw any relevant post-processing-related effects for the paddle.
 */
void GameAssets::DrawPaddlePostEffects(double dT, const PlayerPaddle& p, const Camera& camera) {
	// Do nothing if we are in paddle camera mode
	if (p.GetIsPaddleCameraOn()) {
		return;
	}
	
	Point2D paddleCenter = p.GetCenterPosition();
	float paddleScaleFactor = p.GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	

	// When the paddle has the 'sticky' power-up we attach sticky goo to its top
	if ((p.GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {	
		// Set the texture for the refraction in the goo - be careful here, we can't get
		// the 'post full scene' fbo because we are currently in the middle of drawing to it
		this->paddleStickyAttachment->SetSceneTexture(this->fboAssets->GetFullSceneFBO()->GetFBOTexture());

		// Obtain the lights that affect the paddle
		PointLight paddleKeyLight, paddleFillLight, ballLight;
		this->lightAssets->GetPaddleAffectingLights(paddleKeyLight, paddleFillLight, ballLight);

		// Draw the sticky goo
		this->paddleStickyAttachment->Draw(p, camera, paddleKeyLight, paddleFillLight, ballLight);
	}

	if ((p.GetPaddleType() & PlayerPaddle::LaserPaddle) == PlayerPaddle::LaserPaddle) {
		// Draw glowy effects where the laser originates...
		this->espAssets->DrawPaddleLaserEffects(dT, camera, p);
	}

	glPopMatrix();
}

/**
 * Draw the skybox for the current world type.
 */
void GameAssets::DrawSkybox(const Camera& camera) {
	this->worldAssets->DrawSkybox(camera);
}

/**
 * Draw the background model for the current world type.
 */
void GameAssets::DrawBackgroundModel(const Camera& camera) {
	PointLight bgKeyLight, bgFillLight;
	this->lightAssets->GetBackgroundAffectingLights(bgKeyLight, bgFillLight);
	this->worldAssets->DrawBackgroundModel(camera, bgKeyLight, bgFillLight);
}

/**
 * Draw the background effects for the current world type.
 */
void GameAssets::DrawBackgroundEffects(const Camera& camera) {
	this->worldAssets->DrawBackgroundEffects(camera);
}

/**
 * Draw a given item in the world.
 */
void GameAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem) {
	PointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	this->itemAssets->DrawItem(dT, camera, gameItem, fgKeyLight, fgFillLight, ballLight);
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameAssets::DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight) {
	this->itemAssets->DrawTimers(timers, displayWidth, displayHeight);
}

/**
 * Draw HUD elements for any of the active items when applicable.
 */
void GameAssets::DrawActiveItemHUDElements(const GameModel& gameModel, int displayWidth, int displayHeight) {
	
	// If the laser paddle is active and paddle camera is also active then we draw a crosshair overlay
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	if (paddle->GetPaddleType() == PlayerPaddle::LaserPaddle && paddle->GetIsPaddleCameraOn()) {
		this->crosshairHUD->Draw(displayWidth, displayHeight);
	}
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->paddleLaserAttachment == NULL) {
		this->paddleLaserAttachment = new LaserPaddleGun();
	}
	if (this->paddleStickyAttachment == NULL) {
		this->paddleStickyAttachment = new StickyPaddleGoo();
	}
}

void GameAssets::LoadRegularEffectAssets() {
	if (this->invisiBallEffect == NULL) {
		this->invisiBallEffect = new CgFxPostRefract();
		this->invisiBallEffect->SetWarpAmountParam(50.0f);
		this->invisiBallEffect->SetIndexOfRefraction(1.33f);
	}
	if (this->ghostBallEffect == NULL) {
		this->ghostBallEffect = new CgFxVolumetricEffect();
		this->ghostBallEffect->SetTechnique(CgFxVolumetricEffect::GHOSTBALL_TECHNIQUE_NAME);
		this->ghostBallEffect->SetColour(Colour(0.80f, 0.90f, 1.0f));
		this->ghostBallEffect->SetFadeExponent(1.0f);
		this->ghostBallEffect->SetScale(0.1f);
		this->ghostBallEffect->SetFrequency(0.8f);
		this->ghostBallEffect->SetAlphaMultiplier(1.0f);
		this->ghostBallEffect->SetFlowDirection(Vector3D(0, 0, 1));
	}
}

void GameAssets::DeleteRegularEffectAssets() {
	// Delete the invisiball effect
	if (this->invisiBallEffect != NULL) {
		delete this->invisiBallEffect;
		this->invisiBallEffect = NULL;
	}

	if (this->ghostBallEffect != NULL) {
		delete this->ghostBallEffect;
		this->ghostBallEffect = NULL;
	}

	// Delete any left behind particles
	if (this->espAssets != NULL) {
		delete this->espAssets;
		this->espAssets = NULL;
	}
}

/**
 * Cause the laser paddle attachment to animate - like it's reacting to shooting
 * a bullet or something.
 */
void GameAssets::FirePaddleLaser(const PlayerPaddle& paddle) {
	this->paddleLaserAttachment->FirePaddleLaserGun(paddle);
}

/*
 * This will load a set of assets for use in a game based off a
 * given world-style, after loading all assets will be available for use in-game.
 * Precondition: world != NULL.
 */
void GameAssets::LoadWorldAssets(const GameWorld* world) {
	assert(world != NULL);

	// Delete all previously loaded style-related assets
	this->DeleteWorldAssets();
	
	// Load up the new set of assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading world assets...");
	this->worldAssets = GameWorldAssets::CreateWorldAssets(world->GetStyle());
	assert(this->worldAssets != NULL);

	// Load all of the level meshes for the world
	const std::vector<GameLevel*>& levels = world->GetAllLevelsInWorld();
	for (std::vector<GameLevel*>::const_iterator iter = levels.begin(); iter != levels.end(); iter++) {
		const GameLevel* level = *iter;
		assert(level != NULL);

		LoadingScreen::GetInstance()->UpdateLoadingScreen(LoadingScreen::ABSURD_LOADING_DESCRIPTION);

		// Create a mesh for the level
		LevelMesh* levelMesh = new LevelMesh(this->worldAssets, level);
		this->loadedLevelMeshes.insert(std::pair<const GameLevel*, LevelMesh*>(level, levelMesh));
	}

	// Load all of the music for the world
	this->soundAssets->LoadWorldSounds(world->GetStyle());
}

/**
 * Activate the effect for a particular item - this can be anything from changing lighting to
 * making pretty eye-candy-particles.
 */
void GameAssets::ActivateItemEffects(const GameModel& gameModel, const GameItem& item, const Camera& camera) {
	// First deal with any particle related effects
	this->espAssets->SetItemEffect(item, gameModel);
	// Also make the FBO assets aware of a newly active effect
	this->fboAssets->ActivateItemEffects(item);

	// Deal with any light changes...
	switch (item.GetItemType()) {
		case GameItem::BlackoutItem: {
				// Turn the lights off and make only the paddle and ball visible.
				assert(gameModel.IsBlackoutEffectActive());
				this->lightAssets->ToggleLights(false);
			}
			break;
			
		case GameItem::PoisonPaddleItem: {
				// The poison item will make the lights turn a sickly green colour
				assert((gameModel.GetPlayerPaddle()->GetPaddleType() & PlayerPaddle::PoisonPaddle) == PlayerPaddle::PoisonPaddle);

				this->lightAssets->StartStrobeLight(GameLightAssets::FGKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
				this->lightAssets->StartStrobeLight(GameLightAssets::FGFillLight, GameViewConstants::GetInstance()->POISON_LIGHT_DEEP_COLOUR, 1.0f);
				this->lightAssets->StartStrobeLight(GameLightAssets::BallKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
			}
		  break;

		case GameItem::PaddleCamItem: {
				// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
				// NOTE that this is not permanent and just does so for any currently falling items
				this->espAssets->TurnOffCurrentItemDropStars(camera);
				
				// We make the safety net transparent so that it doesn't obstruct the paddle cam... too much
				LevelMesh* currLevelMesh = this->GetLevelMesh(gameModel.GetCurrentLevel());
				assert(currLevelMesh != NULL);
				currLevelMesh->PaddleCameraActiveToggle(true);

				// Move the key light in the foreground so that it is behind the camera when it goes into
				// paddle cam mode.
				float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;

				Point3D newFGKeyLightPos(0.0f, -(halfLevelHeight + 10.0f), 0.0f);
				Point3D newFGFillLightPos(0.0f, (halfLevelHeight + 10.0f), 0.0f);
				
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGKeyLight, newFGKeyLightPos, 2.0f);
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGFillLight, newFGFillLightPos, 2.0f);

				// Fade out the background...
				this->worldAssets->FadeBackground(true, 2.0f);
			}
			break;

		default:
			break;
	}
}

/**
 * Deactivate the effect for a particular item - this will clean up anything that was activated
 * which needs manual clean up.
 */
void GameAssets::DeactivateItemEffects(const GameModel& gameModel, const GameItem& item) {
	// Also make the FBO assets aware of the deactivated effect
	this->fboAssets->DeactivateItemEffects(item);

	// Deal with any light changes...
	switch (item.GetItemType()) {
		case GameItem::BlackoutItem: {
				// Turn the lights back on and revert lights back to their defaults
				assert(!gameModel.IsBlackoutEffectActive());
				this->lightAssets->ToggleLights(true);
			}
		  break;

		case GameItem::PoisonPaddleItem: {
				// Stop strobing the lights
				this->lightAssets->StopStrobeLight(GameLightAssets::FGKeyLight);
				this->lightAssets->StopStrobeLight(GameLightAssets::FGFillLight);
				this->lightAssets->StopStrobeLight(GameLightAssets::BallKeyLight);
			}
		  break;

		case GameItem::PaddleCamItem: {
				// We make the safety net visible (if activated) again
				LevelMesh* currLevelMesh = this->GetLevelMesh(gameModel.GetCurrentLevel());
				assert(currLevelMesh != NULL);
				currLevelMesh->PaddleCameraActiveToggle(false);

				// Move the foreground key and fill lights back to their default positions...
				this->lightAssets->RestoreLightPosition(GameLightAssets::FGKeyLight, 2.0f);
				this->lightAssets->RestoreLightPosition(GameLightAssets::FGFillLight, 2.0f);

				// Show the background once again...
				this->worldAssets->FadeBackground(false, 2.0f);
			}
			break;

		default:
			break;
	}
}

/**
 * Deactivate all misc. effects in the view that may not be deactivated from items alone.
 */
void GameAssets::DeactivateMiscEffects() {
	this->fboAssets->DeactivateInkSplatterEffect();
	this->espAssets->KillAllActiveEffects();
}