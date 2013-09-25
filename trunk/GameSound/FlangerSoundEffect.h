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