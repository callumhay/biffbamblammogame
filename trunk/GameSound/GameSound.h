/**
 * GameSound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMESOUND_H__
#define __GAMESOUND_H__

#include "../GameModel/GameWorld.h"

#include "SoundCommon.h"

// IrrKlang forward declarations
namespace irrklang {
    class ISoundEngine;
}

// GameSound forward declarations
class Sound;
class SoundEffect;
class SoundSource;

// BlammoEngine forward declarations
class Camera;
class IPositionObject;

class GameSound {
    friend class MSFReader;
public:
    enum EffectType {
        NoEffect = -1,
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
        LevelMenuPageChangedSelectionEvent,
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
		BallSpawnEvent,
        BallBallCollisionEvent,
        LastBallSpiralingToDeathLoop,

        // -> Paddle/ball sounds
        BallPaddleCollisionEvent,
        BallStickyPaddleCollisionEvent,
        BallShieldPaddleCollisionEvent,
		BallOrPaddleGrowEvent,
		BallOrPaddleShrinkEvent,
        
        // -> Ball/block sounds
		BallBlockCollisionEvent,
		
		// -> Block-only sounds
		BombBlockDestroyedEvent,
		InkBlockDestroyedEvent,
		BasicBlockDestroyedEvent,
		CannonBlockLoadedEvent,
		CannonBlockFiredEvent,
		PortalTeleportEvent,
		CollateralBlockFlashingLoop,
		CollateralBlockFallingLoop,
        CannonBlockRotatingLoop,

        // -> Projectile and beam sounds
		RocketExplodedEvent,
        RocketMovingLoop,
        MineExplodedEvent,
        LaserBulletShotEvent,
		LaserBulletMovingLoop,
		LaserBeamFiringLoop,

		BallSafetyNetCreatedEvent,
		BallSafetyNetDestroyedEvent,
		ItemSpawnedEvent,
		PowerUpItemActivatedEvent,
		PowerNeutralItemActivatedEvent,
		PowerDownItemActivatedEvent,
		PowerUpItemTimerEndsEvent,
		PowerNeutralItemTimerEndsEvent,
		PowerDownItemTimerEndsEvent,
        ItemMovingLoop
    };

    typedef std::map<GameSound::SoundType, SoundSource*> SoundSourceMap;
    typedef SoundSourceMap::iterator SoundSourceMapIter;

    typedef std::map<GameSound::EffectType, SoundEffect*> EffectMap;
    typedef EffectMap::iterator EffectMapIter;

    typedef std::map<GameWorld::WorldStyle, SoundSourceMap> WorldSoundSourceMap;
    typedef WorldSoundSourceMap::iterator WorldSoundSourceMapIter;

    GameSound();
    ~GameSound();

    // Intitialization function (MUST BE CALLED FIRST!)
    bool Init();

    // Tick function (updates the sounds that are attached to moving objects and any sound animations)
    void Tick(double dT);

    // Sound loading functions
    void LoadGlobalSounds();
    void LoadWorldSounds(const GameWorld::WorldStyle& world);
    void ReloadFromMSF();

    // Play/stop functions
    void StopAllSounds();
    void PauseAllSounds();
    void UnpauseAllSounds();
    SoundID PlaySound(const GameSound::SoundType& soundType, bool isLooped);
    SoundID PlaySoundAtPosition(const GameSound::SoundType& soundType, bool isLooped, const Point3D& position);
    void StopSound(SoundID soundID, double fadeOutTimeInSecs = 0.0);

    // Game object positional sound attaching/detaching functions
    SoundID AttachAndPlaySound(const IPositionObject* posObj, const GameSound::SoundType& soundType, bool isLooped);
    void DetachAndStopAllSounds(const IPositionObject* posObj);
    void SetPauseForAllAttachedSounds(const IPositionObject* posObj, bool isPaused);
 
    // Volume functions
    void SetMasterVolume(float volume);
    //void SetAllPlayingSoundsVolume(float volume, double lerpTimeInSecs = 0.0);

    // 3D/2D Sound functions
    void SetListenerPosition(const Camera& camera);
    
    // Query functions
    bool IsSoundPlaying(SoundID soundID) const;

private:
    typedef std::map<SoundID, Sound*> SoundMap;
    typedef SoundMap::iterator SoundMapIter;

    typedef std::map<const IPositionObject*, SoundMap> AttachedSoundMap;
    typedef AttachedSoundMap::iterator AttachedSoundMapIter;

    // Sound Source and Effects Maps
    SoundSourceMap globalSounds;
    EffectMap globalEffects;
    WorldSoundSourceMap worldSounds;
    GameWorld::WorldStyle currLoadedWorldStyle;
    
    // Active sounds (i.e., sounds that are playing in the game right now)
    SoundMap nonAttachedPlayingSounds;
    AttachedSoundMap attachedPlayingSounds;

    // IrrKlang stuff
    irrklang::ISoundEngine* soundEngine;

    // Helper functions
    SoundSource* BuildSoundSource(const GameSound::SoundType& soundType,
        const std::string& soundName, const std::string& filePath);

    bool LoadFromMSF();

    bool PlaySoundWithID(const SoundID& id, const GameSound::SoundType& soundType, bool isLooped);

    void ClearAll();
    void ClearEffects();
    void ClearSounds();
    void ClearSoundSources();

    SoundSource* GetSoundSourceFromType(const GameSound::SoundType& type);
    Sound* GetPlayingSound(SoundID soundID);

    Sound* BuildSound(const GameSound::SoundType& soundType, bool isLooped, const Point3D* position = NULL);

    DISALLOW_COPY_AND_ASSIGN(GameSound);
};

#endif // __GAMESOUND_H__