/**
 * Reverb3DSoundEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include <irrKlang.h>

#include "Reverb3DSoundEffect.h"
#include "Sound.h"

Reverb3DSoundEffect::Reverb3DSoundEffect(const EffectParameterMap& parameterMap) : SoundEffect(parameterMap) { 

    // Parse relevant parameters from the given parameterMap
    SoundEffect::ParseSoundParameter<int>(parameterMap,   "R3DRoomAtten", -10000, 0, -1000, this->roomAtten);
    SoundEffect::ParseSoundParameter<int>(parameterMap,   "R3DRoomHighFreqAtten", -10000, 0, -100, this->roomHighFreqAtten);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DRoomRolloff", 0.0f, 10.0f, 0.0f, this->roomRolloff);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DDecayTime", 0.1f, 20.0f, 1.49f, this->decayTime);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DDecayHighFreqRatio", 0.1f, 2.0f, 0.83f, this->decayHighFreqRatio);
    SoundEffect::ParseSoundParameter<int>(parameterMap,   "R3DReflectionAtten", -10000, 1000, -2602, this->reflectionAtten);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DReflectionDelay", 0.0f, 0.3f, 0.007f, this->reflectionDelay);
    SoundEffect::ParseSoundParameter<int>(parameterMap,   "R3DReverbAtten", -10000, 2000, 200, this->reverbAtten);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DReverbDelay", 0.0f, 0.1f, 0.011f, this->reverbDelay);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DDiffusion", 0.0f, 100.0f, 100.0f, this->diffusion);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DDensity", 0.0f, 100.0f, 100.0f, this->density);
    SoundEffect::ParseSoundParameter<float>(parameterMap, "R3DHighFreqReference", 20.0f, 20000.0f, 5000.0f, this->highFreqReference);
}

Reverb3DSoundEffect::~Reverb3DSoundEffect() {
}

bool Reverb3DSoundEffect::EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    return soundEffectCtrl.enableI3DL2ReverbSoundEffect(this->roomAtten, this->roomHighFreqAtten,
        this->roomRolloff, this->decayTime, this->decayHighFreqRatio, this->reflectionAtten, this->reflectionDelay,
        this->reverbAtten, this->reverbDelay, this->diffusion, this->density, this->highFreqReference);
}

bool Reverb3DSoundEffect::DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const {
    soundEffectCtrl.disableI3DL2ReverbSoundEffect();
    return true;
}