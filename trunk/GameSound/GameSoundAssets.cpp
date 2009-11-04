#include "GameSoundAssets.h"

GameSoundAssets::GameSoundAssets() {
	this->LoadGlobalSounds();
}

GameSoundAssets::~GameSoundAssets() {
}

/**
 * Called during the game event loop - this is passed the amount of
 * time passed since the previous frame of game play and is responsible
 * for updating sound effects over time.
 */
void GameSoundAssets::Tick(double dT) {
	// TODO
}

/**
 * Load the sound effects associated with the given world style. This will
 * load all sounds for the given world style into memory.
 */
void GameSoundAssets::LoadWorldSounds(int worldStyle) {
	// TODO
}

/**
 * Private helper function that will load all the global sound effects
 * for the game. Global sound effects are ALWAYS loaded.
 */
void GameSoundAssets::LoadGlobalSounds() {
	// TODO
}