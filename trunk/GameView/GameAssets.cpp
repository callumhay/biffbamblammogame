#include "GameAssets.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "DecoSkybox.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"
#include "CgFxVolumetricEffect.h"
#include "GameFontAssetsManager.h"
#include "LoadingScreen.h"
#include "LivesLeftHUD.h"

// Game Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/BlackoutItem.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/ResourceManager.h"

// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

// *****************************************************

GameAssets::GameAssets(): 

worldAssets(NULL),
espAssets(NULL),
itemAssets(NULL),

lifeHUD(NULL),

ball(NULL), 
spikeyBall(NULL), 
paddleLaserAttachment(NULL),

invisiBallEffect(NULL), 
ghostBallEffect(NULL) {

	// Load ESP assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading purdy pictures...");
	this->espAssets = new GameESPAssets();

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
	this->lifeHUD = new LivesLeftHUD();

	// Initialize default light values
	this->ToggleLights(true);
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

	// Delete the lives left HUD
	delete this->lifeHUD;
	this->lifeHUD = NULL;
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

/**
 * Toggle the lights in the game either on or off (both foreground
 * and background lights are affected).
 */
void GameAssets::ToggleLights(bool turnOn) {
	if (turnOn) {
		this->fgKeyLight  = PointLight(Point3D(-30.0f, 40.0f, 65.0f), GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_COLOUR, 0.0f);
		this->fgFillLight = PointLight(Point3D(25.0f, 0.0f, 40.0f), GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR,  0.037f);
		this->ballLight		= PointLight(Point3D(0,0,0), GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR, 
																	 GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_ATTEN);

		this->ballKeyLight	= this->fgKeyLight;
		this->ballKeyLight.SetDiffuseColour(Colour(0.9f, 0.9f, 0.9f));
		this->ballFillLight	= this->fgFillLight;
		this->ballFillLight.SetDiffuseColour(Colour(0,0,0));
		this->paddleKeyLight = this->fgKeyLight;
		this->paddleFillLight = this->fgFillLight;
	}
	else {
		// Turn off foreground lights...
		this->fgKeyLight.SetDiffuseColour(Colour(0,0,0));
		this->fgFillLight.SetDiffuseColour(Colour(0,0,0));

		// Set the attenuation to be a smaller distance for the ball
		this->ballLight.SetLinearAttenuation(0.8f);
	}

	// Background lights...
	if (this->worldAssets != NULL) {
		this->worldAssets->ToggleBackgroundLights(turnOn);	
	}
}

// Draw the foreground level pieces...
void GameAssets::DrawLevelPieces(double dT, const GameLevel* currLevel, const Camera& camera) {
	std::map<const GameLevel*, LevelMesh*>::const_iterator iter = this->loadedLevelMeshes.find(currLevel);
	assert(iter != this->loadedLevelMeshes.end());
	iter->second->Draw(dT, camera, this->fgKeyLight, this->fgFillLight, this->ballLight);
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, Texture2D* sceneTex, const Vector2D& worldT) {
	
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
				this->invisiBallEffect->SetFBOTexture(sceneTex);
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

		if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
			this->spikeyBall->Draw(camera, ballEffectTemp, this->ballKeyLight, this->ballFillLight);
		}
		else {
			this->ball->Draw(camera, ballEffectTemp, this->ballKeyLight, this->ballFillLight);
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
		Point3D newAvgBallPos =  gameModel.GetTransformInfo().GetGameTransform() * avgBallPosition;

		// Set the ball light to the correct position
		this->ballLight.SetPosition(newAvgBallPos);
	}
	else {
		avgBallColour = Colour(0,0,0);
	}

	// Set the ball light to the correct diffuse colour
	this->ballLight.SetDiffuseColour(avgBallColour);
}

void GameAssets::Tick(double dT) {
	this->worldAssets->Tick(dT);
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera) const {
	Point2D paddleCenter = p.GetCenterPosition();	

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1], 0);

	// Draw any effects on the paddle (e.g., item acquiring effects)
	this->espAssets->DrawBackgroundPaddleEffects(dT, camera, p);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	
	this->worldAssets->DrawPaddle(p, camera, this->paddleKeyLight, this->paddleFillLight, this->ballLight);

	// In the case of a laser paddle, we draw the laser attachment
	if ((p.GetPaddleType() & PlayerPaddle::LaserPaddle) == PlayerPaddle::LaserPaddle) {
		this->paddleLaserAttachment->Draw(camera, this->paddleKeyLight, this->paddleFillLight);
		
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
	this->worldAssets->DrawBackgroundModel(camera);
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
void GameAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem) const {
	this->itemAssets->DrawItem(dT, camera, gameItem);
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameAssets::DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight) {
	this->itemAssets->DrawTimers(timers, displayWidth, displayHeight);
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->paddleLaserAttachment == NULL) {
		this->paddleLaserAttachment = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_LASER_ATTACHMENT_MESH);
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
		this->ghostBallEffect->SetConstantFactor(0.05f);
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
}

/**
 * Activate the effect for a particular item - this can be anything from changing lighting to
 * making pretty eye-candy-particles.
 */
void GameAssets::ActivateItemEffects(const GameModel& gameModel, const GameItem& item, const Camera& camera) {
	// First deal with any particle related effects
	this->espAssets->SetItemEffect(item, gameModel);

	// Deal with any light changes...
	if (item.GetName() == BlackoutItem::BLACKOUT_ITEM_NAME) {
		// Turn the lights off and make only the paddle and ball visible.
		assert(gameModel.IsBlackoutEffectActive());
		this->ToggleLights(false);
	}
}

/**
 * Deactivate the effect for a particular item - this will clean up anything that was activated
 * which needs manual clean up.
 */
void GameAssets::DeactivateItemEffects(const GameModel& gameModel, const GameItem& item) {
	
	// Deal with any light changes...
	if (item.GetName() == BlackoutItem::BLACKOUT_ITEM_NAME) {
		// Turn the lights back on and revert lights back to their defaults
		assert(!gameModel.IsBlackoutEffectActive());
		this->ToggleLights(true);
	}
}

/**
 * Debug function for drawing the lights that affect the game - draws them as
 * coloured cubes.
 */
void GameAssets::DebugDrawLights() const {
	if (!GameDisplay::IsDrawDebugLightGeometryOn()) { return; }

	this->fgKeyLight.DebugDraw();
	this->fgFillLight.DebugDraw();
	this->ballLight.DebugDraw();
}