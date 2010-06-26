#ifndef _SOUND_H_
#define _SOUND_H_

#include "../BlammoEngine/BasicIncludes.h"

/**
 * Class to represent the abstract highlevel structure of a in-game sound.
 * This class encapsulates much of the functionality for loading sounds using 
 * the SDL Mixer library.
 */
class Sound {

public:
	virtual ~Sound();
	
	enum SoundType { EventSound, MaskSound, MusicSound };
	virtual Sound::SoundType GetType() const = 0;
		
	// Whether or not this sound is valid
	// Returns: true if valid, false otherwise.
	virtual bool IsValid() const;
	// Plays the sound
	virtual void Play(bool doFadeIn) = 0;
	// Pauses/Unpauses the sound
	virtual void Pause() = 0;
	virtual void UnPause() = 0;
	// Stops the sound from playing
	virtual void Stop(bool doFadeOut) = 0;
	// Whether this sound is playing or not
	virtual bool IsPlaying() const = 0;
	// Whether this sound is paused or not
	virtual bool IsPaused() const = 0;
	// Whether this sound loops indefintely
	virtual bool IsLooped() const = 0;

	std::string GetSoundName() const { return this->soundName; }

protected:
	Sound(const std::string& name, int msFadein, int msFadeout);

	static const int INVALID_SDL_CHANNEL;	// Invalid value for a channel

	std::string soundName;
	int msFadein;		// Millisecond fade in when playing this sound
	int msFadeout;	// Millisecond fade out when playing this sound
};

inline bool Sound::IsValid() const {
	if (this->msFadein < 0 || this->msFadeout < 0) {
		return false;
	}
	return true;
}

#endif