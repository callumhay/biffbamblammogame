#ifndef __MUSICSOUND_H__
#define __MUSICSOUND_H__

#include "Sound.h"

class MusicSound : public Sound {
public:
	~MusicSound();

	static MusicSound* BuildMusicSound(const std::string& name, const std::string& filepath, int msFadein, int msFadeout);

	Sound::SoundType GetType() const;
	bool IsValid() const;
	void Play(bool doFadeIn);
	void Pause();
	void UnPause();
	void Stop(bool doFadeOut);
	bool IsPlaying() const;
	bool IsPaused() const;
	bool IsLooped() const;

protected:
	MusicSound(const std::string& name, int msFadein, int msFadeout);
	

	bool isPlaying;
	bool isPaused;
	Mix_Music* sdlMusic;		// SDL Music object pointer

};

inline Sound::SoundType MusicSound::GetType() const {
	return Sound::MusicSound;
}

inline bool MusicSound::IsValid() const {
	if (!Sound::IsValid()) {
		return false;
	}
	if (this->sdlMusic == NULL) {
		return false;
	}

	return true;
}

inline void MusicSound::Play(bool doFadeIn) {
	assert(this->IsValid());
	
	// Shouldn't be the case that more than one piece of music is attempted to be played at once..
	if (this->IsPlaying() || (!this->isPlaying && Mix_PlayingMusic() == 1)) {
		this->Stop(false);
	}
	
	int fadeInAmt = doFadeIn ? this->msFadein : 0;
	Mix_FadeInMusic(this->sdlMusic, -1, fadeInAmt);
	this->isPlaying = true;
}

inline void MusicSound::Pause() {
	if (this->isPlaying) {
		Mix_PauseMusic();
		this->isPaused = true;
	}
	this->isPlaying = false;
}

inline void MusicSound::UnPause() {
	if (this->IsPaused()) {
		Mix_ResumeMusic();
		this->isPlaying = true;
		this->isPaused = false;
	}
}

inline void MusicSound::Stop(bool doFadeOut) {
	assert(this->IsValid());

	if (this->IsPlaying()) {
		int fadeoutAmt = doFadeOut ? this->msFadeout : 0;
		Mix_FadeOutMusic(fadeoutAmt);
	}

	this->isPlaying = false;
}

inline bool MusicSound::IsPlaying() const {
	return (Mix_PlayingMusic() == 1 && this->isPlaying);
}

inline bool MusicSound::IsPaused() const {
	return (Mix_PausedMusic() == 1 && this->isPaused);
}

inline bool MusicSound::IsLooped() const {
	return true;
}

#endif // __MUSICSOUND_H__