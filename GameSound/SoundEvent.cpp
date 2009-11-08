#include "SoundEvent.h"

SoundEvent::SoundEvent(const std::string& filepath) :
Sound(filepath), startDelay(DEFAULT_DELAY), numLoops(DEFAULT_LOOPS), fadein(DEFAULT_FADEIN),
fadeout(DEFAULT_FADEOUT) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::Tick(double dT) {
}

