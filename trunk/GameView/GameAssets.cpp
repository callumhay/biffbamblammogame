#include "GameAssets.h"
#include "GameDisplay.h"
#include "LevelMesh.h"
#include "DecoSkybox.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"
#include "CgFxVolumetricEffect.h"
#include "GameFontAssetsManager.h"

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
levelMesh(NULL), 

invisiBallEffect(NULL), 
ghostBallEffect(NULL) {

	// Initialize DevIL
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	
	// Load ESP assets
	this->espAssets = new GameESPAssets();

	// Load item assets
	this->itemAssets = new GameItemAssets(this->espAssets);
	bool didItemAssetsLoad = this->itemAssets->LoadItemAssets();
	assert(didItemAssetsLoad);

	// Load minimal fonts
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();

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
}



// Draw the foreground level pieces...
void GameAssets::DrawLevelPieces(const Camera& camera) const {
	this->levelMesh->Draw(camera);
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
	glTranslatef(loc[0], loc[1], 0);

	Vector3D ballRot = b.GetRotation();
	glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);
	if ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		this->spikeyBall->Draw(camera, ballEffectTemp);
	}
	else {
		this->ball->Draw(camera, ballEffectTemp);
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
 * Draw the skybox for the current world type.
 */
void GameAssets::DrawSkybox(double dT, const Camera& camera) {
	this->worldAssets->DrawSkybox(dT, camera);
}

/**
 * Draw the background model for the current world type.
 */
void GameAssets::DrawBackgroundModel(double dT, const Camera& camera) {
	this->worldAssets->DrawBackgroundModel(dT, camera);
}

/**
 * Draw the background effects for the current world type.
 */
void GameAssets::DrawBackgroundEffects(double dT, const Camera& camera) {
	this->worldAssets->DrawBackgroundEffects(dT, camera);
}

/**
 * Draw any particles present in the game currently.
 */
void GameAssets::DrawParticleEffects(double dT, const Camera& camera) {
	this->espAssets->DrawParticleEffects(dT, camera);
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
