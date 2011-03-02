/**
 * Sound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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

	virtual void SetVolume(int volume);

	std::string GetSoundName() const { return this->soundName; }


protected:
	Sound(const std::string& name, int msFadein, int msFadeout);

	static const int INVALID_SDL_CHANNEL;	// Invalid value for a channel

	std::string soundName;
	int volume;			// The volume specified in the interval [0, MIX_MAX_VOLUME]
	int msFadein;		// Millisecond fade in when playing this sound
	int msFadeout;	// Millisecond fade out when playing this sound
};

inline bool Sound::IsValid() const {
	if (this->msFadein < 0 || this->msFadeout < 0) {
		return false;
	}
	return true;
}

inline void Sound::SetVolume(int volume) {
	assert(volume >= 0 && volume <= MIX_MAX_VOLUME);
	this->volume = volume;
}

#endif