#include "Sound.h"

#include "../ResourceManager.h"

const int Sound::INVALID_SDL_CHANNEL = INT_MIN;

Sound::Sound(const std::string& name, int msFadein, int msFadeout) : 
soundName(name), msFadein(msFadein), msFadeout(msFadeout) {
}

Sound::~Sound() {
}

