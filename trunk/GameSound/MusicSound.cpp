#include "MusicSound.h"
#include "../ResourceManager.h"

MusicSound::MusicSound(const std::string& name, int msFadein, int msFadeout) :
Sound(name, msFadein, msFadeout), sdlMusic(NULL), isPlaying(false), isPaused(false) {
}

MusicSound::~MusicSound() {
	// Clean up the music pointer in SDL
#ifdef _DEBUG
	Mix_FreeMusic(this->sdlMusic);
	this->sdlMusic = NULL;
#else
	bool success = ResourceManager::GetInstance()->ReleaseMusicSoundResource(this->sdlMusic);
	if (!success) {
		std::cerr << "Error releasing music " << this->GetSoundName() << std::endl;
	}
#endif
}

// Static factory method for building a Music sound.
MusicSound* MusicSound::BuildMusicSound(const std::string& name, const std::string& filepath, int msFadein, int msFadeout) {

	MusicSound* newMusic = new MusicSound(name, msFadein, msFadeout);
	
#ifdef _DEBUG
	newMusic->sdlMusic = Mix_LoadMUS(filepath.c_str());
#else
	newMusic->sdlMusic = ResourceManager::GetInstance()->GetMusicSoundResource(filepath);
	if (newMusic->sdlMusic == NULL) {
		std::cerr << "Error loading music file " << filepath << std::endl;
	}
#endif

	if (!newMusic->IsValid()) {
		std::cerr << "Invalid music found: " << newMusic->GetSoundName() << std::endl;
		delete newMusic;
		newMusic = NULL;
	}

	return newMusic;
}