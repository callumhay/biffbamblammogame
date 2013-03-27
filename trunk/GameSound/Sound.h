/**
 * Sound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include <irrKlang.h>

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"

#include "SoundCommon.h"

class Sound {
public:
    Sound(irrklang::ISound* sound);
    ~Sound();

    SoundID GetSoundID() const;

    bool IsFinished() const;
    void Stop();
    void SetPosition(const Point3D& pos);

private:
    const SoundID id;
    irrklang::ISound* sound;

    DISALLOW_COPY_AND_ASSIGN(Sound);
};

inline Sound::Sound(irrklang::ISound* sound) : 
id(GenerateSoundID()), sound(sound) {
    assert(sound != NULL);
}

inline Sound::~Sound() {
    this->sound->drop();
}

inline SoundID Sound::GetSoundID() const {
    return this->id;
}

inline bool Sound::IsFinished() const {
    return this->sound->isFinished();
}

inline void Sound::Stop() {
    this->sound->stop();
}

inline void Sound::SetPosition(const Point3D& pos) {
    this->sound->setPosition(irrklang::vec3df(pos[0], pos[1], pos[2]));
}

#endif // __SOUND_H__