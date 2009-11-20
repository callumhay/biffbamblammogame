#include "Sound.h"
#include "SoundEvent.h"
#include "SoundMask.h"

#include "../ResourceManager.h"

// Default position and velocity of this sound in the game
const ALfloat Sound::DEFAULT_SOURCE_POS[3]		= {0.0, 0.0, 0.0};
const ALfloat Sound::DEFAULT_SOURCE_VEL[3]		= {0.0, 0.0, 0.0};

Sound::Sound(const std::string& name, const std::string& filepath) : 
soundName(name), soundFile(filepath), isLooping(true) {
	debug_openal_state();

	// Try to load the given sound file into an OpenAL buffer - in debug mode we load right off disk,
	// in release we load it from the zip file system
#ifdef _DEBUG
	this->buffer = alutCreateBufferFromFile(filepath.c_str());
#else
	int dataLength = 0;
	char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(filepath, dataLength);
	this->buffer = alutCreateBufferFromFileImage(soundMemData, dataLength);
#endif

	// Continue loading the sound if the buffer was loaded properly
	if (this->buffer != 0) {

		// Load the source, only continue if it loads properly
		alGenSources(1, &this->source);
		if (this->source != 0) {
			// Setup the source with the appropriate default values
			alSourcei(this->source, AL_BUFFER, this->buffer);
			alSourcef(this->source, AL_PITCH, 1.0f);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSourcefv(this->source, AL_POSITION, Sound::DEFAULT_SOURCE_POS);
			alSourcefv(this->source, AL_VELOCITY, Sound::DEFAULT_SOURCE_VEL);
		}
	}

	debug_openal_state();
}

Sound::~Sound() {
	if (this->IsValid()) {
		// Make sure the sound is stopped first
		this->Stop();
	}

	// Clean up the OpenAL sound source and buffer...
	if (this->source != 0) {
		alDeleteSources(1, &this->source);
	}
	if (this->buffer != 0) {
		alDeleteBuffers(1, &this->buffer);
	}

	debug_openal_state();
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool Sound::IsSoundMask(int soundType) {
	return soundType == MainMenuBackgroundMask;
}