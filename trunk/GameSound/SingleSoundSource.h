/**
 * SingleSoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    Sound* Spawn2DSoundWithID(const SoundID& id, bool isLooped);
    Sound* Spawn3DSoundWithID(const SoundID& id, bool isLooped, const Point3D& pos);

    DISALLOW_COPY_AND_ASSIGN(SingleSoundSource);
};

inline bool SingleSoundSource::IsLoaded() const {
    return (this->source != NULL);
}

#endif // __SINGLESOUNDSOURCE_H__