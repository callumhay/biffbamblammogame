/**
 * GameSound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
		
        // -> Ball-only sounds
        PlayerLostABallButIsStillAliveEvent,
		LastBallExplodedEvent,
        BallBallCollisionEvent,
        LastBallSpiralingToDeathLoop,

        // -> Paddle/ball sounds
        BallPaddleCollisionEvent,
        BallStickyPaddleAttachEvent,
        BallStickyPaddleBounceEvent,
        BallShieldPaddleCollisionEvent,
		BallOrPaddleGrowEvent,
		BallOrPaddleShrinkEvent,
        
        // -> Ball/block sounds
		BallBlockBasicBounceEvent,
        BallBlockCollisionColourChange,
		
		// -> Block-only sounds
		BombBlockDestroyedEvent,
		InkBlockDestroyedEvent,
        InkSplatterEvent,
		BasicBlockDestroyedEvent,
		CannonBlockLoadedEvent,
		CannonBlockRotatingLoop,
        CannonBlockFiredEvent,
		PortalTeleportEvent,
		CollateralBlockFlashingLoop,
		CollateralBlockFallingLoop,
        TeslaLightningArcLoop,
        SwitchBlockActivated,
        BlockFrozenEvent,
        BlockOnFireLoop,
        IceShatterEvent,
        IceMeltedEvent,
        FireFrozeEvent,

        // -> Projectile and beam sounds
		PaddleRocketLaunchEvent,
        PaddleRocketMovingLoop,
        //BossRocketMovingLoop,
        //TurretRocketMovingLoop,
        RocketExplodedEvent,
        TurretMineFiredEvent,
        PaddleMineFiredEvent,
        MineExplodedEvent,
        LaserBulletShotEvent,
		LaserBeamFiringLoop,
        LaserDeflectedByShieldEvent,
        RocketOrMineDeflectedByShieldEvent,

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
        BossElectricitySpasmLoop,
        BossAngryEvent,
        BossHurtEvent,
        BossBlowingUpLoop,
        BossDeathFlashToFullscreen,
        BossCrosshairTargetingEvent,
        BossLaserBeamLoop,
        ClassicalBossArmShakeLoop,
        ClassicalBossArmAttackEvent,
        ClassicalBossArmAttackHitEvent,
        GothicBossSummonItemChargeEvent,
        GothicBossChargeShockwaveEvent,
        GothicBossMassiveShockwaveEvent,
        DecoBossArmRotateLoop,
        DecoBossArmExtendEvent,
        DecoBossArmRetractEvent,
        DecoBossArmLevelCollisionEvent,
        DecoBossArmPaddleCollisionEvent,
        DecoBossLevelRotatingLoop,

        // -> Bullet-time / boost sounds
        EnterBulletTimeEvent,
        InBulletTimeLoop,
        ExitBulletTimeEvent,
        BallBoostEvent,
        BallBoostGainedEvent,

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
        LevelSummaryNewHighScoreEvent,
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
    
    static const float DEFAULT_MIN_3D_SOUND_DIST;
    static const float DEFAULT_3D_SOUND_ROLLOFF_FACTOR;

    GameSound();
    ~GameSound();

    // Initialization function (MUST BE CALLED FIRST!)
    bool Init();

    // Tick function (updates the sounds that are attached to moving objects and any sound animations)
    void Tick(double dT);

    // Sound loading functions
    void LoadGlobalSounds();
    void LoadWorldSounds(const GameWorld::WorldStyle& world);
    void ReloadFromMSF();

    // Play/stop functions
    void StopAllSounds(double fadeOutTimeInSecs = 0.0);
    void StopAllSoundLoops(double fadeOutTimeInSecs = 0.0);
    void PauseAllSounds();
    void UnpauseAllSounds();
    SoundID PlaySound(const GameSound::SoundType& soundType, bool isLooped, bool applyActiveEffects = true);
    SoundID PlaySoundAtPosition(const GameSound::SoundType& soundType, bool isLooped, 
        const Point3D& position, bool applyActiveEffects, bool applyLevelTranslation, bool applyGameFGTransform, 
        float minDistance = DEFAULT_MIN_3D_SOUND_DIST);

    void StopSound(SoundID soundID, double fadeOutTimeInSecs = 0.0);
    void StopAllSoundsWithType(const GameSound::SoundType& soundType, double fadeOutTimeInSecs = 0.0);

    // Game object positional sound attaching/detaching functions
    SoundID AttachAndPlaySound(const IPositionObject* posObj, const GameSound::SoundType& soundType, bool isLooped, 
        const Vector3D& localTranslation = Vector3D(0,0,0));
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
    void SetSoundVolume(const SoundID& soundID, float volume);
    //void SetAllPlayingSoundsVolume(float volume, double lerpTimeInSecs = 0.0);

    // 3D/2D Sound functions
    void SetListenerPosition(const Camera& camera);
    void SetLevelTranslation(const Vector3D& t);
    void SetGameFGTransform(const Matrix4x4& m);
    
    // Query functions
    bool IsSoundPlaying(SoundID soundID) const;
    bool IsEffectActive(GameSound::EffectType type) const;
    bool IsEffectPaused(GameSound::EffectType type) const;

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

    // IrrKlang stuff
    irrklang::ISoundEngine* soundEngine;

    // Helper functions
    AbstractSoundSource* BuildSoundSource(const GameSound::SoundType& soundType,
        const std::string& soundName, const std::vector<std::string>& filePaths);
    SoundEffect* BuildSoundEffect(const GameSound::EffectType& effectType, 
        const std::string& effectName, const std::vector<std::string>& effectsStrs,
        const SoundEffect::EffectParameterMap& parameterMap);

    bool LoadFromMSF();

    bool PlaySoundWithID(const SoundID& id, const GameSound::SoundType& soundType, bool isLooped);

    void ClearAll();
    void ClearEffects();
    void ClearSounds();
    void ClearSoundSources();

    AbstractSoundSource* GetSoundSourceFromType(const GameSound::SoundType& type) const;
    SoundEffect* GetSoundEffectFromType(const GameSound::EffectType& type) const;
    Sound* GetPlayingSound(SoundID soundID) const;
    void GetAllPlayingSoundsAsList(std::list<Sound*>& playingSounds) const;

    Sound* BuildSound(const GameSound::SoundType& soundType, bool isLooped, 
        const Point3D* position = NULL, bool applyActiveEffects = true);

    DISALLOW_COPY_AND_ASSIGN(GameSound);
};

inline bool GameSound::IsSoundPlaying(SoundID soundID) const {
    return this->GetPlayingSound(soundID) != NULL;
}

inline bool GameSound::IsEffectActive(GameSound::EffectType type) const {
    return this->activeEffects.find(type) != this->activeEffects.end();
}

inline bool GameSound::IsEffectPaused(GameSound::EffectType type) const {
    return this->pausedEffects.find(type) != this->pausedEffects.end();
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