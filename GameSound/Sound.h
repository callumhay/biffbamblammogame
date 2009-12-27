#ifndef _SOUND_H_
#define _SOUND_H_

#include "../BlammoEngine/BasicIncludes.h"

/**
 * Class to represent the abstract highlevel structure of a in-game sound.
 * This class encapsulates much of the functionality for loading sounds.
 */
class Sound {

public:
	Sound(const std::string& name, const std::string& filepath);
	~Sound();
	
	// Whether or not this sound is valid
	// Returns: true if valid, false otherwise.
	bool IsValid() const {
		return (this->buffer != 0) && (this->source != 0);
	}

	// Plays the sound
	void Play()	{
		assert(this->IsValid());
		alSourcePlay(this->source);
	}

	// Stops the sound from playing
	void Stop() {
		assert(this->IsValid());
		alSourceStop(this->source);
	}

	void Tick(double dT) {};

	// Returns: Whether this sound is playing or not
	bool IsPlaying() const {
		ALint sourceState = AL_STOPPED;
		alGetSourcei(this->source, AL_SOURCE_STATE, &sourceState);
		return (sourceState == AL_PLAYING);
	}

	std::string GetSoundName() const { return this->soundName; }
	std::string GetSoundFilename() const { return this->soundFile; }

	// Set whether the sound loops or not
	void SetLooping(bool doesLoop) {
		this->isLooping = doesLoop;
		alSourcei(this->source, AL_LOOPING, this->isLooping);
	}

protected:
	static const ALfloat DEFAULT_SOURCE_POS[3];
	static const ALfloat DEFAULT_SOURCE_VEL[3];

	ALuint buffer;	// The ID for the OpenAL buffer that holds the sound data
	ALuint source;	// The ID for the OpenAL source, representing the point in space that emits the sound

	ALenum format;				// The format of the loaded sound 
	ALsizei size;					// The size in bytes of the loaded sound
	ALsizei frequency;		// The frequency of the loaded sound
	ALboolean isLooping;	// Whether the sound is set to loop indefinitely or not

	std::string soundName;
	std::string soundFile;
};

#endif