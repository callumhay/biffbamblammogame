#include "SoundEvent.h"

SoundEvent::SoundEvent(const std::string& filepath, int delay, int loops, int fadein, int fadeout) :
Sound(filepath) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::Play() {
}

void SoundEvent::Tick(double dT) {
}

