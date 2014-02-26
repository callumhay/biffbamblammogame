/**
 * SingleSoundSource.cpp
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

Sound* SingleSoundSource::Spawn2DSoundWithID(const SoundID& id, bool isLooped, bool startPaused) {
    if (this->source == NULL) {
        assert(false);
        return NULL;
    }

    return this->Spawn2DSoundWithIDAndSource(id, this->source, isLooped, startPaused);
}

Sound* SingleSoundSource::Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos, bool startPaused) {
    if (this->source == NULL) {
        assert(false);
        return NULL;
    }

    return this->Spawn2DSoundWithIDAndSource(id, this->source, isLooped, pos, startPaused);
}