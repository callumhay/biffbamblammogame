/**
 * AbstractSoundSource.cpp
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
#include "AbstractSoundSource.h"
#include "Sound.h"

#include "../ResourceManager.h"

AbstractSoundSource::AbstractSoundSource(irrklang::ISoundEngine* soundEngine, 
                                         const GameSound::SoundType& soundType,
                                         const std::string& soundName) :
soundEngine(soundEngine), soundType(soundType), soundName(soundName), isMusic(GameSound::IsMusic(soundType)) {
    assert(soundEngine != NULL);
}

AbstractSoundSource::~AbstractSoundSource() {
}

irrklang::ISoundSource* AbstractSoundSource::LoadSoundSource(const std::string& filepath) {
    irrklang::ISoundSource* source = NULL;

    // Attempt to load the source directly from the engine (in cases where the source already has been loaded)
    source = this->soundEngine->getSoundSource(filepath.c_str(), false);
    if (source == NULL) {
        
        // Looks like the sound hasn't been loaded yet, load it from memory using the file path
        long dataLength = 0;
        char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(filepath, dataLength);
        if (soundMemData == NULL) {
            std::cerr << "[Sound: " << this->soundName << "] - Could not find file: " << filepath << std::endl;
            return NULL;
        }

        source = this->soundEngine->addSoundSourceFromMemory(soundMemData, dataLength, filepath.c_str(), true);
        delete[] soundMemData;
        soundMemData = NULL;
    }

    return source;
}

void AbstractSoundSource::UnloadSoundSource(irrklang::ISoundSource* source) {
    assert(source != NULL);
    if (this->soundEngine == NULL) {
        assert(false);
        return;
    }

    this->soundEngine->removeSoundSource(source);
}

Sound* AbstractSoundSource::Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, 
                                                        bool isLooped, bool startPaused) {
    assert(source != NULL);

    irrklang::ISound* newSound = this->soundEngine->play2D(source, isLooped, startPaused, true, true);
    if (newSound == NULL) {
        assert(false);
        return NULL;
    }

    return new Sound(id, this->soundType, newSound, this->isMusic);
}

Sound* AbstractSoundSource::Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, 
                                                        bool isLooped, const Point3D& pos, bool startPaused) {
    assert(source != NULL);
    
    irrklang::ISound* newSound = this->soundEngine->play3D(source, irrklang::vec3df(pos[0], pos[1], pos[2]), isLooped, startPaused, true, true);
    newSound->setMinDistance(GameSound::DEFAULT_MIN_3D_SOUND_DIST); // Make sure we're playing at max volume everywhere in the game
    if (newSound == NULL) {
        assert(false);
        return NULL;
    }

    return new Sound(id, this->soundType, newSound, this->isMusic);
}