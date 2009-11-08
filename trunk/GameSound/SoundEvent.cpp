#include "SoundEvent.h"

SoundEvent::SoundEvent(const std::string& name, const std::string& filepath) :
Sound(name, filepath), startDelay(DEFAULT_DELAY), numLoops(DEFAULT_LOOPS), fadein(DEFAULT_FADEIN),
fadeout(DEFAULT_FADEOUT) {

	// The sound event, by default does not loop...
	this->isLooping = false;
	alSourcei(this->source, AL_LOOPING, this->isLooping);
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::Tick(double dT) {
}

