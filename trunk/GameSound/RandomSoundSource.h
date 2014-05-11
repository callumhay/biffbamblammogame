/**
 * RandomSoundSource.h
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

#ifndef __RANDOMSOUNDSOURCE_H__
#define __RANDOMSOUNDSOURCE_H__

#include "AbstractSoundSource.h"

class RandomSoundSource : public AbstractSoundSource {
    friend class GameSound;
public:
    ~RandomSoundSource();

    bool IsLoaded() const;

    bool Load();
    void Unload();

private:
    RandomSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName, const std::vector<std::string>& filePaths);

    bool isInit;
    std::vector<irrklang::ISoundSource*> sources;
    std::vector<std::string> soundFilePaths;

    Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped, bool startPaused);
    Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos, bool startPaused);

    DISALLOW_COPY_AND_ASSIGN(RandomSoundSource);
};

inline bool RandomSoundSource::IsLoaded() const {
    return this->isInit;
}

#endif // __RANDOMSOUNDSOURCE_H__