/**
 * GameAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../BlammoEngine/Light.h"
#include "../BlammoEngine/Animation.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

// Compositional classes for asssets
#include "GameWorldAssets.h"
#include "GameESPAssets.h"
#include "GameItemAssets.h"
#include "GameFBOAssets.h"
#include "GameLightAssets.h"
#include "../GameSound/GameSoundAssets.h"

class GameModel;
class Texture3D;
class LevelMesh;
class CgFxPostRefract;
class CgFxVolumetricEffect;
class ESPEmitter;
class LivesLeftHUD;
class CrosshairLaserHUD;
class PlayerHurtHUD;
class FlashHUD;
class StickyPaddleGoo;
class LaserPaddleGun;
class PaddleRocketMesh;

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorldAssets* worldAssets;	// World-related assets
	GameESPAssets* espAssets;			// Emitter/Sprite/Particle assets
	GameItemAssets* itemAssets;		// Item-related assets (item drops, timers, etc.)
	GameFBOAssets* fboAssets;			// Framebuffer Object related assets
	GameLightAssets* lightAssets; // Light assets in the foreground and background
	GameSoundAssets* soundAssets; // Sound assets for everything

	LivesLeftHUD* lifeHUD;
	CrosshairLaserHUD* crosshairHUD;
	PlayerHurtHUD* painHUD;
	FlashHUD* flashHUD;

	// Level-related meshes
	std::map<const GameLevel*, LevelMesh*> loadedLevelMeshes;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;																// Ball used to break blocks
	Mesh* spikeyBall;													// What happens to the ball when it becomes uber
	PaddleRocketMesh* rocketMesh;							// The rocket that can be fired from the paddle as a power-up

	Mesh* paddleBeamAttachment;								// Laser beam attachment for paddle
	LaserPaddleGun* paddleLaserAttachment;		// Laser bullet/gun attachment for the paddle
	StickyPaddleGoo* paddleStickyAttachment;	// Sticky goo attachment for the paddle

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;
	CgFxVolumetricEffect* ghostBallEffect;

	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularEffectAssets();

	// Projectile specific functionality
	void FirePaddleLaser(const PlayerPaddle& paddle);
	void FirePaddleRocket(const Projectile& rocketProjectile);

public:
	GameAssets(int screenWidth, int screenHeight);
	~GameAssets();

	void LoadWorldAssets(const GameWorld* world);
	void Tick(double dT);

	// Draw functions ******************************************************************************
	void DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera);
	void DrawPaddlePostEffects(double dT, GameModel& gameModel, const Camera& camera);

	void DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, const Vector2D& worldT);
	void DrawGameBallsPostEffects(double dT, GameModel& gameModel, const Camera& camera);

	void DrawSkybox(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera);
	void DrawBackgroundEffects(const Camera& camera);

	void DrawLevelPieces(double dT, const GameLevel* currLevel, const Camera& camera);
	void DrawSafetyNetIfActive(double dT, const GameLevel* currLevel, const Camera& camera);
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem);
	void DrawTimers(double dT, const Camera& camera);

	void DrawBeams(double dT, const GameModel& gameModel, const Camera& camera);
	void DrawProjectiles(double dT, const GameModel& gameModel, const Camera& camera);

	void DrawActiveItemHUDElements(double dT, const GameModel& gameModel, int displayWidth, int displayHeight);

#ifdef _DEBUG
	void DebugDrawLights() const { this->lightAssets->DebugDrawLights(); };
#endif

	// Public Getter Functions **********************************************************************
	LevelMesh* GetLevelMesh(const GameLevel* currLevel) const {
		std::map<const GameLevel*, LevelMesh*>::const_iterator iter = this->loadedLevelMeshes.find(currLevel);
		if (iter == this->loadedLevelMeshes.end()) {
			assert(false);
			return NULL;
		}
		return iter->second;
	}

	void ActivateItemEffects(const GameModel& gameModel, const GameItem& item, const Camera& camera);
	void DeactivateItemEffects(const GameModel& gameModel, const GameItem& item);
	void DeactivateMiscEffects();

	void ActivateLastBallDeathEffects(const GameBall& lastBall);
	void DeactivateLastBallDeathEffects();

	GameItemAssets* GetItemAssets() const {
		return this->itemAssets;
	}

	GameESPAssets* GetESPAssets() const {
		return this->espAssets;
	}

	GameFBOAssets* GetFBOAssets() {
		return this->fboAssets;
	}

	GameSoundAssets* GetSoundAssets() {
		return this->soundAssets;
	}
	//void PlaySound() {
	//	this->soundAssets->
	//}

	LivesLeftHUD* GetLifeHUD() const {
		return this->lifeHUD;
	}

	void AddProjectile(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectile(Camera& camera, const GameModel& gameModel, const Projectile& projectile);

	void PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void ExplosionFlash(double timeLength, float intensityPercent);
};

#endif