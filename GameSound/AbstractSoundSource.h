/**
 * AbstractSoundSource.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __ABSTRACTSOUNDSOURCE_H__
#define __ABSTRACTSOUNDSOURCE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "GameSound.h"

// IrrKlang Forward Declarations
namespace irrklang {
class ISoundEngine;
class ISoundSource;
};

class Sound;

class AbstractSoundSource {
    friend class GameSound;
public:
    virtual ~AbstractSoundSource();

    GameSound::SoundType GetSoundType() const;
    virtual bool IsLoaded() const = 0;

    // Sound sources start off as proxies and must be loaded into memory to spawn playable sounds
    virtual bool Load()   = 0;
    virtual void Unload() = 0;

    Sound* Spawn2DSound(bool isLooped, bool startPaused = false);
    Sound* Spawn3DSound(bool isLooped, const Point3D& position, bool startPaused = false);

protected:
    AbstractSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName);

    irrklang::ISoundEngine* soundEngine;

    const GameSound::SoundType soundType;
    const std::string soundName;
    bool isMusic;

    irrklang::ISoundSource* LoadSoundSource(const std::string& filepath);
    void UnloadSoundSource(irrklang::ISoundSource* source);

    virtual Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped, bool startPaused) = 0;
    virtual Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos, bool startPaused) = 0;

    Sound* Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, bool isLooped, bool startPaused);
    Sound* Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, bool isLooped, const Point3D& pos, bool startPaused);

private:
    DISALLOW_COPY_AND_ASSIGN(AbstractSoundSource);
};

inline GameSound::SoundType AbstractSoundSource::GetSoundType() const {
    return this->soundType;
}

inline Sound* AbstractSoundSource::Spawn2DSound(bool isLooped, bool startPaused) {
    return this->Spawn2DSoundWithID(GenerateSoundID(), isLooped, startPaused);
}

inline Sound* AbstractSoundSource::Spawn3DSound(bool isLooped, const Point3D& position, bool startPaused) {
    return this->Spawn3DSoundWithID(GenerateSoundID(), isLooped, position, startPaused);
}

#endif // __ABSTRACTSOUNDSOURCE_H__