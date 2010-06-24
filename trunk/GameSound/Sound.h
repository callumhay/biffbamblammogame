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
	virtual bool IsValid() const = 0;
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

	//void Tick(double dT) {};

	std::string GetSoundName() const { return this->soundName; }

protected:
	Sound(const std::string& name);

	static const int INVALID_SDL_CHANNEL;	// Invalid value for a channel

	std::string soundName;
};

#endif