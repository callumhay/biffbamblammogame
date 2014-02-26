/**
 * DistortionSoundEffect.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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