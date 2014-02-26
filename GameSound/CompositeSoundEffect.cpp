/**
 * CompositeSoundEffect.cpp
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
