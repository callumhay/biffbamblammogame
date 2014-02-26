/**
 * RandomSoundSource.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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