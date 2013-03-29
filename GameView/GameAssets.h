/**
 * GameAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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
#include "../GameModel/SafetyNet.h"

// Compositional classes for asssets
#include "GameWorldAssets.h"
#include "GameESPAssets.h"
#include "GameItemAssets.h"
#include "GameFBOAssets.h"
#include "GameLightAssets.h"
#include "GameTutorialAssets.h"
#include "RandomToItemAnimation.h"
#include "LevelMesh.h"
#include "BallSafetyNetMesh.h"

class GameSound;
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
class BallBoostHUD;
class BallReleaseHUD;
class StickyPaddleGoo;
class LaserPaddleGun;
class RocketMesh;
class PaddleShield;
class RandomItem;
class OmniLaserBallEffect;
class MagnetPaddleEffect;
class RocketProjectile;
class MineMeshManager;
class FullscreenFlashEffectInfo;

// Includes all the models, textures, etc. for the game.
class GameAssets {
public:
	GameAssets(int screenWidth, int screenHeight, GameSound* sound);
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
    void DrawBoss(double dT, const GameLevel* currLevel, const Camera& camera);
	void DrawSafetyNetIfActive(double dT, const Camera& camera, const GameModel& gameModel);
	void DrawStatusEffects(double dT, const Camera& camera);
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem);
	void DrawTimers(double dT, const Camera& camera);

	void DrawBeams(const GameModel& gameModel, const Camera& camera);
	void DrawTeslaLightning(double dT, const Camera& camera);
	void DrawProjectiles(double dT, const GameModel& gameModel, const Camera& camera);

	void DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel);

	void DrawActiveItemHUDElements(double dT, const GameModel& gameModel, int displayWidth, int displayHeight);

    void BallSafetyNetCreated();
    void BallSafetyNetDestroyed(const GameLevel& currLevel, const Point2D& pos);

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

    double ActivateBossIntro();
    double ActivateBossExplodingFlashEffects(double delayInSecs,
        const GameModel* model, const Camera& camera);

    void ToggleLights(bool lightsOn, double toggleTime);
    void ToggleLightsForBossDeath(bool lightsOn, double toggleTime);

	void ActivateLastBallDeathEffects(const GameBall& lastBall);
	void DeactivateLastBallDeathEffects();

    void ReinitializeAssets();

	GameItemAssets* GetItemAssets() const {
		return this->itemAssets;
	}

	GameESPAssets* GetESPAssets() const {
		return this->espAssets;
	}

	GameFBOAssets* GetFBOAssets() {
		return this->fboAssets;
	}

    GameTutorialAssets* GetTutorialAssets() {
        return this->tutorialAssets;
    }

	LivesLeftHUD* GetLifeHUD() const {
		return this->lifeHUD;
	}
    PointsHUD* GetPointsHUD() const {
        return this->pointsHUD;
    }
    BallBoostHUD* GetBoostHUD() const {
        return this->boostHUD;
    }
    BallReleaseHUD* GetBallReleaseHUD() const {
        return this->ballReleaseHUD;
    }

	void AddProjectile(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectile(const Projectile& projectile);

	void PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void RocketExplosion(const RocketProjectile& rocket, Camera& camera);
    void MineExplosion(const MineProjectile& mine, Camera& camera);

    void FullscreenFlashExplosion(const FullscreenFlashEffectInfo& info, Camera& camera);

private:
    GameSound* sound; // Sound module for the game

	GameWorldAssets* worldAssets;	    // World-related assets
	GameESPAssets* espAssets;           // Emitter/Sprite/Particle assets
	GameItemAssets* itemAssets;		    // Item-related assets (item drops, timers, etc.)
	GameFBOAssets* fboAssets;           // Framebuffer Object related assets
	GameLightAssets* lightAssets;       // Light assets in the foreground and background
    GameTutorialAssets* tutorialAssets; // Tutorial texture assets

	LivesLeftHUD* lifeHUD;
	CrosshairLaserHUD* crosshairHUD;
	PlayerHurtHUD* painHUD;
	FlashHUD* flashHUD;
    PointsHUD* pointsHUD;
    BallBoostHUD* boostHUD;
    BallReleaseHUD* ballReleaseHUD;

	// Level-related meshes
	LevelMesh* currentLevelMesh;

	// Regular meshes - these persist throughout the entire game
    Mesh* ball;									// Ball used to break blocks
    Mesh* spikeyBall;							// What happens to the ball when it becomes uber
    RocketMesh* rocketMesh;						// The rocket that can be fired from the paddle as a power-up
    
    MineMeshManager* mineMeshMgr;               // A manager for the mines that can be fired from the paddle

	Mesh* paddleBeamAttachment;                 // Laser beam attachment for paddle
    Mesh* paddleMineAttachment;                 // Mine launcher attachment for paddle
	LaserPaddleGun* paddleLaserAttachment;		// Laser bullet/gun attachment for the paddle
	StickyPaddleGoo* paddleStickyAttachment;	// Sticky goo attachment for the paddle
	PaddleShield* paddleShield;                 // Refractive glowy shield for the paddle

	// Misc. display lists and meshes for other geometry activated by items and such
	BallSafetyNetMesh* ballSafetyNet;

	RandomToItemAnimation randomToItemAnimation;

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisibleEffect;
	CgFxVolumetricEffect* ghostBallEffect;
	CgFxFireBallEffect* fireBallEffect;

    OmniLaserBallEffect* omniLaserBallEffect;
    MagnetPaddleEffect* magnetPaddleEffect;

	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularEffectAssets();

	// Projectile specific functionality
	void FirePaddleLaser(const PlayerPaddle& paddle);
	void FireRocket(const RocketProjectile& rocketProjectile);

    // Private draw functions
    void DrawGameBallsPreEffects(double dT, GameModel& gameModel, const Camera& camera);
    void DrawGameBallsBoostPostEffects(double dT, GameModel& gameModel, const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(GameAssets);
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
	this->GetCurrentLevelMesh()->DrawPieces(worldTransform, dT, camera,
        this->lightAssets->GetIsBlackOutActive(), fgKeyLight, fgFillLight,
        ballLight, this->fboAssets->GetPostFullSceneFBO()->GetFBOTexture());
}

inline void GameAssets::DrawSafetyNetIfActive(double dT, const Camera& camera, const GameModel& gameModel) {
	BasicPointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);

	// If the ball safety net is active then we draw it
	if (gameModel.IsSafetyNetActive() || this->ballSafetyNet->IsPlayingAnimation()) {
        GameLevel* currLevel = gameModel.GetCurrentLevel();
        
        glPushMatrix();
		glTranslatef(-currLevel->GetLevelUnitWidth() / 2.0f, -(currLevel->GetLevelUnitHeight() / 2.0f + SafetyNet::SAFETY_NET_HALF_HEIGHT), 0.0f);
		this->ballSafetyNet->Draw(dT, camera, fgKeyLight, fgFillLight, ballLight);
		glPopMatrix();
	}
}

// Draw the block status effects
inline void GameAssets::DrawStatusEffects(double dT, const Camera& camera) {
	this->GetCurrentLevelMesh()->DrawStatusEffects(dT, camera, this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
}

/**
 * Activates the boss intro animation.
 * Returns: The total time of the animation.
 */
inline double GameAssets::ActivateBossIntro() {
    return this->GetCurrentLevelMesh()->ActivateBossIntro();
}

/**
 * Activates the final exploding flash animation for a boss on its 'outro'.
 * Returns: The total time of the animation, including the given delay.
 */
inline double GameAssets::ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera) {
    return this->GetCurrentLevelMesh()->ActivateBossExplodingFlashEffects(delayInSecs, model, camera);
}

inline void GameAssets::ToggleLights(bool lightsOn, double toggleTime) {
    this->lightAssets->ToggleLights(lightsOn, toggleTime);
}
inline void GameAssets::ToggleLightsForBossDeath(bool lightsOn, double toggleTime) {
    this->lightAssets->ToggleLightsForBossDeath(lightsOn, toggleTime);
}

#endif