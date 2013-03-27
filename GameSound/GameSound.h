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

        // Main menu sounds
		MainMenuBackgroundLoop,
		MenuItemChangedEvent, 
		MenuItemEnteredEvent,
		MenuItemCancelEvent,
		MenuItemVerifyAndSelectEvent, 
		MenuSelectionItemScrolledEvent,
        MenuScrollerItemScrolledEvent,
        MenuOpenSubMenuWindowEvent,
        MenuCloseSubMenuWindowEvent,

        // World-specific sounds
        WorldBackgroundLoop,

        // In-game event sounds
		PaddleHitWallEvent,
		PlayerLostABallButIsStillAliveEvent,
		LastBallExplodedEvent,
		BallSpawnEvent,

		BallBlockCollisionEvent,
		BallPaddleCollisionEvent,
		StickyBallPaddleCollisionEvent,
		BallBallCollisionEvent,
		BombBlockDestroyedEvent,
		InkBlockDestroyedEvent,
		BasicBlockDestroyedEvent,
		CannonBlockLoadedEvent,
		CannonBlockFiredEvent,
		PortalTeleportEvent,
		RocketExplodedEvent,

		BallSafetyNetCreatedEvent,
		BallSafetyNetDestroyedEvent,
		ItemSpawnedEvent,
		PowerUpItemActivatedEvent,
		PowerNeutralItemActivatedEvent,
		PowerDownItemActivatedEvent,
		PowerUpItemTimerEndsEvent,
		PowerNeutralItemTimerEndsEvent,
		PowerDownItemTimerEndsEvent,
		BallOrPaddleGrowEvent,
		BallOrPaddleShrinkEvent,
		LaserBulletShotEvent,
		LevelCompletedEvent,
		WorldCompletedEvent,

		// In-game looped sounds
		LaserBulletMovingLoop,
		RocketMovingLoop,
		LaserBeamFiringLoop,
		CollateralBlockFlashingLoop,
		CollateralBlockFallingLoop,
		CannonBlockRotatingLoop,
		LastBallSpiralingToDeathLoop,
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
    SoundID PlaySound(const GameSound::SoundType& soundType, bool isLooped, double fadeInTimeInSecs = 0.0);
    void StopSound(SoundID soundID, double fadeOutTimeInSecs = 0.0);

    // Game object sound attaching functions
    //void AttachAndPlaySound(const IPositionGameObj* posGameObj, const GameSound::SoundType& soundType, int numPlays);
    //void DetachAndStopSound(const IPositionGameObj* posGameObj, const GameSound::SoundType& soundType);
 
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


    // IrrKlang stuff
    irrklang::ISoundEngine* soundEngine;
    
    // Sound Source and Effects Maps
    SoundSourceMap globalSounds;
    EffectMap globalEffects;
    WorldSoundSourceMap worldSounds;
    GameWorld::WorldStyle currLoadedWorldStyle;
    
    // Active sounds (i.e., sounds that are playing in the game right now)
    SoundMap nonAttachedPlayingSounds;
    //AttachedSoundMap attachedSounds;


    SoundSource* BuildSoundSource(const std::string& soundName, const std::string& filePath);

    void ClearSounds();
    void ClearSoundSources();
    
    SoundSource* GetSoundSourceFromType(const GameSound::SoundType& type);
    Sound* GetPlayingSound(SoundID soundID);

    DISALLOW_COPY_AND_ASSIGN(GameSound);
};

#endif // __GAMESOUND_H__