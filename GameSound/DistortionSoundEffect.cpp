/**
 * DistortionSoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrklang.h>

#include "DistortionSoundEffect.h"

DistortionSoundEffect::DistortionSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {

    // Parse the relevant parameters for distortion...
    SoundEffect::ParseSoundParameter<float>(parameterMap, "distortionGain", -60, 0, -18, this->gain);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "distortionIntensity", 0.0f, 100.0f, 15.0f, this->intensity);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "distortionPostEQCenterFreq", 100.0f, 8000.0f, 2400.0f, this->postEQCenterFreq);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "distortionPostEQBandwidth", 100.0f, 8000.0f, 2400.0f, this->postEQBandwidth);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "distortionPreLowpassCutoff", 100.0f, 8000.0f, 8000.0f, this->preLowpassCutoff);

}

DistortionSoundEffect::~DistortionSoundEffect() {
}

bool DistortionSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableDistortionSoundEffect(this->gain, this->intensity, this->postEQCenterFreq,
        this->postEQBandwidth, this->preLowpassCutoff);
}

bool DistortionSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableDistortionSoundEffect();
    return true;
}