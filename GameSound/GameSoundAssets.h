#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_

#include "../BlammoEngine/BasicIncludes.h"

#include <list>
#include <map>

#include "GameSound.h"

/**
 * Holder of all the sound assets for the Biff Bam Blammo game. This class is
 * responsible for managing, playing, stopping, ticking, etc. all the sounds
 * in the game.
 */
class GameSoundAssets {

public:
	GameSoundAssets();
	~GameSoundAssets();

	void SetGameVolume(int volumeLvl);

	void Tick(double dT);

	// Sound loader and unloaders
	void LoadWorldSounds(int worldStyle);
	void UnloadWorldSounds();
	// play, playAtPosition, playAtPositionWithVelocity, stop, pauseall

	void LoadMainMenuSounds();
	void UnloadMainMenuSounds(bool waitForFinish);
	void PlayMainMenuSound(GameSound::MainMenuSound sound);
	void StopMainMenuSound(GameSound::MainMenuSound sound);

private:
	static const ALfloat DEFAULT_LISTENER_POS[3];
	static const ALfloat DEFAULT_LISTENER_VEL[3];
	static const ALfloat DEFAULT_LISTENER_ORIENT[6];

	// World sound effect variables - indexed using Sound::WorldSound enum
	std::map<int, GameSound*> worldSounds;

	// Main Menu sound effect variables - indexed using Sound::MainMenuSound enum
	std::map<int, GameSound*> mainMenuSounds;

	// Active sound variables - these hold sounds that are currently active (e.g., being played) in the game
	std::list<GameSound*> activeSounds;

	void LoadGlobalSounds();
	void UnloadGlobalSounds();

	static void SetupOpenALListener();
	static GameSound* FindSound(std::map<int, GameSound*>& soundMap, int soundID);

};
#endif // _GAMESOUNDASSETS_H_