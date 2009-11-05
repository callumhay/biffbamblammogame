#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include "SoundEvent.h"
#include "SoundMask.h"

#include <list>
#include <map>

/**
 * Holder of all the sound assets for the Biff Bam Blammo game. This class is
 * responsible for managing, playing, stopping, ticking, etc. all the sounds
 * in the game.
 */
class GameSoundAssets {

public:
	GameSoundAssets();
	~GameSoundAssets();

	void Tick(double dT);

	// Sound loader and unloaders
	void LoadWorldSounds(int worldStyle);
	void UnloadWorldSounds();
	// play, playAtPosition, playAtPositionWithVelocity, stop, pauseall

	void LoadMainMenuSounds();
	void UnloadMainMenuSounds();
	void PlayMainMenuSound(Sound::MainMenuSound sound);
	void StopMainMenuSound(Sound::MainMenuSound sound);

private:
	// World sound effect variables
	std::map<Sound::WorldSound, Sound*> worldSounds;

	// Main Menu sound effect variables
	std::map<Sound::MainMenuSound, Sound*> mainMenuSounds;

	// Active sound variables - these hold sounds that are currently active (e.g., being played) in the game
	std::list<Sound*> activeSounds;

	void LoadGlobalSounds();
	void UnloadGlobalSounds();

};
#endif // _GAMESOUNDASSETS_H_