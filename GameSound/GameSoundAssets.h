#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include <list>
#include <map>

#include "Sound.h"

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
	void UnloadMainMenuSounds(bool waitForFinish);
	void PlayMainMenuSound(Sound::MainMenuSound sound);
	void StopMainMenuSound(Sound::MainMenuSound sound);

private:
	static const ALfloat DEFAULT_LISTENER_POS[3];
	static const ALfloat DEFAULT_LISTENER_VEL[3];
	static const ALfloat DEFAULT_LISTENER_ORIENT[6];

	// World sound effect variables - indexed using Sound::WorldSound enum
	std::map<int, Sound*> worldSounds;

	// Main Menu sound effect variables - indexed using Sound::MainMenuSound enum
	std::map<int, Sound*> mainMenuSounds;

	// Active sound variables - these hold sounds that are currently active (e.g., being played) in the game
	std::list<Sound*> activeSounds;

	void LoadGlobalSounds();
	void UnloadGlobalSounds();

	static void SetupOpenALListener();
	static Sound* FindSound(std::map<int, Sound*>& soundMap, int soundID);

};
#endif // _GAMESOUNDASSETS_H_