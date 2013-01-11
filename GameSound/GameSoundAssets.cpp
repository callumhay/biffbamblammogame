/**
 * GameSoundAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameSoundAssets.h"
#include "MusicSound.h"
#include "EventSound.h"
#include "MSFReader.h"

#include "../GameModel/LevelPiece.h"
#include "../GameModel/CannonBlock.h"
#include "../GameModel/GameBall.h"

#include <cassert>

const int GameSoundAssets::MAX_MIX_GAME_SOUNDS = 1024;
int GameSoundAssets::BASE_MUSIC_SOUND_VOLUME = MIX_MAX_VOLUME;
int GameSoundAssets::BASE_EVENT_AND_MASK_SOUND_VOLUME = MIX_MAX_VOLUME;

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

	// We set the base level of the max volume - this is because we want to be able to adjust relative levels 
	// (e.g., more potent effect sounds might be louder than typical sounds) of volume 
	// for various effects in the game.
	GameSoundAssets::BASE_MUSIC_SOUND_VOLUME					= mixVolumeLvl;
	GameSoundAssets::BASE_EVENT_AND_MASK_SOUND_VOLUME = mixVolumeLvl;

	// Set all channels (-1 means all channels) to the given volume level
	Mix_Volume(-1, GameSoundAssets::BASE_EVENT_AND_MASK_SOUND_VOLUME);
	Mix_VolumeMusic(GameSoundAssets::BASE_MUSIC_SOUND_VOLUME);
}

void GameSoundAssets::LoadSoundPallet(GameSoundAssets::SoundPallet pallet) {
	switch (pallet) {
		case GameSoundAssets::MainMenuSoundPallet:
			this->LoadMainMenuSounds();
			break;
        case GameSoundAssets::ClassicalSoundPallet:
            // TODO
            break;
		case GameSoundAssets::DecoWorldSoundPallet:
			this->LoadDecoWorldSounds();
			break;
        case GameSoundAssets::FuturismWorldSoundPallet:
            this->LoadFuturismWorldSounds();
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
        case GameSoundAssets::ClassicalSoundPallet:
            break;
		case GameSoundAssets::DecoWorldSoundPallet:
			this->UnloadDecoWorldSounds();
			break;
        case GameSoundAssets::FuturismWorldSoundPallet:
            this->UnloadFuturismWorldSounds();
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

void GameSoundAssets::PauseWorldSounds() {
	// If no world is active then just exit
	if (this->activeWorld == GameWorld::None) {
		return;
	}

	debug_output("Pausing world sounds.");
	GameSoundAssets::SoundPallet pallet = this->GetSoundPalletFromWorldStyle(this->activeWorld);
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator findIter = this->loadedEventSounds.find(pallet);
	
	// Pause all sound events/masks for the given pallet
	if (findIter != this->loadedEventSounds.end()) {
		std::map<int, EventSound*>& eventSounds = findIter->second;
		for (std::map<int, EventSound*>::iterator iter2 = eventSounds.begin(); iter2 != eventSounds.end(); ++iter2) {
			EventSound* currSound = iter2->second;
			assert(currSound != NULL);
			currSound->Pause();
		}
	}

	if (this->activeMusic != NULL) {
		// Check to see if the music is in the provided pallet
		for (std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator iter = this->loadedMusicSounds.begin();
				 iter != this->loadedMusicSounds.end(); ++iter) {
			
			if (iter->second == this->activeMusic) {
				if (iter->first == pallet) {
					this->activeMusic->Pause();
					this->activeMusic = NULL;
				}
				break;
			}
		}
	}

}

void GameSoundAssets::UnpauseWorldSounds() {
	// If no world is active then just exit
	if (this->activeWorld == GameWorld::None) {
		return;
	}	
	
	debug_output("Unpausing world sounds.");
	GameSoundAssets::SoundPallet pallet = this->GetSoundPalletFromWorldStyle(this->activeWorld);
	std::map<GameSoundAssets::SoundPallet, std::map<int, EventSound*>>::iterator findEventIter = this->loadedEventSounds.find(pallet);
	
	// Unpause all sound events/masks for the given pallet
	if (findEventIter != this->loadedEventSounds.end()) {
		std::map<int, EventSound*>& eventSounds = findEventIter->second;
		for (std::map<int, EventSound*>::iterator iter2 = eventSounds.begin(); iter2 != eventSounds.end(); ++iter2) {
			EventSound* currSound = iter2->second;
			assert(currSound != NULL);
			currSound->UnPause();
		}
	}

	std::map<GameSoundAssets::SoundPallet, MusicSound*>::iterator findMusicIter = this->loadedMusicSounds.find(pallet);

	// Unpause the music for the given pallet
	if (findMusicIter != this->loadedMusicSounds.end()) {
		MusicSound* currMusic = findMusicIter->second;

		// Check to see if the music is paused, if it is then we stop the currently
		// active music (if there is any) and resume the paused music
		if (currMusic->IsPaused()) {
			if (this->activeMusic != NULL) {
				this->activeMusic->Stop(false);
			}
			this->activeMusic = currMusic;
			this->activeMusic->UnPause();
		}
	}
}

/**
 * General version of the play sound * function.
 */
void GameSoundAssets::PlaySoundGeneral(GameSoundAssets::SoundPallet pallet, int sound, GameSoundAssets::SoundVolumeLoudness volume) {
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

		int finalMusicVolume = GameSoundAssets::CalculateFinalVolume(Sound::MusicSound, volume);
		musicFoundSound->SetVolume(finalMusicVolume);
		musicFoundSound->Play(true);

		this->activeMusic = musicFoundSound;
	}
	else {
		eventFoundSound = this->FindEventSound(pallet, sound);
		if (eventFoundSound == NULL) {
			return;
		}

		int finalEventVolume = GameSoundAssets::CalculateFinalVolume(eventFoundSound->GetType(), volume);
		eventFoundSound->SetVolume(finalEventVolume);
		eventFoundSound->Play(true);

		if (eventFoundSound->GetType() == Sound::MaskSound) {
			this->activeMasks.push_back(eventFoundSound);
		}
	}
}

void GameSoundAssets::StopSoundGeneral(GameSoundAssets::SoundPallet pallet, int sound) {
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

void GameSoundAssets::PlayBallHitBlockEvent(const GameBall& ball, const LevelPiece& block, GameSoundAssets::SoundVolumeLoudness volume) {
	// Play a sound for the ball colliding with the block based on the block...
	switch (block.GetType()) {
		case LevelPiece::Solid:
		case LevelPiece::SolidTriangle:
		case LevelPiece::Breakable:
		case LevelPiece::BreakableTriangle:
		case LevelPiece::Collateral:
		case LevelPiece::Bomb:
		case LevelPiece::Prism:
		case LevelPiece::PrismTriangle:
		case LevelPiece::Tesla:
		case LevelPiece::ItemDrop:
        case LevelPiece::OneWay:
        case LevelPiece::NoEntry:
        case LevelPiece::LaserTurret:
        case LevelPiece::RocketTurret:
        case LevelPiece::MineTurret:
			this->PlayWorldSound(GameSoundAssets::WorldSoundBallBlockCollisionEvent, volume);
			break;
		
        case LevelPiece::Switch:
            break;

		case LevelPiece::Ink:
		case LevelPiece::Empty:
		case LevelPiece::Portal:
			break;

		case LevelPiece::Cannon: {
				const CannonBlock* cannonBlock = static_cast<const CannonBlock*>(&block);
				if (!cannonBlock->GetIsLoaded() && !ball.IsLastPieceCollidedWith(&block)) {
					// The ball will be loaded into the cannon...
					this->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockLoadedEvent,  GameSoundAssets::NormalVolume);
					this->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockRotatingMask, GameSoundAssets::QuietVolume);
				}
				else if (!ball.IsLastPieceCollidedWith(&block)) {
					// Ball didn't go into the cannon... it must have already been loaded, play typical block hit sound
					this->PlayWorldSound(GameSoundAssets::WorldSoundBallBlockCollisionEvent, volume);
				}
			}
			break;


		default:
			assert(false);
			break;
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
      // Clean up...
      for (std::map<int, Sound*>::iterator iter = loadedSounds.begin(); iter != loadedSounds.end(); ++iter) {
         Sound* sound = iter->second;
         delete sound;
         sound = NULL;
      }
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
					EventSound* eventSound = static_cast<EventSound*>(currSound);
					assert(eventSound != NULL);
                    eventSound->SetVolume(GameSoundAssets::CalculateFinalVolume(eventSound->GetType(), GameSoundAssets::NormalVolume));
					this->loadedEventSounds[pallet].insert(std::make_pair(iter->first, eventSound));	
				}
				break;

			case Sound::MusicSound: {
					MusicSound* musicSound = static_cast<MusicSound*>(currSound);
					assert(musicSound != NULL);
                    musicSound->SetVolume(GameSoundAssets::CalculateFinalVolume(Sound::MusicSound, GameSoundAssets::NormalVolume));
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

		this->StopSoundGeneral(pallet, static_cast<GameSoundAssets::SoundPallet>(iter2->first));

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
	return soundType == WorldSoundInkSplatterMask ||
				 soundType == WorldSoundPoisonSicknessMask ||
				 soundType == WorldSoundLaserBulletMovingMask ||
				 soundType == WorldSoundRocketMovingMask ||
				 soundType == WorldSoundLaserBeamFiringMask ||
			   soundType == WorldSoundCollateralBlockFlashingMask ||
				 soundType == WorldSoundCollateralBlockFallingMask ||
				 soundType == WorldSoundCannonBlockRotatingMask ||
				 soundType == WorldSoundLastBallSpiralingToDeathMask;
}

bool GameSoundAssets::IsMusicSound(int soundType) {
	return soundType == GameSoundAssets::MainMenuBackgroundMusic || 
           soundType == GameSoundAssets::WorldBackgroundMusic;
}

/**
 * Get the sound pallet type based on the given world style.
 */
GameSoundAssets::SoundPallet GameSoundAssets::GetSoundPalletFromWorldStyle(GameWorld::WorldStyle style) {
	switch (style) {
        case GameWorld::Classical:
            return GameSoundAssets::ClassicalSoundPallet;
		case GameWorld::Deco:
			return GameSoundAssets::DecoWorldSoundPallet;
        case GameWorld::Futurism:
            return GameSoundAssets::FuturismWorldSoundPallet;
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

/**
 * Calculates the final volume based on the given sound type and the loudness required
 * for that sound. This will use the base, set volume for that sound type and modify it based on 
 * the given loudness and then return a value in the range of [0, MIX_MAX_VOLUME].
 */
int GameSoundAssets::CalculateFinalVolume(Sound::SoundType soundType, GameSoundAssets::SoundVolumeLoudness loudness) {
	float loudnessModifier = 1.0f;
	switch (loudness) {
		case GameSoundAssets::VeryQuietVolume:
			loudnessModifier = 0.5f;
			break;
		case GameSoundAssets::QuietVolume:
			loudnessModifier = 0.65f;
			break;
		case GameSoundAssets::NormalVolume:
			loudnessModifier = 0.75f;
			break;
		case GameSoundAssets::LoudVolume:
			loudnessModifier = 0.85f;
			break;
		case GameSoundAssets::VeryLoudVolume:
			loudnessModifier = 1.0f;
			break;
		default:
			assert(false);
			break;
	}

	switch (soundType) {
		case Sound::EventSound:
		case Sound::MaskSound:
			return static_cast<int>(GameSoundAssets::BASE_EVENT_AND_MASK_SOUND_VOLUME * loudnessModifier);
		case Sound::MusicSound:
			return static_cast<int>(GameSoundAssets::BASE_MUSIC_SOUND_VOLUME * loudnessModifier);
		default:
			assert(false);
			break;
	}

	return 0;
}