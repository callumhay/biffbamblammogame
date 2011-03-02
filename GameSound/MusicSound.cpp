/**
 * MusicSound.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MusicSound.h"
#include "../ResourceManager.h"

MusicSound::MusicSound(const std::string& name, int msFadein, int msFadeout) :
Sound(name, msFadein, msFadeout), sdlMusic(NULL), isPlaying(false), isPaused(false) {
}

MusicSound::~MusicSound() {
	// Clean up the music pointer in SDL
	bool success = ResourceManager::GetInstance()->ReleaseMusicSoundResource(this->sdlMusic);
	if (!success) {
		std::cerr << "Error releasing music " << this->GetSoundName() << std::endl;
	}
}

// Static factory method for building a Music sound.
MusicSound* MusicSound::BuildMusicSound(const std::string& name, const std::string& filepath, int msFadein, int msFadeout) {

	MusicSound* newMusic = new MusicSound(name, msFadein, msFadeout);
	
	newMusic->sdlMusic = ResourceManager::GetInstance()->GetMusicSoundResource(filepath);
	if (newMusic->sdlMusic == NULL) {
		std::cerr << "Error loading music file " << filepath << std::endl;
	}

	if (!newMusic->IsValid()) {
		std::cerr << "Invalid music found: " << newMusic->GetSoundName() << std::endl;
		delete newMusic;
		newMusic = NULL;
	}

	return newMusic;
}