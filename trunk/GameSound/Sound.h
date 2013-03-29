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

#include "GameSound.h"

class Sound {
public:
    Sound(const SoundID& id, const GameSound::SoundType& soundType, irrklang::ISound* sound);
    ~Sound();

    SoundID GetSoundID() const;
    GameSound::SoundType GetSoundType() const;

    bool IsLooped() const;
    bool IsFinished() const;
    bool IsFadingOut() const;
    
    void Tick(double dT);

    void SetPause(bool isPaused);
    void Stop();
    void SetPosition(const Point3D& pos);

    void SetFadeout(double timeInSecs);

private:
    const SoundID id;
    const GameSound::SoundType soundType;
    irrklang::ISound* sound;

    double fadeOutTimeCountdown;
    double totalFadeOutTime;

    DISALLOW_COPY_AND_ASSIGN(Sound);
};

inline Sound::Sound(const SoundID& id, const GameSound::SoundType& soundType, irrklang::ISound* sound)  : 
id(id), soundType(soundType), sound(sound), fadeOutTimeCountdown(-1), totalFadeOutTime(-1) {
    assert(sound != NULL);
    assert(id != INVALID_SOUND_ID);
}

inline Sound::~Sound() {
    this->Stop();
    this->sound->drop();
}

inline SoundID Sound::GetSoundID() const {
    return this->id;
}

inline GameSound::SoundType Sound::GetSoundType() const {
    return this->soundType;
}

inline bool Sound::IsLooped() const {
    return this->sound->isLooped();
}

inline bool Sound::IsFinished() const {
    return this->sound->isFinished();
}

inline bool Sound::IsFadingOut() const {
    return this->totalFadeOutTime > 0;
}

inline void Sound::Tick(double dT) {

    // Perform any fade-out on the sound
    if (this->IsFadingOut()) {
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

inline void Sound::SetPause(bool isPaused) {
    this->sound->setIsPaused(isPaused);
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