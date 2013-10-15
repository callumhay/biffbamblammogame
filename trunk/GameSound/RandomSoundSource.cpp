/**
 * RandomSoundSource.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>
#include "RandomSoundSource.h"

RandomSoundSource::RandomSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
                                     const std::string& soundName, const std::vector<std::string>& filePaths) :
AbstractSoundSource(soundEngine, soundType, soundName), soundFilePaths(filePaths), isInit(false) {
    this->sources.resize(soundFilePaths.size(), NULL);
}

RandomSoundSource::~RandomSoundSource() {
    this->Unload();
}

bool RandomSoundSource::Load() {
    assert(this->sources.size() == this->soundFilePaths.size());

    if (this->isInit) {
        return true;
    }

    bool allSourcesLoadedProperly = true;
    for (int i = 0; i < static_cast<int>(this->soundFilePaths.size()); i++) {
        
        const std::string& currFilePath = this->soundFilePaths[i];
        if (this->sources[i] == NULL) {
            this->sources[i] = this->LoadSoundSource(currFilePath);
            if (this->sources[i] == NULL) {
                allSourcesLoadedProperly = false; 
            }
        }
    }
    
    if (!allSourcesLoadedProperly) {
        this->Unload();
    }

    this->isInit = allSourcesLoadedProperly;
    return allSourcesLoadedProperly;
}

void RandomSoundSource::Unload() {
    for (int i = 0; i < static_cast<int>(this->sources.size()); i++) {
        if (this->sources[i] != NULL) {
            this->UnloadSoundSource(this->sources[i]);
            this->sources[i] = NULL;
        }
    }
    this->isInit = false;
}

Sound* RandomSoundSource::Spawn2DSoundWithID(const SoundID& id, bool isLooped, bool startPaused) {
    if (!this->IsLoaded()) {
        assert(false);
        return NULL;
    }
    
    // Choose a random source to spawn...
    irrklang::ISoundSource* randomSource = this->sources[Randomizer::GetInstance()->RandomUnsignedInt() % this->sources.size()];
    assert(randomSource != NULL);
    return this->Spawn2DSoundWithIDAndSource(id, randomSource, isLooped, startPaused);
}

Sound* RandomSoundSource::Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos, bool startPaused) {
    if (!this->IsLoaded()) {
        assert(false);
        return NULL;
    }

    // Choose a random source to spawn...
    irrklang::ISoundSource* randomSource = this->sources[Randomizer::GetInstance()->RandomUnsignedInt() % this->sources.size()];
    assert(randomSource != NULL);
    return this->Spawn2DSoundWithIDAndSource(id, randomSource, isLooped, pos, startPaused);
}