/**
 * ChorusSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CHORUSSOUNDEFFECT_H__
#define __CHORUSSOUNDEFFECT_H__

#include "SoundEffect.h"

class ChorusSoundEffect : public SoundEffect {
public:
    ChorusSoundEffect(const EffectParameterMap& parameterMap);
    ~ChorusSoundEffect();

private:
    float wetDryMix;
    float depth;
    float feedback;
    float frequency;
    bool isSinWave;
    float delay;

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(ChorusSoundEffect);
};

#endif // __CHORUSSOUNDEFFECT_H__