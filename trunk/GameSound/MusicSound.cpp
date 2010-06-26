#include "MusicSound.h"

MusicSound::MusicSound(const std::string& name, int msFadein, int msFadeout) :
Sound(name, msFadein, msFadeout), sdlMusic(NULL), isPlaying(false), isPaused(false) {
}

MusicSound::~MusicSound() {
	// Clean up the music pointer in SDL
	Mix_FreeMusic(this->sdlMusic);
	this->sdlMusic = NULL;
}

// Static factory method for building a Music sound.
MusicSound* MusicSound::BuildMusicSound(const std::string& name, const std::string& filepath, int msFadein, int msFadeout) {

	MusicSound* newMusic = new MusicSound(name, msFadein, msFadeout);
	
#ifdef _DEBUG
	newMusic->sdlMusic = Mix_LoadMUS(filepath.c_str());
#else
	bool success = ResourceManager::GetInstance()->GetSoundResourceBuffer(filepath, newMusic->sdlMusic);
#endif

	if (!newMusic->IsValid()) {
		delete newMusic;
		newMusic = NULL;
	}

	return newMusic;
}