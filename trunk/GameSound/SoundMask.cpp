#include "SoundMask.h"

SoundMask::SoundMask(const std::string& name, const std::string& filepath) : Sound(name, filepath) {

	// The sound mask ALWAYS loops...
	this->isLooping = true;
	alSourcei(this->source, AL_LOOPING, this->isLooping);

}

SoundMask::~SoundMask() {
}

void SoundMask::Tick(double dT) {
}


	