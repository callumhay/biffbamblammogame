/**
 * FlangerSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrklang.h>
#include "FlangerSoundEffect.h"

FlangerSoundEffect::FlangerSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {
    // Parse parameters specific to the flanger sound effect
    SoundEffect::ParseSoundParameter<float>(parameterMap, "flangerWetDryMix", 0.0f, 100.0f, 50.0f, this->wetDryMix);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "flangerDepth", 0.0f, 100.0f, 100.0f, this->depth);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "flangerFeedback", -99.0f, 99.0f, -50.0f, this->feedback);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "flangerFrequency", 0.0f, 10.0f, 0.25f, this->frequency);
    SoundEffect::ParseSoundParameter<bool>(parameterMap,  "flangerIsTriangleWave", false, true, true, this->isTriangleWave);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "flangerDelay", 0.0f, 20.0f, 2.0f, this->delay);
}

FlangerSoundEffect::~FlangerSoundEffect() {
}

bool FlangerSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableFlangerSoundEffect(this->wetDryMix, this->depth, this->feedback, 
        this->frequency, this->isTriangleWave, this->delay, 0);
}

bool FlangerSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableFlangerSoundEffect();
    return true;
}