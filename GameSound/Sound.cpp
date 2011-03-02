/**
 * Sound.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Sound.h"

#include "../ResourceManager.h"

const int Sound::INVALID_SDL_CHANNEL = INT_MIN;

Sound::Sound(const std::string& name, int msFadein, int msFadeout) : 
soundName(name), msFadein(msFadein), msFadeout(msFadeout) {
}

Sound::~Sound() {
}

