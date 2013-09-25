/**
 * ReverbWaveSoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>
#include "ReverbWaveSoundEffect.h"

ReverbWaveSoundEffect::ReverbWaveSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {
    SoundEffect::ParseSoundParameter<float>(parameterMap, "reverbWaveInputGain", -96.0f, 0.0f, 0.0f, this->inputGain);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "reverbWaveMix", -96.0f, 0.0f, 0.0f, this->reverbMix);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "reverbWaveTime", 0.001f, 3000.0f, 1000.0f, this->reverbTime);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "reverbWaveHFR", 0.001f, 0.999f, 0.001f, this->highFreqRatio);
}

ReverbWaveSoundEffect::~ReverbWaveSoundEffect() {
}

bool ReverbWaveSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableWavesReverbSoundEffect(this->inputGain, this->reverbMix, this->reverbTime, this->highFreqRatio);
}

bool ReverbWaveSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableWavesReverbSoundEffect();
    return true;
}