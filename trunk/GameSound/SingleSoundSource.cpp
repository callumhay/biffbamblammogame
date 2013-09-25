/**
 * SingleSoundSource.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>
#include "SingleSoundSource.h"

SingleSoundSource::SingleSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
                                     const std::string& soundName, const std::string& filePath) : 
AbstractSoundSource(soundEngine, soundType, soundName), soundFilePath(filePath), source(NULL) {
}

SingleSoundSource::~SingleSoundSource() {
    this->Unload();
}

bool SingleSoundSource::Load() {

    if (this->source == NULL) {
        this->source = this->LoadSoundSource(this->soundFilePath);

        if (this->source == NULL) {
            return false;
        }
    }

    return true;
}

void SingleSoundSource::Unload() {
    if (this->source != NULL) {
        this->UnloadSoundSource(this->source);
        this->source = NULL;
    }
}

Sound* SingleSoundSource::Spawn2DSoundWithID(const SoundID& id, bool isLooped) {
    if (this->source == NULL) {
        assert(false);
        return NULL;
    }

    return this->Spawn2DSoundWithIDAndSource(id, this->source, isLooped);
}

Sound* SingleSoundSource::Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos) {
    if (this->source == NULL) {
        assert(false);
        return NULL;
    }

    return this->Spawn2DSoundWithIDAndSource(id, this->source, isLooped, pos);
}