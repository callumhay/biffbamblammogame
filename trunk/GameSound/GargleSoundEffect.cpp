/**
 * GargleSoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrklang.h>
#include "GargleSoundEffect.h"

GargleSoundEffect::GargleSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {

    // Parse parameters specific to the gargle sound effect
    SoundEffect::ParseSoundParameter<int>(parameterMap, "gargleRate", 1, 1000, 20, this->rate);
    SoundEffect::ParseSoundParameter<bool>(parameterMap, "gargleIsSinWave", false, true, true, this->isSinWave);
}

GargleSoundEffect::~GargleSoundEffect() {
}

bool GargleSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableGargleSoundEffect(this->rate, this->isSinWave);
}

bool GargleSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableGargleSoundEffect();
    return true;
}

