#include "GameSoundAssets.h"
#include "MusicSound.h"
#include "EventSound.h"
#include "MSFReader.h"


#include <cassert>

const int GameSoundAssets::MAX_MIX_GAME_SOUNDS = 64;

GameSoundAssets::GameSoundAssets() : activeMusic(NULL), activeWorld(GameWorld::None) {
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
	//this->LoadGlobalSounds();
}

GameSoundAssets::~GameSoundAssets() {
	// Stop all active sounds/music
	this->StopAllSounds();

	// Clean up all loaded sound pallets
	for (std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator iter1 = this->loadedEventSounds.begin();
		iter1 != this->loadedEventSounds.end(); ++iter1) {
		
		std::map<int, EventSound*>& eventSounds = iter1->second;
		for (std::map<int, EventSound*>::iterator iter2 = eventSounds.begin(); iter2 != eventSounds.end(); ++iter2) {
			EventSound* currSound = iter2->second;
			delete currSound;
			currSound = NULL;
		}
		eventSounds.clear();
	}
	this->loadedEventSounds.clear();

	for (std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator iter = this->loadedMusicSounds.begin();
		iter != this->loadedMusicSounds.end(); ++iter) {
		
		MusicSound* currMusic = iter->second;
		delete currMusic;
		currMusic = NULL;
	}
	this->loadedMusicSounds.clear();

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

void GameSoundAssets::LoadSoundPallet(GameSoundAssets::SoundPallet pallet) {
	switch (pallet) {
		case GameSoundAssets::MainMenuSoundPallet:
			this->LoadMainMenuSounds();
			break;
		case GameSoundAssets::DecoWorldSoundPallet:
			this->LoadDecoWorldSounds();
			break;
		default:
			assert(false);
			break;
	}
}

void GameSoundAssets::UnloadSoundPallet(GameSoundAssets::SoundPallet pallet) {
	switch (pallet) {
		case GameSoundAssets::MainMenuSoundPallet:
			this->UnloadMainMenuSounds();
			break;
		case GameSoundAssets::DecoWorldSoundPallet:
			this->UnloadDecoWorldSounds();
			break;
		default:
			assert(false);
			break;
	}
}

/**
 * Stops all active sounds.
 */
void GameSoundAssets::StopAllSounds() {
	// Stop every possible loaded sound event, mask and music
	for (std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator iter1 = this->loadedEventSounds.begin();
		iter1 != this->loadedEventSounds.end(); ++iter1) {
		
		std::map<int, EventSound*>& eventSounds = iter1->second;
		for (std::map<int, EventSound*>::iterator iter2 = eventSounds.begin(); iter2 != eventSounds.end(); ++iter2) {
			EventSound* currSound = iter2->second;
			assert(currSound != NULL);
			currSound->Stop(true);
		}
	}
	for (std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator iter = this->loadedMusicSounds.begin();
		iter != this->loadedMusicSounds.end(); ++iter) {
		
		MusicSound* currMusic = iter->second;
		assert(currMusic != NULL);
		currMusic->Stop(true);
	}

	// Clear all active sounds
	this->activeMusic = NULL;
	this->activeMasks.clear();
}





/**
 * General version of the play sound * function.
 */
void GameSoundAssets::PlaySound(GameSoundAssets::SoundPallet pallet, int sound) {
	MusicSound* musicFoundSound = NULL;
	EventSound* eventFoundSound = NULL;

	if (GameSoundAssets::IsMusicSound(sound)) {
		musicFoundSound = this->FindMusicSound(pallet);
		if (musicFoundSound == NULL) {
			return;
		}

		// If there was already active music then stop it and replace it with the new music
		if (this->activeMusic != musicFoundSound && this->activeMusic != NULL) {
			this->activeMusic->Stop(false);
		}
		musicFoundSound->Play(true);
		this->activeMusic = musicFoundSound;
	}
	else {
		eventFoundSound = this->FindEventSound(pallet, sound);
		if (eventFoundSound == NULL) {
			return;
		}
		eventFoundSound->Play(true);
		if (eventFoundSound->GetType() == Sound::MaskSound) {
			this->activeMasks.push_back(eventFoundSound);
		}
	}
}

void GameSoundAssets::StopSound(GameSoundAssets::SoundPallet pallet, int sound) {
	if (GameSoundAssets::IsMusicSound(sound)) {
		MusicSound* foundMusic = this->FindMusicSound(pallet);
		if (foundMusic == NULL) {
			return;
		}

		if (this->activeMusic == foundMusic) {
			this->activeMusic->Stop(true);
			this->activeMusic = NULL;
		}
	}
	else {
		EventSound* foundSound = this->FindEventSound(pallet, sound);
		if (foundSound == NULL) {
			return;
		}
		foundSound->Stop(true);
		if (foundSound->GetType() == Sound::MaskSound) {
			this->activeMasks.remove(foundSound);
		}
	}
}


void GameSoundAssets::LoadSounds(const std::string& filepath, GameSoundAssets::SoundPallet pallet) {
	// If it's the case that there are still main menu sounds loaded then we don't need to load them again!
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator findSoundEventIter = 
		this->loadedEventSounds.find(pallet);
	std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator findMusicEventIter =
		this->loadedMusicSounds.find(pallet);
	if (findSoundEventIter != this->loadedEventSounds.end()) {
		//assert(findMusicEventIter != this->loadedMusicSounds.end());
		return;
	}

	std::map<int, Sound*> loadedSounds;
	bool readSuccess = MSFReader::ReadMSF(filepath, loadedSounds);
	if (!readSuccess) {
		std::cerr << "Error reading main menu music script file: " << filepath << std::endl;
		return;
	}

	assert(findSoundEventIter == this->loadedEventSounds.end());	
	assert(findMusicEventIter == this->loadedMusicSounds.end());

	//MusicSound* emptyMusic = NULL;
	this->loadedEventSounds.insert(std::make_pair(pallet, std::map<int, EventSound*>()));
	//this->loadedMusicSounds.insert(std::make_pair(GameSoundAssets::MainMenuSoundPallet, emptyMusic));

	for (std::map<int, Sound*>::iterator iter = loadedSounds.begin(); iter != loadedSounds.end(); ++iter) {
		Sound* currSound = iter->second;
		if (currSound == NULL) {
			assert(false);
			continue;
		}
		
		switch (currSound->GetType()) {
			case Sound::EventSound:
			case Sound::MaskSound: {
					EventSound* eventSound = dynamic_cast<EventSound*>(currSound);
					assert(eventSound != NULL);
					this->loadedEventSounds[pallet].insert(std::make_pair(iter->first, eventSound));	
				}
				break;

			case Sound::MusicSound: {
					MusicSound* musicSound = dynamic_cast<MusicSound*>(currSound);
					assert(musicSound != NULL);
					this->loadedMusicSounds[pallet] = musicSound;
				}
				break;

			default:
				assert(false);
				break;
		}
	}
}

void GameSoundAssets::UnloadSounds(GameSoundAssets::SoundPallet pallet) {
	// Check to see if the sounds are even loaded
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator findSoundEventIter = 
		this->loadedEventSounds.find(pallet);
	std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator findMusicEventIter =
		this->loadedMusicSounds.find(pallet);
	if (findSoundEventIter == this->loadedEventSounds.end()) {
		assert(findMusicEventIter == this->loadedMusicSounds.end());
		return;
	}

	// Sounds are loaded - unload them by stopping and deleting them all
	std::map<int, EventSound*>& eventSounds = findSoundEventIter->second;
	MusicSound* musicSound = findMusicEventIter->second;

	for (std::map<int, EventSound*>::iterator iter2 = eventSounds.begin(); iter2 != eventSounds.end(); ++iter2) {

		this->StopSound(pallet, static_cast<GameSoundAssets::SoundPallet>(iter2->first));

		EventSound* currSound = iter2->second;
		delete currSound;
		currSound = NULL;
	}

	if (musicSound != NULL) {
		if (this->activeMusic == musicSound) {
			this->activeMusic = NULL;
		}
		delete musicSound;
		musicSound = NULL;
	}

	this->loadedEventSounds.erase(pallet);
	this->loadedMusicSounds.erase(pallet);
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool GameSoundAssets::IsMaskSound(int soundType) {
	return false;
}

bool GameSoundAssets::IsMusicSound(int soundType) {
	return soundType == GameSoundAssets::MainMenuBackgroundMusic;
}

/**
 * Get the sound pallet type based on the given world style.
 */
GameSoundAssets::SoundPallet GameSoundAssets::GetSoundPalletFromWorldStyle(GameWorld::WorldStyle style) {
	switch (style) {
		case GameWorld::Deco:
			return GameSoundAssets::DecoWorldSoundPallet;
		default:
			assert(false);
			break;
	}
	return GameSoundAssets::MainMenuSoundPallet;
}

/**
 * Tries to find the given sound ID in the given sound map.
 * Returns: The sound object found, if nothing (likely a music sound not an event/mask) was found then NULL is returned.
 */
EventSound* GameSoundAssets::FindEventSound(GameSoundAssets::SoundPallet pallet, int soundType) const {
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::const_iterator findIter = this->loadedEventSounds.find(pallet);
	if (findIter == this->loadedEventSounds.end()) {
		return NULL;
	}

	const std::map<int, EventSound*>& eventSoundList = findIter->second;
	std::map<int, EventSound*>::const_iterator foundSoundIter = eventSoundList.find(soundType);
	
	// First make sure the sound exists, it should...
	if (foundSoundIter == eventSoundList.end()) {
		debug_output("Sound not found (sound type = " << soundType << ")");
		return NULL;
	}
	
	return foundSoundIter->second;
}

MusicSound* GameSoundAssets::FindMusicSound(GameSoundAssets::SoundPallet pallet) const {
	std::map<GameSoundAssets::SoundPallet, MusicSound*>::const_iterator findIter = this->loadedMusicSounds.find(pallet);
	if (findIter == this->loadedMusicSounds.end()) {
		return NULL;
	}

	return findIter->second;
}