/**
 * GameSound.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __GAMESOUND_H__
#define __GAMESOUND_H__

#include "../GameModel/GameWorld.h"

#include "SoundCommon.h"
#include "SoundEffect.h"

// IrrKlang forward declarations
namespace irrklang {
    class ISoundEngine;
}

// GameSound forward declarations
class Sound;
class SoundEffect;
class AbstractSoundSource;

// BlammoEngine forward declarations
class Camera;
class IPositionObject;

class GameSound {
    friend class MSFReader;
public:
    enum EffectType {
        NoEffect = -1,
        BulletTimeEffect,
		InkSplatterEffect,
		PoisonEffect
    };

    enum SoundType {
        NoSound = -1,

        // Menu sounds
		MainMenuBackgroundLoop,
        MainMenuTitleBiffSlamEvent,
        MainMenuTitleBamSlamEvent,
        MainMenuTitleBlammoSlamEvent,
		MenuItemChangedSelectionEvent, 
		MenuItemEnteredEvent,
		MenuItemCancelEvent,
		MenuItemVerifyAndSelectEvent,
        MenuItemVerifyAndSelectStartGameEvent,
		MenuSelectionItemScrolledEvent,
        MenuScrollerItemScrolledEvent,
        MenuOpenSubMenuWindowEvent,
        
        // -> Blammopedia sounds
        BlammopediaBackgroundLoop,
        BlammopediaBaseMenuItemChangedSelectionEvent,
        BlammopediaBaseMenuItemSelectEvent,
        BlammopediaListItemChangedSelectionEvent,
        BlammopediaListItemSelectEvent,
        BlammopediaListItemDeselectEvent,
        BlammopediaListDeselectEvent,
        BlammopediaListItemLockedEvent,

        // -> World select menu sounds
        WorldMenuBackgroundLoop,
        WorldMenuItemChangedSelectionEvent,
        WorldMenuItemSelectEvent,
        WorldMenuItemLockedEvent,

        // -> Level select menu sounds
        LevelMenuBackgroundLoop,
        LevelMenuItemChangedSelectionEvent,
        LevelMenuItemSelectEvent,
        LevelMenuItemLockedEvent,

        // -> In-game menu sounds
        InGameMenuOpened,
        InGameMenuClosed,

        // World-specific sounds
        WorldBackgroundLoop,

        // In-game event sounds
        // -> Paddle-only sounds
		PaddleHitWallEvent,
        PaddleFrozenEvent,
        PaddleOnFireEvent,
        PaddleElectrocutedEvent,
        PaddleIceShakeEvent,

        // -> Paddle/block sounds
        PaddlePortalTeleportEvent,
		
        // -> Ball-only sounds
        PlayerLostABallButIsStillAliveEvent,
		LastBallExplodedEvent,
        BallBallCollisionEvent,
        BallBossCollisionEvent,
        LastBallSpiralingToDeathLoop,

        // -> Paddle/ball sounds
        BallPaddleCollisionEvent,
        BallStickyPaddleAttachEvent,
        BallStickyPaddleBounceEvent,
        BallShieldPaddleCollisionEvent,
        BallTeslaLightningCollisionEvent,
		BallOrPaddleGrowEvent,
		BallOrPaddleShrinkEvent,
        BallOnPaddleTimerRunningOutLoop,
        
        // -> Ball/block sounds
		BallBlockBasicBounceEvent,
        BallBlockCollisionColourChange,
        BallPortalTeleportEvent,
		
		// -> Block-only sounds
		BombBlockDestroyedEvent,
		InkBlockDestroyedEvent,
        InkSplatterEvent,
		BasicBlockDestroyedEvent,
		CannonBlockLoadedEvent,
		CannonBlockRotatingLoop,
        CannonBlockRotatingPart,
        CannonBlockFiredEvent,
        CannonObstructionMalfunctionEvent,
        FragileCannonDestroyedEvent,
        EnergyProjectilePortalTeleportEvent,
        NonBallObjectPortalTeleportEvent,

        CollateralBlockHitEvent,
		CollateralBlockFlashingLoop,
		CollateralBlockFallingLoop,
        TeslaLightningArcLoop,
        SwitchBlockActivated,
        ItemDropBlockLockedEvent,
        BlockFrozenEvent,
        BlockOnFireLoop,
        IceShatterEvent,
        IceMeltedEvent,
        FireFrozeEvent,

        // -> Projectile and beam sounds
        TurretRocketFiredEvent,
        TurretRocketMovingLoop,
        BossRocketFiredEvent,
        BossRocketMovingLoop,
		PaddleRocketLaunchEvent,
        PaddleRocketMovingLoop,
        RocketExplodedEvent,
        RemoteControlRocketThrustEvent,
        TurretMineFiredEvent,
        PaddleMineFiredEvent,
        MineExplodedEvent,
        MineLatchedOnEvent,
        FlameBlasterShotEvent,
        FlameBlasterHitEvent,
        IceBlasterShotEvent,
        IceBlasterHitEvent,
        LaserBulletShotEvent,
		LaserBeamFiringLoop,
        LaserDeflectedByShieldEvent,
        RocketOrMineDeflectedByShieldEvent,
        BlastDeflectedByShieldEvent,
        FireGlobShieldCollisionEvent,
        FireGlobBlockCollisionEvent,

        PaddleFireGlobCollisionEvent,
        PaddleLaserBulletCollisionEvent,
        PaddleLaserBeamCollisionEvent,
        PaddleCollateralBlockCollisionEvent,
        PortalProjectileOpenedEvent,

        // -> Item sounds
		BallSafetyNetCreatedEvent,
		BallSafetyNetDestroyedEvent,
        LevelFlipEvent,
        LevelUnflipEvent,
        PaddleShieldActivatedEvent,
        PaddleShieldDeactivatedEvent,
        MagnetPaddleLoop,
		ItemSpawnedEvent,
		PowerUpItemActivatedEvent,
		PowerNeutralItemActivatedEvent,
		PowerDownItemActivatedEvent,
        ItemTimerEndingLoop,
		ItemTimerEndedEvent,
        ItemMovingLoop,

        // -> Boss-related/specific sounds
        BossFadeInIntroEvent,
        BossBackgroundLoop,
        BossBackgroundLoopTransition,
        BossBackgroundLoopTransitionSingleHitEvent,
        BossAngryBackgroundLoop,
        BossGlowEvent,
        BossElectricitySpasmLoop,
        BossAngryEvent,
        BossHurtEvent,
        BossBlowingUpLoop,
        BossDeathFlashToFullscreenWhiteoutEvent,
        BossCrosshairTargetingEvent,
        BossLaserBeamLoop,
        BossFrozenEvent,
        BossIceShatterEvent,
        BossIceShakeEvent,
        ClassicalBossSparkleEvent,
        ClassicalBossArmShakeLoop,
        ClassicalBossArmAttackEvent,
        ClassicalBossArmAttackHitEvent,
        ClassicalBossPowerChargeEvent,
        GothicBossOrbAttackEvent,
        GothicBossSummonItemChargeEvent,
        GothicBossChargeShockwaveEvent,
        GothicBossMassiveShockwaveEvent,
        NouveauBossFrillBreakEvent,
        DecoBossLightningRelayTurnOnEvent,
        DecoBossArmRotateLoop,
        DecoBossArmExtendEvent,
        DecoBossArmRetractEvent,
        DecoBossArmLevelCollisionEvent,
        DecoBossArmPaddleCollisionEvent,
        DecoBossLevelRotatingLoop,
        DecoBossLightningBoltAttackEvent,
        DecoBossShockOrbAttackEvent,
        DecoBossElectricShockLoop,
        FuturismBossTeleportationChargingEvent,
        FuturismBossTeleportationEvent,
        FuturismBossPortalSummonChargeEvent,
        FuturismBossPortalFireEvent,
        FuturismBossTwitchBeamWarningEvent,
        FuturismBossArcBeamWarningEvent,
        FuturismBossAttractorBeamEvent,
        FuturismBossChargingEvent,
        FuturismBossSpinCoolDownEvent,

        // -> Bullet-time / boost sounds
        EnterBulletTimeEvent,
        InBulletTimeLoop,
        ExitBulletTimeEvent,
        BallBoostEvent,
        BallBoostGainedEvent,
        BoostMalfunctionPromptEvent,
        BoostAttemptWhileMalfunctioningEvent,

        // -> Star, points and life sounds
        LifeUpAcquiredEvent,
        StarAcquiredEvent,
        FiveStarsAcquiredEvent,
        ScoreMultiplierIncreasedTo2Event,
        ScoreMultiplierIncreasedTo3Event,
        ScoreMultiplierIncreasedTo4Event,
        ScoreMultiplierLostEvent,
        BlockBrokenMultiplierCounterInc1,
        BlockBrokenMultiplierCounterInc2,
        BlockBrokenMultiplierCounterInc3,
        BlockBrokenMultiplierCounterInc4,
        BlockBrokenMultiplierCounterInc5,
        BlockBrokenMultiplierCounterInc6,
        BlockBrokenMultiplierCounterInc7,
        BlockBrokenMultiplierCounterInc8,
        BlockBrokenMultiplierCounterInc9,

        // Non-in-game state sounds
        LevelStartPaddleMoveUpEvent,
        LevelStartBallSpawnOnPaddleEvent,
        LevelEndFadeoutEvent,
        LevelSummaryBackgroundLoop,
        LevelSummaryNewHighScoreEvent,
        LevelSummaryAllStarsEvent,
        LevelSummaryPointTallyLoop,
        LevelSummaryPointTallySkipEvent,
        LevelSummaryStarTallyEvent,
        LevelSummaryConfirmEvent,
        LevelBasicUnlockEvent,
        LevelStarCostPaidUnlockEvent,
        WorldCompleteBackgroundLoop,
        WorldCompleteVictoryMessageEvent,
        WorldSummaryConfirmEvent,
        WorldUnlockEvent,
        GameOverEvent
    };

    typedef std::map<GameSound::SoundType, AbstractSoundSource*> SoundSourceMap;
    typedef SoundSourceMap::iterator SoundSourceMapIter;
    typedef SoundSourceMap::const_iterator SoundSourceMapConstIter;

    typedef std::map<GameSound::EffectType, SoundEffect*> EffectMap;
    typedef EffectMap::iterator EffectMapIter;
    typedef EffectMap::const_iterator EffectMapConstIter;

    typedef std::map<GameWorld::WorldStyle, SoundSourceMap> WorldSoundSourceMap;
    typedef WorldSoundSourceMap::iterator WorldSoundSourceMapIter;
    typedef WorldSoundSourceMap::const_iterator WorldSoundSourceMapConstIter;

    static const int MENU_CONFIRM_SOUND_DELAY_IN_MS = 1200;
    
    static const float DEFAULT_MASTER_VOLUME;
    static const float DEFAULT_MIN_3D_SOUND_DIST;
    static const float DEFAULT_3D_SOUND_ROLLOFF_FACTOR;

    GameSound();
    ~GameSound();

    // Initialization function (MUST BE CALLED FIRST!)
    bool Init(bool arcadeMode);

    // Tick function (updates the sounds that are attached to moving objects and any sound animations)
    void Tick(double dT);

    // Sound loading functions
    void LoadGlobalSounds();
    void LoadWorldSounds(const GameWorld::WorldStyle& world);
    void ReloadFromMSF();

    // Play/stop functions
    void StopAllSounds(double fadeOutTimeInSecs = 0.0);
    void StopAllSoundLoops(double fadeOutTimeInSecs = 0.0);
    void StopAllSoundsExcept(const std::set<GameSound::SoundType>& exceptSounds, double fadeOutTimeInSecs = 0.0);
    void PauseAllSounds();
    void UnpauseAllSounds();
    SoundID PlaySound(const GameSound::SoundType& soundType, bool isLooped, bool applyActiveEffects = true, float volume = 1.0f);
    SoundID PlaySoundAtPosition(const GameSound::SoundType& soundType, bool isLooped, 
        const Point3D& position, bool applyActiveEffects, bool applyLevelTranslation, bool applyGameFGTransform, 
        float minDistance = 500.0f, float volume = 1.0f);

    void StopSound(SoundID soundID, double fadeOutTimeInSecs = 0.0);
    void StopAllSoundsWithType(const GameSound::SoundType& soundType, double fadeOutTimeInSecs = 0.0);

    void SetIgnorePlaySound(bool ignore);
    bool GetIgnorePlaySound() const { return this->ignorePlaySounds; }

    // Game object positional sound attaching/detaching functions
    SoundID AttachAndPlaySound(const IPositionObject* posObj, const GameSound::SoundType& soundType, bool isLooped, 
        const Vector3D& localTranslation = Vector3D(0,0,0), float volume = 1.0f);
    void DetachAndStopAllSounds(const IPositionObject* posObj, double fadeOutTimeInSecs = 0.0);
    void DetachAndStopSound(const IPositionObject* posObj, const GameSound::SoundType& soundType, double fadeOutTimeInSecs = 0.0);
    void SetPauseForAllAttachedSounds(const IPositionObject* posObj, bool isPaused);

    // Sound effect functions
    void StopAllEffects();
    void PauseAllEffects();
    void UnpauseAllEffects();
    void ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn);
    void ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn, SoundID ignoreSound);
    void ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn, const std::set<SoundID>& ignoreSounds);

    // Volume functions
    void SetMasterVolume(float volume);
    void SetMusicVolume(float volume);
    void SetSFXVolume(float volume);
    void SetSoundVolume(const SoundID& soundID, float volume);
    void SetSoundTypeVolume(const GameSound::SoundType& soundType, float volume);
    //void SetAllPlayingSoundsVolume(float volume, double lerpTimeInSecs = 0.0);

    // 3D/2D Sound functions
    void SetListenerPosition(const Camera& camera);
    void SetLevelTranslation(const Vector3D& t);
    void SetGameFGTransform(const Matrix4x4& m);
    
    // Query functions
    static bool IsMusic(const GameSound::SoundType& soundType);
    bool IsSoundPlaying(SoundID soundID) const;
    bool IsEffectActive(GameSound::EffectType type) const;
    bool IsEffectPaused(GameSound::EffectType type) const;
    float GetMusicVolume() const;
    float GetSFXVolume() const;

private:
    typedef std::map<SoundID, Sound*> SoundMap;
    typedef SoundMap::iterator SoundMapIter;
    typedef SoundMap::const_iterator SoundMapConstIter;

    struct AttachedSoundInfo {
        SoundMap soundMap;
        Vector3D localTranslation;

        AttachedSoundInfo() : localTranslation(0,0,0) {}
        ~AttachedSoundInfo() {}

        void ClearSoundMap();
    };

    typedef std::map<const IPositionObject*, AttachedSoundInfo> AttachedSoundMap;
    typedef AttachedSoundMap::iterator AttachedSoundMapIter;
    typedef AttachedSoundMap::const_iterator AttachedSoundMapConstIter;

    typedef std::set<GameSound::EffectType> EffectSet;
    typedef EffectSet::iterator EffectSetIter;
    typedef EffectSet::const_iterator EffectSetConstIter;

    typedef std::map<GameSound::SoundType, bool> IsMusicMap;
    typedef IsMusicMap::iterator IsMusicMapIter;
    typedef IsMusicMap::const_iterator IsMusicMapConstIter;

    // Sound Source and Effects Maps
    SoundSourceMap globalSounds;
    EffectMap globalEffects;
    WorldSoundSourceMap worldSounds;
    GameWorld::WorldStyle currLoadedWorldStyle;
    
    // Active sounds (i.e., sounds that are playing in the game right now)
    SoundMap nonAttachedPlayingSounds;
    AttachedSoundMap attachedPlayingSounds;

    // Active effects (i.e., effects that are affecting all playing sounds in the game right now)
    EffectSet activeEffects;
    EffectSet pausedEffects;

    // 3D Transforms
    Vector3D levelTranslation;
    Matrix4x4 gameFGTransform;

    bool ignorePlaySounds;

    // Separate volumes for music and sound effects
    float musicVolume;
    float sfxVolume;

    static IsMusicMap musicSoundTypeMap; // A map that keeps track of what sounds types are music

    // IrrKlang stuff
    irrklang::ISoundEngine* soundEngine;

    // Helper functions
    AbstractSoundSource* BuildSoundSource(const GameSound::SoundType& soundType,
        const std::string& soundName, const std::vector<std::string>& filePaths);
    SoundEffect* BuildSoundEffect(const GameSound::EffectType& effectType, 
        const std::string& effectName, const std::vector<std::string>& effectsStrs,
        const SoundEffect::EffectParameterMap& parameterMap);

    bool LoadFromMSF();

    bool PlaySoundWithID(const SoundID& id, const GameSound::SoundType& soundType, bool isLooped, bool startPaused = false);
    float GetSoundTypeMasterVolume(const Sound& sound) const;

    void ClearAll();
    void ClearEffects();
    void ClearSounds();
    void ClearSoundSources();

    AbstractSoundSource* GetSoundSourceFromType(const GameSound::SoundType& type) const;
    SoundEffect* GetSoundEffectFromType(const GameSound::EffectType& type) const;
    Sound* GetPlayingSound(SoundID soundID) const;
    void GetAllPlayingSoundsAsList(std::list<Sound*>& playingSounds) const;
    void GetAllPlayingMusicAsList(std::list<Sound*>& playingMusic) const;
    void GetAllPlayingSFXAsList(std::list<Sound*>& playingSFX) const;

    Sound* BuildSound(const GameSound::SoundType& soundType, bool isLooped, 
        const Point3D* position = NULL, bool applyActiveEffects = true, bool startPaused = false);

    DISALLOW_COPY_AND_ASSIGN(GameSound);
};

inline void GameSound::SetIgnorePlaySound(bool ignore) {
    this->ignorePlaySounds = ignore;
}

inline bool GameSound::IsMusic(const GameSound::SoundType& soundType) {
    assert(GameSound::musicSoundTypeMap.find(soundType) != GameSound::musicSoundTypeMap.end());
    return GameSound::musicSoundTypeMap[soundType];
}

inline bool GameSound::IsSoundPlaying(SoundID soundID) const {
    return this->GetPlayingSound(soundID) != NULL;
}

inline bool GameSound::IsEffectActive(GameSound::EffectType type) const {
    return this->activeEffects.find(type) != this->activeEffects.end();
}

inline bool GameSound::IsEffectPaused(GameSound::EffectType type) const {
    return this->pausedEffects.find(type) != this->pausedEffects.end();
}

inline float GameSound::GetMusicVolume() const {
    return this->musicVolume;
}

inline float GameSound::GetSFXVolume() const {
    return this->sfxVolume;
}

inline void GameSound::ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn) {
    std::set<SoundID> temp;
    this->ToggleSoundEffect(effectType, effectOn, temp);
}

inline void GameSound::ToggleSoundEffect(const GameSound::EffectType& effectType, bool effectOn, SoundID ignoreSound) {
    std::set<SoundID> temp;
    temp.insert(ignoreSound);
    this->ToggleSoundEffect(effectType, effectOn, temp);
}

inline void GameSound::SetLevelTranslation(const Vector3D& t) {
    this->levelTranslation = t;
}

inline void GameSound::SetGameFGTransform(const Matrix4x4& m) {
    this->gameFGTransform = m;
}

#endif // __GAMESOUND_H__