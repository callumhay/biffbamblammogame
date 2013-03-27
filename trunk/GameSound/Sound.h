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
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Point.h"

#include "SoundCommon.h"

class Sound {
public:
    Sound(irrklang::ISound* sound);
    ~Sound();

    SoundID GetSoundID() const;

    bool IsFinished() const;
    
    void Tick(double dT);

    void Stop();
    void SetPosition(const Point3D& pos);

    void SetFadeout(double timeInSecs);

private:
    const SoundID id;
    irrklang::ISound* sound;

    double fadeOutTimeCountdown;
    double totalFadeOutTime;

    DISALLOW_COPY_AND_ASSIGN(Sound);
};

inline Sound::Sound(irrklang::ISound* sound) : 
id(GenerateSoundID()), sound(sound), fadeOutTimeCountdown(-1), totalFadeOutTime(-1) {
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

inline void Sound::Tick(double dT) {

    // Perform any fade-out on the sound
    if (this->totalFadeOutTime > 0) {
        this->fadeOutTimeCountdown = std::max<double>(0.0, this->fadeOutTimeCountdown - dT);
        if (this->fadeOutTimeCountdown == 0.0) {
            this->sound->stop();
        }
        else {
            float currVol = NumberFuncs::LerpOverTime<float>(this->totalFadeOutTime, 0.0, 1.0f, 0.0f, this->fadeOutTimeCountdown);
            this->sound->setVolume(currVol);
        }
    }

}

inline void Sound::Stop() {
    this->sound->stop();
}

inline void Sound::SetPosition(const Point3D& pos) {
    this->sound->setPosition(irrklang::vec3df(pos[0], pos[1], pos[2]));
}

inline void Sound::SetFadeout(double timeInSecs) {
    assert(timeInSecs >= 0);
    if (this->totalFadeOutTime > 0) {
        return;
    }
    this->totalFadeOutTime = this->fadeOutTimeCountdown = timeInSecs;
}

#endif // __SOUND_H__