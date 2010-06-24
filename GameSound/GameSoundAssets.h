#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include "Sound.h"

/**
 * Holder of all the sound assets for the Biff Bam Blammo game. This class is
 * responsible for managing, playing, stopping, ticking, etc. all the sounds
 * in the game.
 */
class GameSoundAssets {

public:
	static const int MAX_MIX_GAME_SOUNDS;

	// Per-World Event and Mask Sounds
	enum WorldSound { TODO };
	
	// Main Menu Event and Mask Sounds
	enum MainMenuSound { 
		// Masks
		MainMenuBackgroundMask = 0,
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

	void SetGameVolume(int volumeLvl);

	// Sound loader and unloaders
	void LoadWorldSounds(int worldStyle);
	void UnloadWorldSounds();
	
	void StopAllSounds();

	void LoadMainMenuSounds();
	void UnloadMainMenuSounds(bool waitForFinish);
	void PlayMainMenuSound(GameSoundAssets::MainMenuSound sound);
	void StopMainMenuSound(GameSoundAssets::MainMenuSound sound);


	static bool IsSoundMask(int soundType);

private:
	// World sound effect variables - indexed using Sound::WorldSound enum
	std::map<int, Sound*> worldSounds;

	// Main Menu sound effect variables - indexed using Sound::MaainMenuSound enum
	std::map<int, Sound*> mainMenuSounds;

	// Active sound variables - these hold sounds that are currently active (e.g., being played) in the game
	std::list<Sound*> activeSounds;

	void LoadGlobalSounds();
	void UnloadGlobalSounds();

	static Sound* FindSound(std::map<int, Sound*>& soundMap, int soundID);

};
#endif // _GAMESOUNDASSETS_H_