/**
 * SoundEffect.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <irrKlang.h>

#include "SoundEffect.h"
#include "Sound.h"
#include "CompositeSoundEffect.h"
#include "Reverb3DSoundEffect.h"
#include "DistortionSoundEffect.h"
#include "GargleSoundEffect.h"
#include "FlangerSoundEffect.h"
#include "ReverbWaveSoundEffect.h"
#include "ChorusSoundEffect.h"
//#include "CompressorSoundEffect.h"

const char* SoundEffect::REVERB3D_STR    = "reverb3D";
const char* SoundEffect::DISTORTION_STR  = "distortion";
const char* SoundEffect::GARGLE_STR      = "gargle";
const char* SoundEffect::FLANGER_STR     = "flanger";
const char* SoundEffect::REVERB_WAVE_STR = "reverbWave";
const char* SoundEffect::CHORUS_STR      = "chorus";
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
    if ((typeFlags & SoundEffect::Flanger) == SoundEffect::Flanger) {
        compositeEffects.push_back(new FlangerSoundEffect(parameterMap));
    }
    if ((typeFlags & SoundEffect::ReverbWave) == SoundEffect::ReverbWave) {
        compositeEffects.push_back(new ReverbWaveSoundEffect(parameterMap));
    }
    if ((typeFlags & SoundEffect::Chorus) == SoundEffect::Chorus) {
        compositeEffects.push_back(new ChorusSoundEffect(parameterMap));
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
        else if (currStr.compare(SoundEffect::FLANGER_STR) == 0) {
            flagResult |= SoundEffect::Flanger;
        }
        else if (currStr.compare(SoundEffect::REVERB_WAVE_STR) == 0) {
            flagResult |= SoundEffect::ReverbWave;
        }
        else if (currStr.compare(SoundEffect::CHORUS_STR) == 0) {
            flagResult |= SoundEffect::Chorus;
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