/**
 * ReverbWaveSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __REVERBWAVESOUNDEFFECT_H__
#define __REVERBWAVESOUNDEFFECT_H__

#include "SoundEffect.h"

class ReverbWaveSoundEffect : public SoundEffect {
public:
    ReverbWaveSoundEffect(const EffectParameterMap& parameterMap);
    ~ReverbWaveSoundEffect();

private:
    float inputGain;        // Input gain of signal, in decibels (dB). Min/Max: [-96.0,0.0] Default: 0.0 dB
    float reverbMix;        // Reverb mix, in dB. Min/Max: [-96.0,0.0] Default: 0.0 dB
    float reverbTime;       // Reverb time, in milliseconds. Min/Max: [0.001,3000.0] Default: 1000.0 ms
    float highFreqRatio;    // High-frequency reverb time ratio. Min/Max: [0.001,0.999] Default: 0.001

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(ReverbWaveSoundEffect);
};

#endif // __REVERBWAVESOUNDEFFECT_H__