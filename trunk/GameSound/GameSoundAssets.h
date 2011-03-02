/**
 * GameSoundAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include "Sound.h"
#include "EventSound.h"

#include "../GameModel/GameWorld.h"

#include "../GameView/GameViewConstants.h"

class MusicSound;

/**
 * Holder of all the sound assets for the Biff Bam Blammo game. This class is
 * responsible for managing, playing, stopping, ticking, etc. all the sounds
 * in the game.
 */
class GameSoundAssets {

public:
	static const int MAX_MIX_GAME_SOUNDS;

	enum SoundPallet { MainMenuSoundPallet, DecoWorldSoundPallet, FuturismWorldSoundPallet };

	// Main Menu Event, Mask and Music Sounds
	enum MainMenuSound { 
		// Music
		MainMenuBackgroundMusic = 0,
		// Events
		MainMenuBackgroundBangSmallEvent,
		MainMenuBackgroundBangMediumEvent,
		MainMenuBackgroundBangBigEvent,
		MainMenuItemChangedEvent, 
		MainMenuItemEnteredEvent,
		MainMenuItemBackAndCancelEvent,
		MainMenuItemVerifyAndSelectEvent, 
		MainMenuItemScrolledEvent 
	};

	// Per-world event, mask and music sounds
	enum WorldSound {
		// Music
		WorldBackgroundMusic = 0,
		// Events
		WorldSoundPaddleHitWallEvent,
		WorldSoundPlayerLostABallEvent,
		WorldSoundLastBallExplodedEvent,
		WorldSoundBallSpawnEvent,

		WorldSoundBallBlockCollisionEvent,
		WorldSoundBallPaddleCollisionEvent,
		WorldSoundStickyBallPaddleCollisionEvent,
		WorldSoundBallBallCollisionEvent,
		WorldSoundBombBlockDestroyedEvent,
		WorldSoundInkBlockDestroyedEvent,
		WorldSoundBasicBlockDestroyedEvent,
		WorldSoundCollateralBlockDestroyedEvent,
		WorldSoundCannonBlockLoadedEvent,
		WorldSoundCannonBlockFiredEvent,
		WorldSoundPortalTeleportEvent,
		WorldSoundRocketExplodedEvent,

		WorldSoundBallSafetyNetCreatedEvent,
		WorldSoundBallSafetyNetDestroyedEvent,
		WorldSoundItemSpawnedEvent,
		WorldSoundPowerUpItemActivatedEvent,
		WorldSoundPowerNeutralItemActivatedEvent,
		WorldSoundPowerDownItemActivatedEvent,
		WorldSoundPowerUpItemTimerEndsEvent,
		WorldSoundPowerNeutralItemTimerEndsEvent,
		WorldSoundPowerDownItemTimerEndsEvent,
		WorldSoundBallOrPaddleGrowEvent,
		WorldSoundBallOrPaddleShrinkEvent,
		WorldSoundLaserBulletShotEvent,
		WorldSoundLevelCompletedEvent,
		WorldSoundWorldCompletedEvent,

		// Masks
		WorldSoundInkSplatterMask,
		WorldSoundPoisonSicknessMask,
		WorldSoundLaserBulletMovingMask,
		WorldSoundRocketMovingMask,
		WorldSoundLaserBeamFiringMask,
		WorldSoundCollateralBlockFlashingMask,
		WorldSoundCollateralBlockFallingMask,
		WorldSoundCannonBlockRotatingMask,
		WorldSoundLastBallSpiralingToDeathMask,
		WorldSoundItemMovingMask
	};

	enum SoundVolumeLoudness { VeryQuietVolume = 0, QuietVolume = 1, NormalVolume = 2, LoudVolume = 3, VeryLoudVolume = 4 };

	GameSoundAssets();
	~GameSoundAssets();

	static void SetGameVolume(int volumeLvl);

	void LoadSoundPallet(GameSoundAssets::SoundPallet pallet);
	void UnloadSoundPallet(GameSoundAssets::SoundPallet pallet);
	void StopAllSounds();

	void PauseWorldSounds();
	void UnpauseWorldSounds();

	void PlayMainMenuSound(GameSoundAssets::MainMenuSound sound);
	void StopMainMenuSound(GameSoundAssets::MainMenuSound sound);
	
	void PlayWorldSound(GameSoundAssets::WorldSound sound, GameSoundAssets::SoundVolumeLoudness volume = GameSoundAssets::NormalVolume);
	void StopWorldSound(GameSoundAssets::WorldSound sound);
	bool IsWorldSoundPlaying(GameSoundAssets::WorldSound sound);
	
	void StopAllActiveMasks(bool fadeOut);

	void SetActiveWorldSounds(GameWorld::WorldStyle style, bool unloadPreviousActiveWorldSounds, 
														bool loadNewActiveWorldSounds);
	GameWorld::WorldStyle GetActiveWorldSounds() const;

	void PlaySoundGeneral(GameSoundAssets::SoundPallet pallet, int sound, GameSoundAssets::SoundVolumeLoudness volume = GameSoundAssets::NormalVolume);
	void StopSoundGeneral(GameSoundAssets::SoundPallet pallet, int sound);

	// More specific functions for playing sounds related to particular things...
	void PlayBallHitBlockEvent(const GameBall& ball, const LevelPiece& block, GameSoundAssets::SoundVolumeLoudness volume = GameSoundAssets::NormalVolume);


	static bool IsMaskSound(int soundType);
	static bool IsMusicSound(int soundType);
	static GameSoundAssets::SoundPallet GetSoundPalletFromWorldStyle(GameWorld::WorldStyle style);

private:
	static int BASE_MUSIC_SOUND_VOLUME;
	static int BASE_EVENT_AND_MASK_SOUND_VOLUME;

	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>> loadedEventSounds;
	std::map<GameSoundAssets::SoundPallet, MusicSound*> loadedMusicSounds;

	MusicSound* activeMusic;						// Currently playing music
	std::list<EventSound*> activeMasks;	// Currently playing mask (looping) sounds

	GameWorld::WorldStyle activeWorld;

	void LoadMainMenuSounds();
	void UnloadMainMenuSounds();
	void LoadDecoWorldSounds();
	void UnloadDecoWorldSounds();
    void LoadFuturismWorldSounds();
    void UnloadFuturismWorldSounds();

	void LoadSounds(const std::string& filepath, GameSoundAssets::SoundPallet pallet);
	void UnloadSounds(GameSoundAssets::SoundPallet pallet);

	EventSound* FindEventSound(GameSoundAssets::SoundPallet pallet, int soundType) const;
	MusicSound* FindMusicSound(GameSoundAssets::SoundPallet pallet) const;

	static int CalculateFinalVolume(Sound::SoundType soundType, GameSoundAssets::SoundVolumeLoudness loudness);
};

/**
 * Play a sound associated with the main menu of the game. The sound may be an event or mask.
 */
inline void GameSoundAssets::PlayMainMenuSound(GameSoundAssets::MainMenuSound sound) {
	this->PlaySoundGeneral(GameSoundAssets::MainMenuSoundPallet, sound);
}

inline void GameSoundAssets::StopMainMenuSound(GameSoundAssets::MainMenuSound sound) {
	this->StopSoundGeneral(GameSoundAssets::MainMenuSoundPallet, sound);
}

inline void GameSoundAssets::PlayWorldSound(GameSoundAssets::WorldSound sound, GameSoundAssets::SoundVolumeLoudness volume) {
	this->PlaySoundGeneral(GameSoundAssets::GetSoundPalletFromWorldStyle(this->activeWorld), sound, volume);
}

inline void GameSoundAssets::StopWorldSound(GameSoundAssets::WorldSound sound) {
	this->StopSoundGeneral(GameSoundAssets::GetSoundPalletFromWorldStyle(this->activeWorld), sound);
}

inline bool GameSoundAssets::IsWorldSoundPlaying(GameSoundAssets::WorldSound sound) {
	EventSound* eventSound =  this->FindEventSound(GameSoundAssets::GetSoundPalletFromWorldStyle(this->activeWorld), sound);
	assert(eventSound != NULL);
	return eventSound->IsPlaying();
}

// Stop all active mask sounds
inline void GameSoundAssets::StopAllActiveMasks(bool fadeOut) {
	for (std::list<EventSound*>::iterator iter = this->activeMasks.begin(); iter != this->activeMasks.end(); ++iter) {
		EventSound* currMask = *iter;
		while (currMask->IsPlaying()) {
			currMask->Stop(fadeOut);
		}
	}
}

// Sets the currently active world - this is the style of the world so that whenever PlayWorldSound/StopWorldSound
// are called they will play the currently active world style music.
// unloadPreviousActiveWorldSounds : set this to true to unload the previous world sound pallet
// loadNewActiveWorldSounds        : set this to true to load the given 'style' world sound assets
inline void GameSoundAssets::SetActiveWorldSounds(GameWorld::WorldStyle style,
                                                  bool unloadPreviousActiveWorldSounds, 
												  bool loadNewActiveWorldSounds) {
	if (unloadPreviousActiveWorldSounds) {
		if (this->activeWorld != GameWorld::None) {
			this->UnloadSoundPallet(GameSoundAssets::GetSoundPalletFromWorldStyle(this->activeWorld));
		}
	}
	this->activeWorld = style;
	if (loadNewActiveWorldSounds) {
		this->LoadSoundPallet(GameSoundAssets::GetSoundPalletFromWorldStyle(this->activeWorld));
	}
}

inline GameWorld::WorldStyle GameSoundAssets::GetActiveWorldSounds() const {
	return this->activeWorld;
}

/**
 * Load the sounds associated with the main menu in the game.
 */
inline void GameSoundAssets::LoadMainMenuSounds() {
	this->LoadSounds(GameViewConstants::GetInstance()->MAIN_MENU_SOUND_SCRIPT, GameSoundAssets::MainMenuSoundPallet);
}

/**
 * Unload the sounds associated with the main menu in the game that may have been previously loaded.
 */
inline void GameSoundAssets::UnloadMainMenuSounds() {
	this->UnloadSounds(GameSoundAssets::MainMenuSoundPallet);
}

inline void GameSoundAssets::LoadDecoWorldSounds() {
	this->LoadSounds(GameViewConstants::GetInstance()->DECO_SOUND_SCRIPT, GameSoundAssets::DecoWorldSoundPallet);
}

inline void GameSoundAssets::UnloadDecoWorldSounds() {
	this->UnloadSounds(GameSoundAssets::DecoWorldSoundPallet);
}

inline void GameSoundAssets::LoadFuturismWorldSounds() {
	this->LoadSounds(GameViewConstants::GetInstance()->FUTURISM_SOUND_SCRIPT, GameSoundAssets::FuturismWorldSoundPallet);
}

inline void GameSoundAssets::UnloadFuturismWorldSounds() {
    this->UnloadSounds(GameSoundAssets::FuturismWorldSoundPallet);
}

#endif // _GAMESOUNDASSETS_H_