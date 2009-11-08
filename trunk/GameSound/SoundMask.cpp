#include "SoundMask.h"

SoundMask::SoundMask(const std::string& filepath) : Sound(filepath) {

	// The sound mask ALWAYS loops...
	this->isLooping = true;
	alSourcei(this->source, AL_LOOPING, this->isLooping);

}

SoundMask::~SoundMask() {
}

void SoundMask::Tick(double dT) {
}


	