#include "Sound.h"
#include "SoundEvent.h"
#include "SoundMask.h"


Sound::Sound(const std::string& filepath) {
	// TODO: load the sound...
}

Sound::~Sound() {
	// TODO: destroy sound...
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool Sound::IsSoundMask(int soundType) {
	return soundType == MainMenuBackgroundMask;
}