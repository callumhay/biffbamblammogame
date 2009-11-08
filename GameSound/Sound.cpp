#include "Sound.h"
#include "SoundEvent.h"
#include "SoundMask.h"

// Default position and velocity of this sound in the game
const ALfloat Sound::DEFAULT_SOURCE_POS[3]		= {0.0, 0.0, 0.0};
const ALfloat Sound::DEFAULT_SOURCE_VEL[3]		= {0.0, 0.0, 0.0};

Sound::Sound(const std::string& filepath) : isLooping(true) {
	// Try to load the given sound file into an OpenAL buffer
	this->buffer = alutCreateBufferFromFile(filepath.c_str());

	// Continue loading the sound if the buffer was loaded properly
	if (this->buffer != AL_NONE) {

		// Load the source, only continue if it loads properly
		alGenSources(1, &this->source);
		if (this->source != AL_NONE) {
			// Setup the source with the appropriate default values
			alSourcei(this->source, AL_BUFFER, this->buffer);
			alSourcef(this->source, AL_PITCH, 1.0f);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSourcefv(this->source, AL_POSITION, Sound::DEFAULT_SOURCE_POS);
			alSourcefv(this->source, AL_VELOCITY, Sound::DEFAULT_SOURCE_VEL);
			alSourcei(this->source, AL_LOOPING, this->isLooping);

		}
	}

	debug_openal_state();
}

Sound::~Sound() {
	// Make sure the sound is stopped first
	this->Stop();

	// Clean up the OpenAL sound source and buffer...
	alDeleteSources(1, &this->source);
	alDeleteBuffers(1, &this->buffer);
	debug_openal_state();
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool Sound::IsSoundMask(int soundType) {
	return soundType == MainMenuBackgroundMask;
}