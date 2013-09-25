/**
 * AbstractSoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    Sound* Spawn2DSound(bool isLooped);
    Sound* Spawn3DSound(bool isLooped, const Point3D& position);

protected:
    AbstractSoundSource(irrklang::ISoundEngine* soundEngine, const GameSound::SoundType& soundType,
        const std::string& soundName);

    irrklang::ISoundEngine* soundEngine;

    const GameSound::SoundType soundType;
    const std::string soundName;

    irrklang::ISoundSource* LoadSoundSource(const std::string& filepath);
    void UnloadSoundSource(irrklang::ISoundSource* source);

    virtual Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped) = 0;
    virtual Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos) = 0;

    Sound* Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, bool isLooped);
    Sound* Spawn2DSoundWithIDAndSource(const SoundID& id, irrklang::ISoundSource* source, bool isLooped, const Point3D& pos);

private:
    DISALLOW_COPY_AND_ASSIGN(AbstractSoundSource);
};

inline GameSound::SoundType AbstractSoundSource::GetSoundType() const {
    return this->soundType;
}

inline Sound* AbstractSoundSource::Spawn2DSound(bool isLooped) {
    return this->Spawn2DSoundWithID(GenerateSoundID(), isLooped);
}

inline Sound* AbstractSoundSource::Spawn3DSound(bool isLooped, const Point3D& position) {
    return this->Spawn3DSoundWithID(GenerateSoundID(), isLooped, position);
}

#endif // __ABSTRACTSOUNDSOURCE_H__