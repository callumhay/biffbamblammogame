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
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

#include "../GameSound/GameSoundAssets.h"

// Compositional classes for asssets
#include "GameWorldAssets.h"
#include "GameESPAssets.h"
#include "GameItemAssets.h"
#include "GameFBOAssets.h"
#include "GameLightAssets.h"
#include "RandomToItemAnimation.h"
#include "LevelMesh.h"

class GameModel;
class Texture3D;
class LevelMesh;
class CgFxPostRefract;
class CgFxVolumetricEffect;
class CgFxFireBallEffect;
class ESPEmitter;
class LivesLeftHUD;
class CrosshairLaserHUD;
class PlayerHurtHUD;
class FlashHUD;
class PointsHUD;
class StickyPaddleGoo;
class LaserPaddleGun;
class PaddleRocketMesh;
class PaddleShield;
class RandomItem;
class OmniLaserBallEffect;

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorldAssets* worldAssets;	// World-related assets
	GameESPAssets* espAssets;       // Emitter/Sprite/Particle assets
	GameItemAssets* itemAssets;		// Item-related assets (item drops, timers, etc.)
	GameFBOAssets* fboAssets;       // Framebuffer Object related assets
	GameLightAssets* lightAssets;   // Light assets in the foreground and background
	GameSoundAssets* soundAssets;   // Sound assets for everything

	LivesLeftHUD* lifeHUD;
	CrosshairLaserHUD* crosshairHUD;
	PlayerHurtHUD* painHUD;
	FlashHUD* flashHUD;
    PointsHUD* pointsHUD;

	// Level-related meshes
	LevelMesh* currentLevelMesh;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;																// Ball used to break blocks
	Mesh* spikeyBall;													// What happens to the ball when it becomes uber
	PaddleRocketMesh* rocketMesh;							// The rocket that can be fired from the paddle as a power-up

	Mesh* paddleBeamAttachment;                 // Laser beam attachment for paddle
	LaserPaddleGun* paddleLaserAttachment;		// Laser bullet/gun attachment for the paddle
	StickyPaddleGoo* paddleStickyAttachment;	// Sticky goo attachment for the paddle
	PaddleShield* paddleShield;                 // Refractive glowy shield for the paddle

	RandomToItemAnimation randomToItemAnimation;

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;
	CgFxVolumetricEffect* ghostBallEffect;
	CgFxFireBallEffect* fireBallEffect;

    OmniLaserBallEffect* omniLaserBallEffect;

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

	void LoadWorldAssets(const GameWorld& world);
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
	void DrawSafetyNetIfActive(double dT, const Camera& camera);
	void DrawStatusEffects(double dT, const Camera& camera);
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem);
	void DrawTimers(double dT, const Camera& camera);

	void DrawBeams(const GameModel& gameModel, const Camera& camera);
	void DrawTeslaLightning(double dT, const Camera& camera);
	void DrawProjectiles(double dT, const GameModel& gameModel, const Camera& camera);

	void DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel);

	void DrawActiveItemHUDElements(double dT, const GameModel& gameModel, int displayWidth, int displayHeight);

#ifdef _DEBUG
	void DebugDrawLights() const { this->lightAssets->DebugDrawLights(); };
#endif

	// Public Getter Functions **********************************************************************
	LevelMesh* GetCurrentLevelMesh() const {
		return this->currentLevelMesh;
	}
	void LoadNewLevelMesh(const GameLevel& currLevel);

	void ActivateRandomItemEffects(const GameModel& gameModel, const GameItem& actualItem);
	void ActivateItemEffects(const GameModel& gameModel, const GameItem& item);
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

	LivesLeftHUD* GetLifeHUD() const {
		return this->lifeHUD;
	}
    PointsHUD* GetPointsHUD() const {
        return this->pointsHUD;
    }

	void AddProjectile(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectile(const Projectile& projectile);

	void PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void RocketExplosion(const PaddleRocketProjectile& rocket, Camera& camera);
};

// Draw any currently active tesla lightning bolts in the game.
inline void GameAssets::DrawTeslaLightning(double dT, const Camera& camera) {
	this->espAssets->DrawTeslaLightningArcs(dT, camera);
}

// Draw the foreground level pieces...
inline void GameAssets::DrawLevelPieces(double dT, const GameLevel* currLevel, const Camera& camera) {
	Vector3D worldTransform(-currLevel->GetLevelUnitWidth()/2.0f, -currLevel->GetLevelUnitHeight()/2.0f, 0.0f);

	BasicPointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	this->GetCurrentLevelMesh()->DrawPieces(worldTransform, dT, camera, this->lightAssets->GetIsBlackOutActive(), fgKeyLight, fgFillLight, ballLight, this->fboAssets->GetPostFullSceneFBO()->GetFBOTexture());
}

inline void GameAssets::DrawSafetyNetIfActive(double dT, const Camera& camera) {
	BasicPointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	this->GetCurrentLevelMesh()->DrawSafetyNet(dT, camera, fgKeyLight, fgFillLight, ballLight);
}

// Draw the block status effects
inline void GameAssets::DrawStatusEffects(double dT, const Camera& camera) {
	this->GetCurrentLevelMesh()->DrawStatusEffects(dT, camera, this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
}

#endif