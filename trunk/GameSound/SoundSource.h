/**
 * SoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SOUNDSOURCE_H__
#define __SOUNDSOURCE_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameSound.h"

// IrrKlang Forward Declarations
namespace irrklang {
class ISoundEngine;
class ISoundSource;
};

class Sound;

class SoundSource {
    friend class GameSound;
public:
    ~SoundSource();

    GameSound::SoundType GetSoundType() const;
    bool IsLoaded() const;

    // Sound sources start off as proxies and must be loaded into memory to spawn playable sounds
    bool Load();
    void Unload();

    Sound* Spawn2DSound(bool isLooped);


private:
    SoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName, const std::string& filePath);

    const GameSound::SoundType soundType;
    const std::string soundName;
    const std::string soundFilePath;

    irrklang::ISoundEngine* soundEngine;
    irrklang::ISoundSource* source;

    Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped);

    DISALLOW_COPY_AND_ASSIGN(SoundSource);
};

inline GameSound::SoundType SoundSource::GetSoundType() const {
    return this->soundType;
}

inline bool SoundSource::IsLoaded() const {
    return (this->source != NULL);
}

inline Sound* SoundSource::Spawn2DSound(bool isLooped) {
    return this->Spawn2DSoundWithID(GenerateSoundID(), isLooped);
}

#endif // __SOUNDSOURCE_H__