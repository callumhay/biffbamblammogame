#include "GameSoundAssets.h"
#include "MSFReader.h"

#include "../GameView/GameViewConstants.h"

#include <cassert>

// Position and velocity of the listener
const ALfloat GameSoundAssets::DEFAULT_LISTENER_POS[3]	= {0.0, 0.0, 0.0};
const ALfloat GameSoundAssets::DEFAULT_LISTENER_VEL[3]	= {0.0, 0.0, 0.0};
// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
const ALfloat GameSoundAssets::DEFAULT_LISTENER_ORIENT[6]	= {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};


GameSoundAssets::GameSoundAssets() {
	// Setup OpenAL listener and load any sounds that will always be in memory
	GameSoundAssets::SetupOpenALListener();
	this->LoadGlobalSounds();
}

GameSoundAssets::~GameSoundAssets() {
	this->UnloadMainMenuSounds(true);
	this->UnloadWorldSounds();
	this->UnloadGlobalSounds();
}

/**
 * Called during the game event loop - this is passed the amount of
 * time passed since the previous frame of game play and is responsible
 * for updating sound effects over time.
 */
void GameSoundAssets::Tick(double dT) {
	// Go through each active sound and tick it
	GameSound* currSound = NULL;
	for (std::list<GameSound*>::iterator iter = this->activeSounds.begin(); iter != this->activeSounds.end(); ++iter) {
		currSound = *iter;
		currSound->Tick(dT);
	}
}

/**
 * Load the sound effects associated with the given world style. This will
 * load all sounds for the given world style into memory.
 */
void GameSoundAssets::LoadWorldSounds(int worldStyle) {
	// TODO
}

/**
 * Unload any sounds associated with world style that may have previously been loaded.
 */
void GameSoundAssets::UnloadWorldSounds() {
}

/**
 * Load the sounds associated with the main menu in the game.
 */
void GameSoundAssets::LoadMainMenuSounds() {
	// This shouldn't happen, but if it's the case that there are still main menu sounds loaded
	// that weren't previously cleaned up, clean them up first.
	if (this->mainMenuSounds.size() != 0) {
		assert(false);
		this->UnloadMainMenuSounds(false);
	}

	bool readSuccess = MSFReader::ReadMSF(GameViewConstants::GetInstance()->MAIN_MENU_SOUND_SCRIPT, this->mainMenuSounds);
	if (!readSuccess) {
		std::cerr << "Error reading main menu music script file: " << GameViewConstants::GetInstance()->MAIN_MENU_SOUND_SCRIPT << std::endl;
		return;
	}
}

/**
 * Unload the sounds associated with the main menu in the game that may have been previously loaded.
 */
void GameSoundAssets::UnloadMainMenuSounds(bool waitForFinish) {
	for (std::map<int, GameSound*>::iterator iter = this->mainMenuSounds.begin(); iter != this->mainMenuSounds.end(); ++iter) {
		GameSound* currSound = iter->second;

		// If wait for finish is set then we wait for each song to finish playing
		if (waitForFinish && !GameSound::IsSoundMask(iter->first)) {
			while (currSound->IsPlaying()) {}
		}

		// Stop the sound from being active and delete it
		this->StopMainMenuSound(static_cast<GameSound::MainMenuSound>(iter->first));
		delete currSound;
		currSound = NULL;
	}
	this->mainMenuSounds.clear();
}

/**
 * Play a sound associated with the main menu of the game. The sound may be an event or mask.
 */
void GameSoundAssets::PlayMainMenuSound(GameSound::MainMenuSound sound) {
	GameSound* foundSound = GameSoundAssets::FindSound(this->mainMenuSounds, sound);
	if (foundSound == NULL) {
		return;
	}

	foundSound->Play();
	this->activeSounds.push_back(foundSound);
}

void GameSoundAssets::StopMainMenuSound(GameSound::MainMenuSound sound) {
	GameSound* foundSound = GameSoundAssets::FindSound(this->mainMenuSounds, sound);
	if (foundSound == NULL) {
		return;
	}

	foundSound->Stop();
	this->activeSounds.remove(foundSound);
}

/**
 * Private helper function that will load all the global sound effects
 * for the game. Global sound effects are ALWAYS loaded.
 */
void GameSoundAssets::LoadGlobalSounds() {
	// TODO
}

/**
 * Private helper function that will unload all the global sound effects
 * for the game - this should only be called when the game is exiting.
 */
void GameSoundAssets::UnloadGlobalSounds() {
	// TODO
}

/**
 * Private helper for setting up the OpenAL listener position, velocity and orientation,
 * this determines where the sound is going to be interpretted in the game.
 */
void GameSoundAssets::SetupOpenALListener() {
	alListenerfv(AL_POSITION,    GameSoundAssets::DEFAULT_LISTENER_POS);
	alListenerfv(AL_VELOCITY,    GameSoundAssets::DEFAULT_LISTENER_VEL);
	alListenerfv(AL_ORIENTATION, GameSoundAssets::DEFAULT_LISTENER_ORIENT);
}

/**
 * Tries to find the given sound ID in the given sound map.
 * Returns: The sound object found, if nothing was found then NULL is returned.
 */
GameSound* GameSoundAssets::FindSound(std::map<int, GameSound*>& soundMap, int soundID) {
	std::map<int, GameSound*>::iterator foundSoundIter = soundMap.find(soundID);
	
	// First make sure the sound exists, it should...
	if (foundSoundIter == soundMap.end()) {
		debug_output("Sound not found (sound id = " << soundID << ")");
		return NULL;
	}
	
	return foundSoundIter->second;
}