/**
 * ChorusSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>
#include "ChorusSoundEffect.h"

ChorusSoundEffect::ChorusSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {
    SoundEffect::ParseSoundParameter<float>(parameterMap, "chorusWetDryMix", 0.0f, 100.0f, 50.0f, this->wetDryMix);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "chorusDepth", 0.0f, 100.0f, 10.0f, this->depth);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "chorusFeedback", -99.0f, 99.0f, 25.0f, this->feedback);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "chorusFrequency", 0.0f, 10.0f, 1.1f, this->frequency);
    SoundEffect::ParseSoundParameter<bool>(parameterMap,  "chorusIsSinWave", false, true, true, this->isSinWave);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "chorusDelay", 0.0f, 20.0f, 16.0f, this->delay);
}

ChorusSoundEffect::~ChorusSoundEffect() {
}

bool ChorusSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableChorusSoundEffect(this->wetDryMix, this->depth, this->feedback, 
        this->frequency, this->isSinWave, this->delay, 90);
}

bool ChorusSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableChorusSoundEffect();
    return true;
}