/**
 * RandomSoundSource.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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