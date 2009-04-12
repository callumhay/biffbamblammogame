#include "GameAssets.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "DecoSkybox.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"
#include "CgFxVolumetricEffect.h"
#include "GameFontAssetsManager.h"
#include "LoadingScreen.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"

// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

// *****************************************************

GameAssets::GameAssets(): 

worldAssets(NULL),
espAssets(NULL),
itemAssets(NULL),

ball(NULL), 
spikeyBall(NULL), 
paddleLaserAttachment(NULL),

invisiBallEffect(NULL), 
ghostBallEffect(NULL) {

	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	
	// TODO: Have loading screen stuff before any of this...

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
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();	// TODO

	// Load regular meshes
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading regular geometry...");
	this->LoadRegularMeshAssets();

	// Load regular effects
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading groovy effects...");
	this->LoadRegularEffectAssets();

	// Initialize default light values
	this->keyLight  = PointLight(Point3D(-25.0f, 20.0f, 50.0f), Colour(0.932f, 1.0f, 0.755f), 0.0f);
	this->fillLight = PointLight(Point3D(30.0f, 30.0f, 50.0f),  Colour(1.0f, 0.434f, 0.92f), 0.03f);
	this->ballLight = PointLight(Point3D(0,0,50), Colour(1,1,1), 0.0f);
}

GameAssets::~GameAssets() {
	// Delete regular mesh assets
	this->DeleteRegularMeshAssets();

	// Delete regular effect assets
	this->DeleteRegularEffectAssets();

	// Delete the currently loaded world and level assets if there are any
	this->DeleteWorldAssets();

	// Delete item assets
	if (this->itemAssets != NULL) {
		delete this->itemAssets;
		this->itemAssets = NULL;
	}
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
	if (this->paddleLaserAttachment != NULL) {
		delete this->paddleLaserAttachment;
		this->paddleLaserAttachment = NULL;
	}
}



// Draw the foreground level pieces...
void GameAssets::DrawLevelPieces(const GameLevel* currLevel, const Camera& camera) const {
	std::map<const GameLevel*, LevelMesh*>::const_iterator iter = this->loadedLevelMeshes.find(currLevel);
	assert(iter != this->loadedLevelMeshes.end());
	iter->second->Draw(camera, this->keyLight, this->fillLight, this->ballLight);
}

// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBall(double dT, const GameBall& b, const Camera& camera, Texture2D* sceneTex) const {
	
	CgFxEffectBase* ballEffectTemp = NULL;
	
	// Ball shaders when applicable...
	// The invisiball item always has priority
	if ((b.GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall) {
		this->invisiBallEffect->SetFBOTexture(sceneTex);
		ballEffectTemp = this->invisiBallEffect;
	}
	
	// Ball effects when applicable...
	if ((b.GetBallType() & GameBall::GhostBall) == GameBall::GhostBall &&
			(b.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall) {
		ballEffectTemp = this->ghostBallEffect;
		this->espAssets->DrawGhostBallEffects(dT, camera, b);
	}

	if ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall &&
			(b.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall) {
		// Draw when uber ball and not invisiball
		this->espAssets->DrawUberBallEffects(dT, camera, b);
	}


	// Ball model...
	glPushMatrix();
	Point2D loc = b.GetBounds().Center();
	float ballScaleFactor = b.GetBallScaleFactor();
	glTranslatef(loc[0], loc[1], 0);
	
	// Draw background effects for the ball
	this->espAssets->DrawBackgroundBallEffects(dT, camera, b);

	Vector3D ballRot = b.GetRotation();
	glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);
	glScalef(ballScaleFactor, ballScaleFactor, ballScaleFactor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		this->spikeyBall->Draw(camera, ballEffectTemp, this->keyLight, this->fillLight);
	}
	else {
		this->ball->Draw(camera, ballEffectTemp, this->keyLight, this->fillLight);
	}

	glPopMatrix();
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
	this->worldAssets->DrawPaddle(p, camera, this->keyLight, this->fillLight, this->ballLight);

	// In the case of a laser paddle, we draw the laser attachment
	if ((p.GetPaddleType() & PlayerPaddle::LaserPaddle) == PlayerPaddle::LaserPaddle) {
		this->paddleLaserAttachment->Draw(camera);
		
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
	this->worldAssets->DrawBackgroundModel(camera, this->keyLight, this->fillLight);
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
		this->ball = ObjReader::ReadMesh(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ObjReader::ReadMesh(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->paddleLaserAttachment == NULL) {
		this->paddleLaserAttachment = ObjReader::ReadMesh(GameViewConstants::GetInstance()->PADDLE_LASER_ATTACHMENT_MESH);
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
 * Debug function for drawing the lights that affect the game - draws them as
 * coloured cubes.
 */
void GameAssets::DebugDrawLights() const {
	this->keyLight.DebugDraw();
	this->fillLight.DebugDraw();
	this->ballLight.DebugDraw();

}