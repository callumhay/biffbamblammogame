/**
 * SoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
    Sound* Spawn3DSound(bool isLooped, const Point3D& position);

private:
    SoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName, const std::string& filePath);

    const GameSound::SoundType soundType;
    const std::string soundName;
    const std::string soundFilePath;

    irrklang::ISoundEngine* soundEngine;
    irrklang::ISoundSource* source;

    Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped);
    Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos);

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

inline Sound* SoundSource::Spawn3DSound(bool isLooped, const Point3D& position) {
    return this->Spawn3DSoundWithID(GenerateSoundID(), isLooped, position);
}

#endif // __SOUNDSOURCE_H__