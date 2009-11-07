#include "GameSoundAssets.h"

#include <cassert>

GameSoundAssets::GameSoundAssets() {
	this->LoadGlobalSounds();
}

GameSoundAssets::~GameSoundAssets() {
	this->UnloadMainMenuSounds();
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
	Sound* currSound = NULL;
	for (std::list<Sound*>::iterator iter = this->activeSounds.begin(); iter != this->activeSounds.end(); ++iter) {
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
}

/**
 * Unload the sounds associated with the main menu in the game that may have been previously loaded.
 */
void GameSoundAssets::UnloadMainMenuSounds() {
}

/**
 * Play a sound associated with the main menu of the game. The sound may be an event or mask.
 */
void GameSoundAssets::PlayMainMenuSound(Sound::MainMenuSound sound) {
	std::map<Sound::MainMenuSound, Sound*>::iterator foundSoundIter = this->mainMenuSounds.find(sound);
	
	// First make sure the sound exists, it should...
	if (foundSoundIter == this->mainMenuSounds.end()) {
		assert(false);
		return;
	}

	Sound* foundSound = foundSoundIter->second;
	foundSound->Play();
	this->activeSounds.push_back(foundSound);
}

void GameSoundAssets::StopMainMenuSound(Sound::MainMenuSound sound) {
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