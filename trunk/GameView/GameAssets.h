/**
 * GameAssets.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../BlammoEngine/Light.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"
#include "../GameModel/SafetyNet.h"

// Compositional classes for assets
#include "GameWorldAssets.h"
#include "GameItemAssets.h"
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
class RemoteControlRocketHUD;
class BallCamHUD;
class PaddleCamHUD;
class MalfunctionTextHUD;
class ButtonTutorialHint;
class StickyPaddleGoo;
class PaddleGunAttachment;
class RocketMesh;
class PaddleShield;
class RandomItem;
class OmniLaserBallEffect;
class MagnetPaddleEffect;
class RocketProjectile;
class MineMeshManager;
class FullscreenFlashEffectInfo;
class GameESPAssets;
class GameFBOAssets;
class PaddleMineLauncher;
class PaddleBeamAttachment;
class PaddleStatusEffectRenderer;

// Includes all the models, textures, etc. for the game.
class GameAssets {
public:
	GameAssets(int screenWidth, int screenHeight, GameSound* sound);
	~GameAssets();

	void LoadWorldAssets(const GameWorld& world);
	void Tick(double dT, const GameModel& gameModel);

	// Draw functions ******************************************************************************
	void DrawPaddle(double dT, const GameModel& gameModel, const Camera& camera);
	void DrawPaddlePostEffects(double dT, GameModel& gameModel, const Camera& camera, FBObj* sceneFBO);

	void DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, const Vector2D& worldT);
	void DrawGameBallsPostEffects(double dT, GameModel& gameModel, const Camera& camera);

	void DrawSkybox(const Camera& camera);
    void FastDrawBackgroundModel();
	void DrawBackgroundModel(const Camera& camera);
	void DrawBackgroundEffects(const Camera& camera);

	void DrawFirstPassLevelPieces(double dT, const GameModel& gameModel, const Camera& camera);
    void DrawSecondPassLevelPieces(double dT, const GameModel& gameModel, const Camera& camera);
    void DrawLevelPiecesPostEffects(double dT, const Camera& camera);
    void DrawNoBloomLevelPieces(double dT, const Camera& camera);
    void DrawBoss(double dT, const Camera& camera, const GameModel& gameModel);
    void DrawBossPostEffects(double dT, const Camera& camera, const GameModel& gameModel);
	void DrawSafetyNetIfActive(double dT, const Camera& camera, const GameModel& gameModel);
	void DrawStatusEffects(double dT, const Camera& camera, FBObj* sceneFBO);
    void DrawMiscEffects(const GameModel& gameModel);
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem);
	void DrawTimers(double dT, const Camera& camera, const GameModel& gameModel);

	void DrawBeams(double dT, const GameModel& gameModel, const Camera& camera, const Vector2D& worldT);
    void DrawMeshProjectiles(double dT, const GameModel& gameModel, const Camera& camera);

	void DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel);

	void DrawActiveItemHUDElements(double dT, const GameModel& gameModel, int displayWidth, int displayHeight);

    const Point2D& GetBallSafetyNetPosition(bool bottomSafetyNet) const;
    void BallSafetyNetCreated(bool bottomSafetyNet);
    void BallSafetyNetDestroyed(const GameLevel& currLevel, const Point2D& pos, bool bottomSafetyNet);

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
	void DeactivateItemEffects(const GameModel& gameModel, const GameItem& item, bool gameIsInPlay);
    void RemoveBeamEffects(const GameModel& gameModel, const Beam& beam);
    
    void ActivatePaddleStatusEffect(const GameModel& gameModel, Camera& camera, PlayerPaddle::PaddleSpecialStatus status);
    void DeactivatePaddleStatusEffect(const GameModel& gameModel, Camera& camera, PlayerPaddle::PaddleSpecialStatus status);
    
    void CancelFrozenPaddleWithFireEffect(const PlayerPaddle& paddle);
    void CancelOnFirePaddleWithIceEffect(const PlayerPaddle& paddle);

    void DeactivateMiscEffects();


    double ActivateBossIntro();
    double ActivateBossExplodingFlashEffects(double delayInSecs,
        const GameModel* model, const Camera& camera);

    void ToggleLights(bool lightsOn, double toggleTime);
    void ToggleLightsForBossDeath(bool lightsOn, double toggleTime);

	void ActivateLastBallDeathEffects(const GameBall& lastBall);
	void DeactivateLastBallDeathEffects();

    void ApplyRocketThrust(const PaddleRemoteControlRocketProjectile& rocket);

    void ReinitializeAssets();

    GameLightAssets* GetLightAssets() const {
        return this->lightAssets;
    }

	GameItemAssets* GetItemAssets() const {
		return this->itemAssets;
	}

	GameESPAssets* GetESPAssets() const {
		return this->espAssets;
	}

	GameFBOAssets* GetFBOAssets() const {
		return this->fboAssets;
	}

    GameTutorialAssets* GetTutorialAssets() {
        return this->tutorialAssets;
    }

    GameWorldAssets* GetCurrentWorldAssets() const {
        return this->worldAssets;
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
    RemoteControlRocketHUD* GetRemoteControlRocketHUD() const {
        return this->remoteControlRocketHUD;
    }
    BallCamHUD* GetBallCamHUD() const {
        return this->ballCamHUD;
    }
    PaddleCamHUD* GetPaddleCamHUD() const {
        return this->paddleCamHUD;
    }

    GameSound* GetSound() const {
        return this->sound;
    }

	void AddProjectile(const GameModel& gameModel, const Projectile& projectile);
	void RemoveProjectile(const GameModel& gameModel, const Projectile& projectile);

	void PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile, Camera& camera);
    void PaddleHurtByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void PaddleHurtByBossBodyPart(const GameModel& gameModel, const PlayerPaddle& paddle, const Boss& boss, const BossBodyPart& bossPart);

	void RocketExplosion(const RocketProjectile& rocket, Camera& camera, const GameModel* gameModel);
    void MineExplosion(const MineProjectile& mine, Camera& camera, const GameModel* gameModel);

    void FullscreenFlashExplosion(const FullscreenFlashEffectInfo& info, Camera& camera, const GameModel* gameModel);
    void FullscreenFlash(double durationInS, float intensity);

    void ToggleSkipLabel(bool activate);
    void ReinitializeSkipLabel();

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
    RemoteControlRocketHUD* remoteControlRocketHUD;
    BallCamHUD* ballCamHUD;
    PaddleCamHUD* paddleCamHUD;

    ButtonTutorialHint* skipLabel;

	// Level-related meshes
	LevelMesh* currentLevelMesh;

	// Regular meshes - these persist throughout the entire game
    Mesh* ball;									// Ball used to break blocks
    Mesh* spikeyBall;							// What happens to the ball when it becomes uber
    RocketMesh* rocketMesh;						// The rocket that can be fired from the paddle as a power-up
    
    MineMeshManager* mineMeshMgr;               // A manager for the mines that can be fired from the paddle

	PaddleBeamAttachment* paddleBeamAttachment; // Laser beam attachment for paddle
    PaddleMineLauncher* paddleMineAttachment;   // Mine launcher attachment for paddle
	PaddleGunAttachment* paddleGunAttachment;		// Laser bullet/gun attachment for the paddle
	StickyPaddleGoo* paddleStickyAttachment;	// Sticky goo attachment for the paddle
	PaddleShield* paddleShield;                 // Refractive glowy shield for the paddle

    PaddleStatusEffectRenderer* paddleStatusEffectRenderer;

	// Misc. display lists and meshes for other geometry activated by items and such
	BallSafetyNetMesh* bottomSafetyNetMesh;
    BallSafetyNetMesh* topSafetyNetMesh;

	RandomToItemAnimation randomToItemAnimation;

	// Special effects - persistent special effects in the game
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
	void FireRocket(const GameModel& gameModel, const RocketProjectile& rocketProjectile);

    // Private draw functions
    void DrawGameBallsPreEffects(double dT, GameModel& gameModel, const Camera& camera);
    void DrawGameBallsBoostPostEffects(double dT, GameModel& gameModel, const Camera& camera);
    
    void DrawRandomToItemAnimation(const Camera& camera, double dT, const GameModel& gameModel);
    void DrawSkipDeathAnimation(const Camera& camera, double dT);

    void DrawSafetyNet(BallSafetyNetMesh* safetyNetMesh, double dT, const Camera& camera, const GameModel& gameModel, float yTranslationOffset);
    
    void SetActiveEffectLights(const GameModel& gameModel, GameItem::ItemType itemEffectType);

    DISALLOW_COPY_AND_ASSIGN(GameAssets);
};

/**
 * Draw a given item in the world.
 */
inline void GameAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem) {
	BasicPointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	this->itemAssets->DrawItem(dT, camera, gameItem, fgKeyLight, fgFillLight, ballLight);
}

/**
 * Draw the HUD timer for the given timer type.
 */
inline void GameAssets::DrawTimers(double dT, const Camera& camera, const GameModel& gameModel) {
	this->itemAssets->DrawTimers(dT, camera, gameModel);
}



inline void GameAssets::DrawLevelPiecesPostEffects(double dT, const Camera& camera) {
    BasicPointLight fgKeyLight, fgFillLight, ballLight;
    this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
    this->GetCurrentLevelMesh()->DrawPiecesPostEffects(dT, camera, fgKeyLight, fgFillLight, ballLight);
}

inline void GameAssets::DrawNoBloomLevelPieces(double dT, const Camera& camera) {

    BasicPointLight fgKeyLight, fgFillLight, ballLight;
    this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
    LevelMesh* lvlMesh = this->GetCurrentLevelMesh();
    lvlMesh->DrawNoBloomPieces(dT, camera, fgKeyLight, fgFillLight, ballLight);
    lvlMesh->DrawTransparentNoBloomPieces(dT, camera, fgKeyLight, fgFillLight, ballLight);
}

// Draw the block status effects
inline void GameAssets::DrawStatusEffects(double dT, const Camera& camera, FBObj* sceneFBO) {
	this->GetCurrentLevelMesh()->DrawStatusEffects(dT, camera, sceneFBO->GetFBOTexture());
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