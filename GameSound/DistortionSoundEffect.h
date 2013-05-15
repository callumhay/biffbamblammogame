/**
 * DistortionSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __DISTORTIONSOUNDEFFECT_H__
#define __DISTORTIONSOUNDEFFECT_H__

#include "SoundEffect.h"

class DistortionSoundEffect : public SoundEffect {
public:
    DistortionSoundEffect(const EffectParameterMap& parameterMap);
    ~DistortionSoundEffect();

private:
    float gain;
    float intensity;
    float postEQCenterFreq;
    float postEQBandwidth;
    float preLowpassCutoff;

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(DistortionSoundEffect);
};

#endif // __DISTORTIONSOUNDEFFECT_H__
