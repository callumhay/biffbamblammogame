/**
 * Sound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include <irrKlang.h>

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Point.h"

#include "GameSound.h"
#include "SoundEffect.h"

class Sound {
public:
    Sound(const SoundID& id, const GameSound::SoundType& soundType, irrklang::ISound* sound, bool isMusic);
    ~Sound();

    SoundID GetSoundID() const;
    GameSound::SoundType GetSoundType() const;

    bool IsMusic() const;
    bool IsLooped() const;
    bool IsFinished() const;
    bool IsFadingOut() const;
    
    void Tick(double dT, GameSound& gameSound);

    void SetPause(bool isPaused);
    void Stop();
    void SetPosition(const Point3D& pos);
    void SetMinimumDistance(float minDist);

    void SetMasterVolume(float masterVolume);
    void SetVolume(float masterVolume, float volume);
    void SetFadeout(double timeInSecs);
    
    void Visit(SoundEffect& soundEffect, bool effectOn);
    void StopAllEffects();

private:
    const SoundID id;
    const GameSound::SoundType soundType;

    bool isMusic; // Whether this sound is music (true) or SFX (false)
    float volume; // The volume of this sound, independent of SFX/Music master volumes

    irrklang::ISound* sound;

    float volumeAtStartOfFadeout;
    double fadeOutTimeCountdown;
    double totalFadeOutTime;

    DISALLOW_COPY_AND_ASSIGN(Sound);
};

inline Sound::Sound(const SoundID& id, const GameSound::SoundType& soundType, irrklang::ISound* sound, bool isMusic)  : 
id(id), soundType(soundType), sound(sound), fadeOutTimeCountdown(-1), totalFadeOutTime(-1), isMusic(isMusic), volume(1.0f) {
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

inline bool Sound::IsMusic() const {
    return this->isMusic;
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

inline void Sound::Tick(double dT, GameSound& gameSound) {

    // Perform any fade-out on the sound
    if (this->IsFadingOut()) {
        this->fadeOutTimeCountdown = std::max<double>(0.0, this->fadeOutTimeCountdown - dT);
        if (this->fadeOutTimeCountdown == 0.0 || this->sound->getVolume() == 0.0f) {
            this->sound->stop();
            this->totalFadeOutTime = 0;
            this->volume = 0.0f;
        }
        else {
            // Interpolate the fadeout
            // NOTE: The interpolation applies to the volume, independent of the master (music/SFX) volume
            float currVol = NumberFuncs::LerpOverTime<float>(this->totalFadeOutTime, 0.0, 
                this->volumeAtStartOfFadeout, 0.0f, this->fadeOutTimeCountdown);
            this->SetVolume(this->isMusic ? gameSound.GetMusicVolume() : gameSound.GetSFXVolume(), currVol);
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

inline void Sound::SetMinimumDistance(float minDist) {
    this->sound->setMinDistance(minDist);
}

inline void Sound::SetMasterVolume(float masterVolume) {
    this->sound->setVolume(masterVolume * this->volume);
}

inline void Sound::SetVolume(float masterVolume, float volume) {
    assert(volume >= 0.0f && volume <= 1.0f);
    assert(masterVolume >= 0.0f && masterVolume <= 1.0f);
    this->volume = volume;
    this->SetMasterVolume(masterVolume);
}

inline void Sound::SetFadeout(double timeInSecs) {
    assert(timeInSecs >= 0);
    if (this->totalFadeOutTime > 0) {
        return;
    }
    this->totalFadeOutTime = this->fadeOutTimeCountdown = timeInSecs;
    this->volumeAtStartOfFadeout = this->volume;
}

inline void Sound::Visit(SoundEffect& soundEffect, bool effectOn) {
    soundEffect.ToggleEffect(this->sound, effectOn);
}

inline void Sound::StopAllEffects() {
    irrklang::ISoundEffectControl* effectCtrl = this->sound->getSoundEffectControl();
    if (effectCtrl != NULL) {
        effectCtrl->disableAllEffects();
    }
}

#endif // __SOUND_H__