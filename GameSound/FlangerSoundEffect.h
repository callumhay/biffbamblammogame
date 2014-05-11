/**
 * FlangerSoundEffect.h
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

#ifndef __FLANGERSOUNDEFFECT_H__
#define __FLANGERSOUNDEFFECT_H__

#include "SoundEffect.h"

class FlangerSoundEffect : public SoundEffect {
public:
    FlangerSoundEffect(const EffectParameterMap& parameterMap);
    ~FlangerSoundEffect();

private:
    float wetDryMix;     // Ratio of wet (processed) signal to dry (unprocessed) signal. Minimal Value:0, Maximal Value:100.0f;
    float depth;         // Percentage by which the delay time is modulated by the low-frequency oscillator, in hundredths of a percentage point. Minimal Value:0, Maximal Value:100.0f;
    float feedback;      // Percentage of output signal to feed back into the effect's input. Minimal Value:-99, Maximal Value:99.0f;
    float frequency;     // Frequency of the LFO. Minimal Value:0, Maximal Value:10.0f;
    bool isTriangleWave; // True for triangle wave form, false for square.
    float delay;         // Number of milliseconds the input is delayed before it is played back. Minimal Value:0, Maximal Value:20.0f;

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(FlangerSoundEffect);
};

#endif // __FLANGERSOUNDEFFECT_H__