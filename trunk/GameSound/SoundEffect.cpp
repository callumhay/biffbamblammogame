/**
 * SoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include <irrKlang.h>

#include "SoundEffect.h"
#include "Sound.h"
#include "CompositeSoundEffect.h"
#include "Reverb3DSoundEffect.h"
#include "DistortionSoundEffect.h"
#include "GargleSoundEffect.h"
//#include "CompressorSoundEffect.h"

const char* SoundEffect::REVERB3D_STR    = "reverb3D";
const char* SoundEffect::DISTORTION_STR  = "distortion";
const char* SoundEffect::GARGLE_STR      = "gargle";
//const char* SoundEffect::COMPRESSOR_STR  = "compressor";


const char* SoundEffect::VOLUME_PARAM_NAME = "volume";
const float SoundEffect::DEFAULT_VOLUME = 1.0f;

SoundEffect::SoundEffect(const EffectParameterMap& parameterMap) {

    // Parse volume from the parameter map, if available
    SoundEffect::ParseSoundParameter<float>(parameterMap, SoundEffect::VOLUME_PARAM_NAME, 0.0f, 1.0f, DEFAULT_VOLUME, this->volume);
}

SoundEffect::~SoundEffect() {
}

SoundEffect* SoundEffect::Build(size_t typeFlags, const EffectParameterMap& parameterMap) {

    SoundEffect* fullResult = NULL;
    std::vector<SoundEffect*> compositeEffects;

    if ((typeFlags & SoundEffect::Reverb3D) == SoundEffect::Reverb3D) {
        compositeEffects.push_back(new Reverb3DSoundEffect(parameterMap));
    }
    if ((typeFlags & SoundEffect::Distortion) == SoundEffect::Distortion) {
        compositeEffects.push_back(new DistortionSoundEffect(parameterMap));
    }
    if ((typeFlags & SoundEffect::Gargle) == SoundEffect::Gargle) {
        compositeEffects.push_back(new GargleSoundEffect(parameterMap));
    }

    if (compositeEffects.size() == 1) {
        fullResult = compositeEffects.front();
        assert(fullResult != NULL);
    }
    else if (compositeEffects.size() > 1) {
        fullResult = CompositeSoundEffect::Build(parameterMap, compositeEffects);
        assert(fullResult != NULL);
    }
    
    return fullResult;
}

// Gets the mask for creating a composite sound effect from the given effects strings
size_t SoundEffect::GetEffectFlagsFromStrList(const std::vector<std::string>& effectsStrs) {

    size_t flagResult = SoundEffect::None;

    for (std::vector<std::string>::const_iterator iter = effectsStrs.begin(); iter != effectsStrs.end(); ++iter) {
        const std::string& currStr = *iter;
        if (currStr.compare(SoundEffect::REVERB3D_STR) == 0) {
            flagResult |= SoundEffect::Reverb3D;
        }
        else if (currStr.compare(SoundEffect::DISTORTION_STR) == 0) {
            flagResult |= SoundEffect::Distortion;
        }
        else if (currStr.compare(SoundEffect::GARGLE_STR) == 0) {
            flagResult |= SoundEffect::Gargle;
        }
    }

    return flagResult;
}

void SoundEffect::ToggleEffectOnSound(Sound* sound, bool effectOn) {
    assert(sound != NULL);
    sound->Visit(*this, effectOn);
}

void SoundEffect::ToggleEffect(irrklang::ISound* sound, bool effectOn) {
    if (sound == NULL) {
        assert(false);
        return;
    }

    irrklang::ISoundEffectControl* soundEffectCtrl = sound->getSoundEffectControl();
    if (soundEffectCtrl == NULL) {
        return;
    }

    if (effectOn) {
        sound->setVolume(sound->getVolume() * this->volume);
        this->EnableEffect(*soundEffectCtrl);
    }
    else {
        sound->setVolume(std::min<float>(1.0f, sound->getVolume() * (1.0f / this->volume)));
        this->DisableEffect(*soundEffectCtrl);
    }
}