#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include "Sound.h"

#include "../GameModel/GameWorld.h"

class MusicSound;
class EventSound;

/**
 * Holder of all the sound assets for the Biff Bam Blammo game. This class is
 * responsible for managing, playing, stopping, ticking, etc. all the sounds
 * in the game.
 */
class GameSoundAssets {

public:
	static const int MAX_MIX_GAME_SOUNDS;

	enum SoundPallet { MainMenuSoundPallet, DecoWorldSoundPallet };

	// Main Menu Event, Mask and Music Sounds
	enum MainMenuSound { 
		// Masks
		MainMenuBackgroundMusic = 0,
		// Events
		MainMenuBackgroundBangSmallEvent,
		MainMenuBackgroundBangMediumEvent,
		MainMenuBackgroundBangBigEvent,
		MainMenuItemHighlightedEvent, 
		MainMenuItemEnteredEvent,
		MainMenuItemBackAndCancelEvent,
		MainMenuItemVerifyAndSelectEvent, 
		MainMenuItemScrolledEvent 
	};

	GameSoundAssets();
	~GameSoundAssets();

	static void SetGameVolume(int volumeLvl);


	void LoadSoundPallet(GameSoundAssets::SoundPallet pallet);
	void UnloadSoundPallet(GameSoundAssets::SoundPallet pallet);
	void StopAllSounds();

	void PlayMainMenuSound(GameSoundAssets::MainMenuSound sound);
	void StopMainMenuSound(GameSoundAssets::MainMenuSound sound);


	static bool IsMaskSound(int soundType);
	static bool IsMusicSound(int soundType);
	static GameSoundAssets::SoundPallet GetSoundPalletFromWorldStyle(GameWorld::WorldStyle style);

private:
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>> loadedEventSounds;
	std::map<GameSoundAssets::SoundPallet, MusicSound*> loadedMusicSounds;

	MusicSound* activeMusic;						// Currently playing music
	std::list<EventSound*> activeMasks;	// Currently playing mask (looping) sounds

	void LoadMainMenuSounds();
	void UnloadMainMenuSounds();

	EventSound* FindEventSound(GameSoundAssets::SoundPallet pallet, int soundType) const;
	MusicSound* FindMusicSound(GameSoundAssets::SoundPallet pallet) const;

};
#endif // _GAMESOUNDASSETS_H_