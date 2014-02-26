/**
 * SingleSoundSource.h
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

#ifndef __SINGLESOUNDSOURCE_H__
#define __SINGLESOUNDSOURCE_H__

#include "AbstractSoundSource.h"

// IrrKlang Forward Declarations
namespace irrklang {
class ISoundEngine;
class ISoundSource;
};

class Sound;

class SingleSoundSource : public AbstractSoundSource {
    friend class GameSound;
public:
    ~SingleSoundSource();

    bool IsLoaded() const;

    // Sound sources start off as proxies and must be loaded into memory to spawn playable sounds
    bool Load();
    void Unload();

private:
    SingleSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName, const std::string& filePath);

    const std::string soundName;
    const std::string soundFilePath;

    irrklang::ISoundSource* source;

    Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped, bool startPaused);
    Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos, bool startPaused);

    DISALLOW_COPY_AND_ASSIGN(SingleSoundSource);
};

inline bool SingleSoundSource::IsLoaded() const {
    return (this->source != NULL);
}

#endif // __SINGLESOUNDSOURCE_H__