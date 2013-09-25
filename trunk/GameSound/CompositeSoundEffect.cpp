/**
 * CompositeSoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CompositeSoundEffect.h"

CompositeSoundEffect::CompositeSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) {
}

CompositeSoundEffect::~CompositeSoundEffect() {
    // Clean up all contained sound effects
    for (std::vector<SoundEffect*>::iterator iter = this->soundEffects.begin(); iter != this->soundEffects.end(); ++iter) {
        SoundEffect* effect = *iter;
        delete effect;
        effect = NULL;
    }
    this->soundEffects.clear();
}

CompositeSoundEffect* CompositeSoundEffect::Build(const EffectParameterMap& parameterMap, std::vector<SoundEffect*> soundEffects) {
    if (soundEffects.size() <= 1) {
        assert(false);
        return NULL;
    }

    CompositeSoundEffect* result = new CompositeSoundEffect(parameterMap);
    result->soundEffects = soundEffects;
    return result;
}

bool CompositeSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    bool success = true;
    for (std::vector<SoundEffect*>::const_iterator iter = this->soundEffects.begin(); iter != this->soundEffects.end(); ++iter) {
        SoundEffect* currEffect = *iter;
        success &= currEffect->EnableEffect(soundEffectCtrl);
    }
    return success;
}

bool CompositeSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    bool success = true;
    for (std::vector<SoundEffect*>::const_iterator iter = this->soundEffects.begin(); iter != this->soundEffects.end(); ++iter) {
        SoundEffect* currEffect = *iter;
        success &= currEffect->DisableEffect(soundEffectCtrl);
    }
    return success;
}
