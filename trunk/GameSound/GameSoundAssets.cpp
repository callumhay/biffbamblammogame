#include "GameSoundAssets.h"
#include "MSFReader.h"

#include "../GameView/GameViewConstants.h"

#include <cassert>

const int GameSoundAssets::MAX_MIX_GAME_SOUNDS = 64;

GameSoundAssets::GameSoundAssets() {
	int sdlMixerFlags  = MIX_INIT_OGG;
	int loadedSDLFlags = Mix_Init(sdlMixerFlags);
	if ((sdlMixerFlags & loadedSDLFlags) != sdlMixerFlags) {
		debug_output("Mix_Init: Failed to initialize certain sound type support:");
		debug_output(Mix_GetError() << std::endl);
	}

	// Set up the number of mixing channels for sound effects - some fairly large number
	// (should be large enough such that no more than that number of sounds are playing at
	// any given time).
	Mix_AllocateChannels(GameSoundAssets::MAX_MIX_GAME_SOUNDS);

	// Setup and load any sounds that will always be in memory
	this->LoadGlobalSounds();
}

GameSoundAssets::~GameSoundAssets() {
	this->UnloadMainMenuSounds(true);
	this->UnloadWorldSounds();
	this->UnloadGlobalSounds();

	Mix_Quit();
}

/**
 * Set the overall volume level of the game and all of its sound assets.
 * The given volume level is in the interval [0, 100].
 */
void GameSoundAssets::SetGameVolume(int volumeLvl) {
	float volumeFraction = static_cast<float>(std::max<int>(0, std::min<int>(100, volumeLvl))) / 100.0f;
	
	// Map the volume into the range of 0 to MIX_MAX_VOLUME
	int mixVolumeLvl = static_cast<int>(MIX_MAX_VOLUME * volumeFraction);
	// Set all channels (-1 means all channels) to the given volume level
	Mix_Volume(-1, mixVolumeLvl);
	Mix_VolumeMusic(mixVolumeLvl);
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

void GameSoundAssets::StopAllSounds() {
	for (std::list<Sound*>::iterator iter = this->activeSounds.begin(); iter != this->activeSounds.end(); ++iter) {
		Sound* sound = *iter;
		sound->Stop(true);
	}
	this->activeSounds.clear();
}

/**
 * Load the sounds associated with the main menu in the game.
 */
void GameSoundAssets::LoadMainMenuSounds() {
	// If it's the case that there are still main menu sounds loaded then we don't need to load them again!
	if (this->mainMenuSounds.size() != 0) {
		return;
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
	for (std::map<int, Sound*>::iterator iter = this->mainMenuSounds.begin(); iter != this->mainMenuSounds.end(); ++iter) {
		Sound* currSound = iter->second;

		// If wait for finish is set then we wait for each song to finish playing
		if (waitForFinish && !GameSoundAssets::IsSoundMask(iter->first)) {
			while (currSound->IsPlaying()) {}
		}

		// Stop the sound from being active and delete it
		this->StopMainMenuSound(static_cast<GameSoundAssets::MainMenuSound>(iter->first));
		delete currSound;
		currSound = NULL;
	}
	this->mainMenuSounds.clear();
}

/**
 * Play a sound associated with the main menu of the game. The sound may be an event or mask.
 */
void GameSoundAssets::PlayMainMenuSound(GameSoundAssets::MainMenuSound sound) {
	Sound* foundSound = GameSoundAssets::FindSound(this->mainMenuSounds, sound);
	if (foundSound == NULL) {
		return;
	}

	foundSound->Play(true);
	this->activeSounds.push_back(foundSound);
}

void GameSoundAssets::StopMainMenuSound(GameSoundAssets::MainMenuSound sound) {
	Sound* foundSound = GameSoundAssets::FindSound(this->mainMenuSounds, sound);
	if (foundSound == NULL) {
		return;
	}

	foundSound->Stop(true);
	this->activeSounds.remove(foundSound);
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool GameSoundAssets::IsSoundMask(int soundType) {
	return soundType == GameSoundAssets::MainMenuBackgroundMask;
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
 * Tries to find the given sound ID in the given sound map.
 * Returns: The sound object found, if nothing was found then NULL is returned.
 */
Sound* GameSoundAssets::FindSound(std::map<int, Sound*>& soundMap, int soundID) {
	std::map<int, Sound*>::iterator foundSoundIter = soundMap.find(soundID);
	
	// First make sure the sound exists, it should...
	if (foundSoundIter == soundMap.end()) {
		debug_output("Sound not found (sound id = " << soundID << ")");
		return NULL;
	}
	
	return foundSoundIter->second;
}